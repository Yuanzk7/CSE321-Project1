#include "b-tree.h"

b_node::b_node(int d,bool leaf,bool is_b_plus) {
    this->isleaf = leaf;
    this->current_count = 0;
    this->parent = nullptr;
    this->next = nullptr;
    this->keys = new long long[d+2];
    this->children = new b_node*[d+3];
    if(!is_b_plus || leaf) this->rids = new int[d+2]; // b+ 트리라면 리프일때만 rid 할당
    else this->rids = nullptr;
    for(int i=0;i<=d+2;i++){
        this->children[i] = nullptr;
    }
}

btree::btree(int order) {
    root = nullptr;
    d = order;
    split_count = 0;
}

btree::~btree() {
    clear(root);
}

void btree::clear(b_node* node) { 
    if (node == nullptr) return;

    // 리프가 아니라면 자식들을 먼저 재귀 방식을 통해 삭제
    if (!node->isleaf) { // post-order 방식으로 해제
        for (int i = 0; i <= node->current_count; i++) {
            clear(node->children[i]);
        }
    }

    // 노드 내부의 배열들 삭제
    delete[] node->keys;
    delete[] node->rids;
    delete[] node->children;
    delete node;
}

int btree::get_split_count() {return split_count;} // 노드가 몇 번 분할되었는 지를 반환

double btree::get_node_utilization() { // node utilization 반환 
    if (root == nullptr) return 0;

    long long total_keys = 0;
    long long total_nodes = 0;

    calculate_utilization(root, total_keys, total_nodes);

    double max_keys = (double)(total_nodes * (d - 1));
    
    if (max_keys == 0) return 0;

    return ((double)total_keys / max_keys) * 100.0;
}

void btree::calculate_utilization(b_node* curr, long long& total_keys, long long& total_nodes) {
    if (curr == nullptr) return;

    total_keys += curr->current_count;
    total_nodes += 1;

    if (!curr->isleaf) {
        for (int i = 0; i <= curr->current_count; i++) {
            calculate_utilization(curr->children[i], total_keys, total_nodes);
        }
    }
}

void btree::insert(long long key,int rid) {
    if (root == nullptr) { // 첫 삽입
        root = new b_node(d,true,false);
        root->keys[0] = key;
        root->rids[0] = rid;
        root->current_count++;
        return;
    }

    b_node* curr = root;
    while(!curr->isleaf){ // 리프노드까지 내려가 삽입할 위치 탐색
        int i=0;
        while(i<curr->current_count && key > curr->keys[i]) i++;
        curr = curr -> children[i];
    }

    int j = curr->current_count-1; // 해당 노드의 키값들을 밀어내서 키를 삽입함.
    while(j >= 0 && curr->keys[j] > key) {
        curr->keys[j+1] = curr->keys[j];
        curr->rids[j+1] = curr->rids[j];
        j--;
    }
    curr->keys[j+1] = key;
    curr->rids[j+1] = rid;
    curr->current_count++;

    if(curr->current_count < d) return; // overflow가 발생하지 않았다면 종료

    while(curr != nullptr && curr->current_count == d){ // overflow가 발생했다면 split 과정을 통해 해결
        curr = split(curr);
    }
    return;
}

