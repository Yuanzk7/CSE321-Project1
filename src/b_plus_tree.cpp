#include "b_plus_tree.h"
#include <vector>
#include <iostream>

b_plus_tree::b_plus_tree(int order) : btree(order) {
    root = nullptr;
    d = order;
    split_count = 0;
}

void b_plus_tree::insert(long long key, int rid) {
    if(root == nullptr) { // 첫 삽입
        root = new b_node(d,true,true);
        root->keys[0] = key;
        root->rids[0] = rid;
        root->current_count++;
        return;
    } 

    b_node* curr = root;
    while(!curr->isleaf){ // 리프노드까지 내려가 삽입할 위치 탐색
        int i=0;
        while(i<curr->current_count && key >= curr->keys[i]) i++;
        curr = curr -> children[i];
    }

    int j = curr->current_count-1;
    while(j >= 0 && curr->keys[j] > key){
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

b_node* b_plus_tree::split(b_node* curr) {
    if(curr->isleaf){ // 리프노드가 쪼개질 때
        long long median_key = curr->keys[d/2];
        b_node* right = new b_node(d,true,true);

        int j = 0;
        for(int i=d/2;i<d;i++){
            right->keys[j] = curr->keys[i];
            right->rids[j] = curr->rids[i];
            j++;
        }
        curr->current_count = d/2; 
        right->current_count = j;

        right->next = curr->next;
        curr->next = right;

        // 중간값을 부모 노드에 복사
        if(curr == root) { 
            b_node* new_root = new b_node(d, false, true);

            new_root->keys[0] = median_key;

            new_root->children[0] = curr;
            new_root->children[1] = right;
            new_root->current_count = 1;

            curr->parent = new_root;
            right->parent = new_root;
            root = new_root;
            this->split_count++;
            return nullptr; 
        }
        else{
            b_node* p = curr->parent;
            right->parent = p;

            int k = p->current_count - 1;
            while (k >= 0 && p->keys[k] > median_key) {
                p->keys[k+1] = p->keys[k];
                p->children[k+2] = p->children[k+1];
                k--;
            }
            p->keys[k+1] = median_key;
            p->children[k+2] = right; 
            p->current_count++;
            this->split_count++;
            return p;
        }
    }
    else{ // 내부노드가 쪼개질 때
        long long median_key = curr->keys[d/2];
        b_node* right = new b_node(d, false,true);

        int j = 0;
        for(int i = d/2+1; i < d; i++) {
            right->keys[j] = curr->keys[i];
            right->children[j] = curr->children[i];
            if (right->children[j] != nullptr) {
                right->children[j]->parent = right;
            }
            curr->children[i] = nullptr;
            j++;
        }

        right->children[j] = curr->children[d];
        if (right->children[j] != nullptr) {
            right->children[j]->parent = right;
        }

        curr->children[d] = nullptr;
        curr->current_count = d/2;
        right->current_count = j;

        if(curr == root) { 
            b_node* new_root = new b_node(d, false,true);

            new_root->keys[0] = median_key;

            new_root->children[0] = curr;
            new_root->children[1] = right;
            new_root->current_count = 1;

            curr->parent = new_root;
            right->parent = new_root;
            root = new_root;
            this->split_count++;
            return nullptr; 
        }
        else{
            b_node* p = curr->parent;
            right->parent = p;

            int k = p->current_count-1;
            while (k >= 0 && p->keys[k] > median_key) { 
                p->keys[k+1] = p->keys[k];
                p->children[k+2] = p->children[k+1];
                k--;
            }
            p->keys[k+1] = median_key;
            p->children[k+2] = right;
            p->current_count++;
            return p;
        }
    }
}

void b_plus_tree::remove(long long key) {
    b_node* target = root;
    int min_key = (d+1)/2 - 1;
    int i,j;
    while(!target->isleaf){ // 삭제할 키가 있는 리프노드의 위치 탐색
        i = 0;
        while(i < target->current_count && key >= target->keys[i]) i++;
        target = target->children[i];
    }

    for(j = 0;j < target->current_count;j++){
        if(target->keys[j] == key) break;
        if(target->keys[j] > key) return;
    }
    if(j == target->current_count) return;

    for(;j < target->current_count-1;j++){ // target의 데이터 삭제
        target->keys[j] = target->keys[j+1];
        target->rids[j] = target->rids[j+1];
    }
    target->current_count--;

    while(target != root && target->current_count < min_key){ // underflow 발생
        b_node* p = target->parent;
        b_node* left = nullptr;
        b_node* right = nullptr;
        int child_index = 0;
        while(child_index <= p->current_count && p->children[child_index] != target) child_index++;
        
        if(child_index > 0) left = p->children[child_index-1];
        if(child_index < p->current_count) right = p->children[child_index+1];

        if(left != nullptr && left->current_count > min_key){ // 왼쪽 형제에서 빌려올 수 있는 경우
            borrow_Left(target,left,p,child_index);
            break;
        }
        else if(right != nullptr && right->current_count > min_key){ // 오른쪽 형제에서 빌려올 수 있는 경우
            borrow_Right(target,right,p,child_index);
            break;
        }
        else{ // 두 형제에게 모두 빌려올 수 없는 경우
            if(left!=nullptr){ // 왼쪽 형제랑 병합
                merge(left,target,p,child_index-1);
            }
            else{ // 오른쪽 형제랑 병합
                merge(target,right,p,child_index);
            }
            target = p;
        }
    }
    if(target == root && target->current_count == 0) { // target이 루트노드인데 빈 경우
        b_node* old_root = root;

        if(old_root->isleaf){
            root = nullptr;
        }
        else{
            root = old_root->children[0];
            if(root != nullptr){
                root->parent = nullptr;
            }
        }
        delete[] old_root->keys;
        delete[] old_root->rids;
        delete[] old_root->children;
        delete old_root;
    }
}

void b_plus_tree::borrow_Left(b_node* target, b_node* left, b_node* parent, int child_index) {
    if(target->isleaf){
        for(int i = target->current_count-1;i >= 0;i--){
            target->keys[i+1] = target->keys[i];
            target->rids[i+1] = target->rids[i];
        }

        target->keys[0] = left->keys[left->current_count-1];
        target->rids[0] = left->rids[left->current_count-1];
        target->current_count++;
        left->current_count--;

        parent->keys[child_index-1] = target->keys[0]; 
    }
    else{
        for(int i = target->current_count-1;i >= 0;i--){
            target->keys[i+1] = target->keys[i];
            target->children[i+2] = target->children[i+1];
        }
        target->children[1] = target->children[0];

        target->keys[0] = parent->keys[child_index-1];
        parent->keys[child_index-1] = left->keys[left->current_count-1];

        target->children[0] = left->children[left->current_count];
        if(target->children[0]!=nullptr){
            target->children[0]->parent = target;
        }
        target->current_count++;
        left->current_count--;
    }
}
void b_plus_tree::borrow_Right(b_node* target, b_node* right, b_node* parent, int child_index) {
    if(target->isleaf){
        target->keys[target->current_count] = right->keys[0];
        target->rids[target->current_count] = right->rids[0];
        for(int i = 0;i < right->current_count-1;i++){ 
            right->keys[i] = right->keys[i+1];
            right->rids[i] = right->rids[i+1];
        }
        target->current_count++;
        right->current_count--;

        parent->keys[child_index] = right->keys[0]; 
    }
    else{
        target->keys[target->current_count] = parent->keys[child_index];
        parent->keys[child_index] = right->keys[0];

        target->children[target->current_count+1] = right->children[0];
        if(target->children[target->current_count+1]!=nullptr){
            target->children[target->current_count+1]->parent = target;
        }

        for(int i = 0;i < right->current_count-1;i++){
            right->keys[i] = right->keys[i+1];
            right->children[i] = right->children[i+1];
        }
        right->children[right->current_count-1] = right->children[right->current_count];

        target->current_count++;
        right->current_count--;
    }
}

void b_plus_tree::merge(b_node* left, b_node* right, b_node* parent, int parent_index) {
    if(left->isleaf){ // 리프노드인 경우
        for(int i = 0;i < right->current_count;i++){
            left->keys[i+left->current_count] = right->keys[i];
            left->rids[i+left->current_count] = right->rids[i];
        }
        left->current_count += right->current_count;

        left->next = right->next;

        for(int i = parent_index;i < parent->current_count-1;i++){ // 오른쪽 노드를 가리키던 키 삭제
            parent->keys[i] = parent->keys[i+1];
            parent->children[i+1] = parent->children[i+2];
        }
        parent->current_count--;

        // 오른쪽 노드 메모리 해제
        delete[] right->keys;
        delete[] right->rids;
        delete[] right->children;
        delete right;
    }
    else { // 내부노드인 경우
        left->keys[left->current_count] = parent->keys[parent_index];
        left->current_count++;

        // 오른쪽 노드의 모든 key와 자식 포인터를 왼쪽 노드로 이동
        for (int i = 0; i < right->current_count; i++) {
            left->keys[left->current_count] = right->keys[i];
            
            left->children[left->current_count] = right->children[i];
            if (left->children[left->current_count] != nullptr) {
                left->children[left->current_count]->parent = left;
            }
            left->current_count++;
        }
        
        // 오른쪽 노드의 마지막 자식 포인터 처리
        left->children[left->current_count] = right->children[right->current_count];
        if (left->children[left->current_count] != nullptr) {
            left->children[left->current_count]->parent = left;
        }

        // 부모 노드 재정렬
        for (int i = parent_index; i < parent->current_count - 1; i++) {
            parent->keys[i] = parent->keys[i+1];
            parent->children[i+1] = parent->children[i+2];
        }
        parent->current_count--;

        // 오른쪽 노드 메모리 해제
        delete[] right->keys;
        delete[] right->rids;
        delete[] right->children;
        delete right;
    }
}

int b_plus_tree::search(long long key) {
    b_node* curr = root;

    while(!curr->isleaf){
        int i = 0;
        while(i < curr->current_count && key >= curr->keys[i]) i++;
        curr = curr->children[i];
    }
    
    for(int j = 0;j < curr->current_count;j++){
        if(curr->keys[j] == key) return curr->rids[j];
        if(curr->keys[j] > key) return -1;
    }
    return -1;
}

std::vector<int> b_plus_tree::range_query(long long start_key, long long end_key) {
    std::vector<int> result;
    b_node* curr = root;

    while(!curr->isleaf){
        int i = 0;
        while(i < curr->current_count && start_key > curr->keys[i]) i++;
        curr = curr->children[i];
    }
    int j = 0;
    bool flag = true;
    while(curr != nullptr && flag){
        for(int i = 0;i < curr->current_count;i++){
            if(curr->keys[i] > end_key){
                flag = false;
                break;
            }
            else if(curr->keys[i] >= start_key){
                result.push_back(curr->rids[i]);
                j++;
            }
        }
        if(!flag) break;
        curr = curr->next;
    }
    return result;
}