b_node* btree::split(b_node* curr) {
    long long median_key = curr->keys[d/2];
    int median_rid = curr->rids[d/2];
    b_node* right = new b_node(d, curr->isleaf,false);

    int j = 0;
    for(int i = d/2+1; i < d; i++) { // 오른쪽 노드 생성 후 key 이동
        right->keys[j] = curr->keys[i];
        right->rids[j] = curr->rids[i];
        j++;
    }

    if(!curr->isleaf) { // 분할되는 노드가 내부 노드일 경우 자식들도 이동
        int c = 0;
        for(int i = d/2+1; i <= d; i++) { 
            right->children[c] = curr->children[i];
            if (right->children[c] != nullptr) {
                right->children[c]->parent = right;
            }
            curr->children[i] = nullptr;
            c++;
        }
    }
    
    curr->current_count = d/2;
    right->current_count = j;

    // 중간값을 부모 노드로 이동
    if(curr == root) { // 분할되는 노드가 루트인 경우
        b_node* new_root = new b_node(d, false,false);

        new_root->keys[0] = median_key;
        new_root->rids[0] = median_rid;

        new_root->children[0] = curr;
        new_root->children[1] = right;
        new_root->current_count = 1;

        curr->parent = new_root;
        right->parent = new_root;
        root = new_root;
        this->split_count++;
        return nullptr; 
    }
    else { // 분할되는 노드가 내부 노드이거나 리프 노드인 경우
        b_node* p = curr->parent;
        right->parent = p;
        
        int k = p->current_count - 1; 
        while(k >= 0 && p->keys[k] > median_key) {
            p->keys[k+1] = p->keys[k];
            p->rids[k+1] = p->rids[k];
            p->children[k+2] = p->children[k+1];
            k--;
        }
        
        p->keys[k+1] = median_key;
        p->rids[k+1] = median_rid;
        p->children[k+2] = right;
        p->current_count++;
        this->split_count++;
        return p; 
    }
}

void btree::remove(long long key) {
    b_node* target = root;
    int i;
    int target_index;
    int min_key = ((d+1)/2)-1; // underflow 조건 

    while(target!=nullptr){ // 삭제할 노드의 위치 탐색
        i = 0;
        while(i < target->current_count && key > target->keys[i]) i++;

        if(i < target->current_count && key == target->keys[i]) break;
        if(target->isleaf) return;
        target = target->children[i];
    }
    if(!target->isleaf){ // 내부 노드인경우
        target = find_Successor(target,i);
        target_index = 0;
    }
    else{
        target_index = i;
    }

    for (int k = target_index; k < target->current_count-1; k++) { // target의 데이터 삭제
        target->keys[k] = target->keys[k + 1];
        target->rids[k] = target->rids[k + 1];
    }
    target->current_count--;

    while(target != root && target->current_count < min_key){ // underflow 발생
        b_node* p = target->parent;
        b_node* left = nullptr;
        b_node* right = nullptr;
        int child_index = 0;
        while(child_index < p->current_count && p->children[child_index] != target) child_index++; // target이 부모의 몇번째 자식인지 확인

        if(child_index > 0) left = p->children[child_index-1];
        if(child_index < p->current_count) right = p->children[child_index+1];

        if(left != nullptr && left->current_count > min_key) { // 왼쪽 형제에서 빌려올 수 있는 경우
            borrow_Left(target,left,p,child_index);
            break;
        }
        else if(right != nullptr && right->current_count > min_key){ // 오른쪽 형제에서 빌려올 수 있는 경우
            borrow_Right(target,right,p,child_index);
            break;
        }
        else{ // 두형제에게 모두 빌려올 수 없는경우
            if(left!=nullptr){ // 왼쪽 형제랑 병합
               merge(left,target,p,child_index-1);
            }
            else{ // 오른쪽 형제랑 병합
                merge(target,right,p,child_index);
            }
            target = p;
        }  
    }
    if(target == root && target->current_count == 0){ // target이 루트노드인데 빈 경우
        b_node* old_root = root;
        root = old_root->children[0];
        if(root!=nullptr){
            root->parent = nullptr;
        }
        delete[] old_root->keys;
        delete[] old_root->rids;
        delete[] old_root->children;
        delete old_root;
    }
}

b_node* btree::find_Successor(b_node* target, int index) {
    b_node* successor = target->children[index+1];

    while (!successor->isleaf) {
        successor = successor->children[0];
    }


    int delete_key = target->keys[index];
    int delete_rid = target->rids[index];

    target->keys[index] = successor->keys[0];
    target->rids[index] = successor->rids[0];

    successor->keys[0] = delete_key;
    successor->rids[0] = delete_rid;

    return successor;
}

void btree::borrow_Left(b_node* target, b_node* left, b_node* parent, int child_index) {
    for(int i = target->current_count-1; i >= 0; i--) { 
        target->keys[i+1] = target->keys[i];
        target->rids[i+1] = target->rids[i];
    }

    if(!target->isleaf) { 
        for(int i = target->current_count; i >= 0; i--) {
            target->children[i+1] = target->children[i];
        }
    }

    target->keys[0] = parent->keys[child_index-1]; 
    target->rids[0] = parent->rids[child_index-1];
    
    parent->keys[child_index-1] = left->keys[left->current_count-1]; 
    parent->rids[child_index-1] = left->rids[left->current_count-1];

    if(!target->isleaf) { 
        target->children[0] = left->children[left->current_count];
        if(target->children[0] != nullptr) { 
            target->children[0]->parent = target;
        }
    }
    target->current_count++;
    left->current_count--;
}
void btree::borrow_Right(b_node* target, b_node* right, b_node* parent, int child_index) {
    target->keys[target->current_count] = parent->keys[child_index]; 
    target->rids[target->current_count] = parent->rids[child_index];
    
    parent->keys[child_index] = right->keys[0]; 
    parent->rids[child_index] = right->rids[0];

    if(!target->isleaf) { 
        target->children[target->current_count+1] = right->children[0];
        if(target->children[target->current_count+1] != nullptr) { 
            target->children[target->current_count+1]->parent = target;
        }
    }

    for(int i = 0; i < right->current_count-1; i++) { 
        right->keys[i] = right->keys[i+1];
        right->rids[i] = right->rids[i+1];
    }
    if(!(right->isleaf)) { 
        for (int i = 0; i < right->current_count; i++) {
            right->children[i] = right->children[i+1];
        }
    }
    target->current_count++;
    right->current_count--;
}
void btree::merge(b_node* left, b_node* right, b_node* parent, int parent_index) {
    // 부모의 데이터를 왼쪽 노드로 내림
    left->keys[left->current_count] = parent->keys[parent_index];
    left->rids[left->current_count] = parent->rids[parent_index];
    left->current_count++;

    // 오른쪽 노드의 모든 데이터와 자식 포인터를 왼쪽 노드로 이동
    for (int i = 0; i < right->current_count; i++) {
        left->keys[left->current_count] = right->keys[i];
        left->rids[left->current_count] = right->rids[i];
        
        if (!left->isleaf) { 
            left->children[left->current_count] = right->children[i];
            if (left->children[left->current_count] != nullptr) {
                left->children[left->current_count]->parent = left;
            }
        }
        left->current_count++;
    }
    
    // 오른쪽 노드의 마지막 자식 포인터 처리
    if (!(left->isleaf)) {
        left->children[left->current_count] = right->children[right->current_count];
        if (left->children[left->current_count] != nullptr) {
            left->children[left->current_count]->parent = left;
        }
    } 

    // 부모 노드 재정렬
    for (int i = parent_index; i < parent->current_count-1; i++) {
        parent->keys[i] = parent->keys[i+1];
        parent->rids[i] = parent->rids[i+1];
        parent->children[i+1] = parent->children[i+2];
    }
    parent->current_count--;

    // 오른쪽 노드 메모리 해제
    delete[] right->keys;
    delete[] right->rids;
    delete[] right->children;
    delete right;
}

int btree::search(long long key) {
    b_node* curr = root;

    while(curr != nullptr) {
        int i = 0;
        while(i < curr->current_count && key > curr->keys[i]) i++;

        if(i < curr->current_count && key == curr->keys[i]) return curr->rids[i];

        if(curr->isleaf) return -1; // 리프노드까지 도달하였지만 탐색 실패 
        curr = curr->children[i];
    }
    return -1;
}

std::vector<int> btree::range_query(long long start_key, long long end_key) {
    std::vector<int> result;
    range_recursive(root, start_key, end_key, result);
    return result;
}

void btree::range_recursive(b_node* curr, long long start_key, long long end_key, std::vector<int>& result) {
    if (curr == nullptr) return;

    int i = 0;  
    while (i < curr->current_count && curr->keys[i] < start_key) i++;
    while (i < curr->current_count && curr->keys[i] <= end_key) {
        if (!curr->isleaf) {
            range_recursive(curr->children[i], start_key, end_key, result);
        }
        result.push_back(curr->rids[i]);
        i++;
    }
    if (!curr->isleaf) {
        range_recursive(curr->children[i], start_key, end_key, result);
    }
}