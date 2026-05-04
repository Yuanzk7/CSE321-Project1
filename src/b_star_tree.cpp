#include "b_star_tree.h"

b_star_tree::b_star_tree(int order) : btree(order) {
    root = nullptr;
    d = order;
    split_count = 0;
}

void b_star_tree::insert(long long key,int rid) {
    if(root == nullptr){ // 첫 삽입
        root = new b_node(d,true,false);
        root->keys[0] = key;
        root->rids[0] = rid;
        root->current_count++;
        return;
    }

    b_node* curr = root;
    while(!curr->isleaf){ // 리프노드까지 내려가 삽입할 위치 탐색
        int i = 0;
        while(i<curr->current_count && key > curr->keys[i]) i++;
        curr = curr -> children[i];
    }

    int j = curr->current_count-1; // 해당 노드의 키값들을 밀어내서 키를 삽입함.
    while(j >= 0 && curr->keys[j] > key){ 
        curr->keys[j+1] = curr->keys[j];
        curr->rids[j+1] = curr->rids[j];
        j--;
    }
    curr->keys[j+1] = key;
    curr->rids[j+1] = rid;
    curr->current_count++;

    if(curr->current_count < d) return; // overflow가 발생하지 않았다면 종료

    while(curr->current_count == d) {
        if(curr == root){ // 루트일경우는 b-tree와 똑같이 split 진행
            btree::split(curr);
            return;
        }
        else{ // 루트가 아닐경우
            b_node* p = curr->parent;
            b_node* left = nullptr;
            b_node* right = nullptr;
            int child_index = 0;
            while(child_index < p->current_count && p->children[child_index] != curr) child_index++;

            if(child_index > 0) left = p->children[child_index-1];
            if(child_index < p->current_count) right = p->children[child_index+1];

            if(left != nullptr && left->current_count < d-1){ // 왼쪽 형제에게 key를 줄 수 있는 경우
                give_Left(curr,left,p,child_index);
                break;
            }
            else if(right != nullptr && right->current_count < d-1){ // 오른쪽 형제에게 key를 줄 수 있는 경우
                give_Right(curr,right,p,child_index);
                break;
            }
            else{ // 두 형제 모두에게 줄 수 없는 경우
                if(left != nullptr) {
                    curr = split2to3(left,curr,p,child_index-1);
                }
                else if(right != nullptr) {
                    curr = split2to3(curr,right,p,child_index);
                }
            }
        }
    }
}

void b_star_tree::give_Left(b_node* curr, b_node* left, b_node* parent, int child_index) {
    left->keys[left->current_count] = parent->keys[child_index-1];
    left->rids[left->current_count] = parent->rids[child_index-1];
    
    parent->keys[child_index-1] = curr->keys[0];
    parent->rids[child_index-1] = curr->rids[0];

    if(!curr->isleaf) {
        left->children[left->current_count+1] = curr->children[0];
        if(left->children[left->current_count+1] != nullptr) {
            left->children[left->current_count+1]->parent = left;
        }  
    }

    for(int i = 0; i < curr->current_count-1; i++) {
        curr->keys[i] = curr->keys[i+1];
        curr->rids[i] = curr->rids[i+1];
    }
    if(!curr->isleaf) {
        for (int i = 0; i < curr->current_count; i++) {
            curr->children[i] = curr->children[i+1];
        }
    }

    curr->current_count--;
    left->current_count++;
}
void b_star_tree::give_Right(b_node* curr, b_node* right, b_node* parent, int child_index) {
    for(int i = right->current_count-1; i >= 0; i--) {
        right->keys[i+1] = right->keys[i];
        right->rids[i+1] = right->rids[i];
    }

    if(!right->isleaf) {
        for(int i = right->current_count; i >= 0; i--) {
            right->children[i+1] = right->children[i];
        }
    }

    right->keys[0] = parent->keys[child_index];
    right->rids[0] = parent->rids[child_index];
    
    parent->keys[child_index] = curr->keys[curr->current_count-1];
    parent->rids[child_index] = curr->rids[curr->current_count-1];

    if(!curr->isleaf) {
        right->children[0] = curr->children[curr->current_count];
        if (right->children[0] != nullptr) {
            right->children[0]->parent = right;
        }
    }

    right->current_count++;
    curr->current_count--;
}

void b_star_tree::remove(long long key) {
    b_node* target = root;
    int i;
    int target_index;
    int min_key = (2*d-1) / 3; // underflow 조건 
    
    while(target != nullptr){ // 삭제할 key가 있는 노드의 위치 탐색
        i = 0;
        while(i < target->current_count && key > target->keys[i]) i++;

        if(i < target->current_count && key == target->keys[i]) break;
        if(target->isleaf) return;
        target = target->children[i];
    }
    if(target == nullptr) return;

    if(!target->isleaf){ // 내부 노드인경우
        target = btree::find_Successor(target,i);
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

    while(target != root){

        if (target->parent->current_count == 1) min_key = (d-1) / 2; // 자식이 2개뿐일 때는 b-tree 최소 키 조건 적용
        else min_key = (2*d-1) / 3;

        if(target->current_count >= min_key) break; // underflow가 아니라면 종료

        b_node* p = target->parent;
        b_node* left = nullptr;
        b_node* right = nullptr;
        int child_index = 0;
        while(child_index < p->current_count && p->children[child_index] != target) child_index++; // target이 부모의 몇번째 자식인지 확인

        if(child_index > 0) left = p->children[child_index-1];
        if(child_index < p->current_count) right = p->children[child_index+1];

        if (left != nullptr && left->current_count > min_key) { // 왼쪽 형제에서 빌려올 수 있는 경우
            btree::borrow_Left(target, left, p, child_index);
            break;
        }
        else if (right != nullptr && right->current_count > min_key) { // 오른쪽 형제에서 빌려올 수 있는 경우
            btree::borrow_Right(target, right, p, child_index);
            break;
        }
        else{ // 두 형제에게 모두 빌려올 수 없는경우
            if(p->current_count >= 2) { 
                if(child_index == 0) { // target이 맨 왼쪽 자식
                    merge3to2(target, p->children[1], p->children[2], p, 0);
                } 
                else if(child_index == p->current_count) { // target이 맨 오른쪽 자식
                    merge3to2(p->children[child_index - 2], p->children[child_index - 1], target, p, child_index - 2);
                } 
                else { // target이 중간 자식
                    merge3to2(left, target, right, p, child_index - 1);
                }
            }
            else{ // 부모의 자식이 2개뿐일 때는 b-tree의 병합과 동일하게 진행
                if(left != nullptr){ // 왼쪽 형제랑 병합
                    merge(left,target,p,child_index-1);
                }
                else{ // 오른쪽 형제랑 병합
                    merge(target,right,p,child_index);
                }
            }
            target = p;
        }
    }
    if(target == root && target->current_count == 0){ // target이 루트노드인데 빈 경우
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

b_node* b_star_tree::split2to3(b_node* left, b_node* right, b_node* parent, int child_index) {
    long long* temp_keys = new long long[2*d+1]; 
    int* temp_rids = new int[2*d+1];
    b_node** temp_children = new b_node*[2*d +2];

    int temp_k = 0;
    int temp_c = 0;

    // 왼쪽 노드 병합
    for(int i = 0; i < left->current_count; i++){
        temp_keys[temp_k] = left->keys[i];
        temp_rids[temp_k++] = left->rids[i];
        temp_children[temp_c++] = left->children[i];
    }
    temp_children[temp_c++] = left->children[left->current_count];

    // 부모의 데이터 하나 병합
    temp_keys[temp_k] = parent->keys[child_index];
    temp_rids[temp_k++] = parent->rids[child_index];

    // 오른쪽 노드 병합
    temp_children[temp_c++] = right->children[0]; 
    for(int i = 0; i < right->current_count; i++){
        temp_keys[temp_k] = right->keys[i];
        temp_rids[temp_k++] = right->rids[i];
        temp_children[temp_c++] = right->children[i+1];
    }

    int total = temp_k; 
    int num1 = total/3; // 첫 번째 노드가 가져갈 데이터의 개수                      
    int num2 = (total-num1-1)/2; // 두 번째 노드가 가져갈 데이터의 개수
    b_node* new_node = new b_node(d, left->isleaf, false);

    // 첫 번째 노드
    for(int i = 0; i < num1; i++){ 
        left->keys[i] = temp_keys[i];
        left->rids[i] = temp_rids[i];
        left->children[i] = temp_children[i];
        if(!left->isleaf && left->children[i] != nullptr) left->children[i]->parent = left;
    }
    left->current_count = num1;
    left->children[num1] = temp_children[num1];
    if(!left->isleaf && left->children[num1] != nullptr) left->children[num1]->parent = left;

    // 두 번째 노드
    for(int i = 0; i < num2; i++){ 
        int k = num1 + 1 + i;
        new_node->keys[i] = temp_keys[k];
        new_node->rids[i] = temp_rids[k];
        new_node->children[i] = temp_children[k];
        if(!new_node->isleaf && new_node->children[i] != nullptr) new_node->children[i]->parent = new_node; 
        new_node->current_count++;
    }
    new_node->children[new_node->current_count] = temp_children[num1+num2+1];
    if(!new_node->isleaf && new_node->children[new_node->current_count] != nullptr) new_node->children[new_node->current_count]->parent = new_node;

    // 세 번째 노드
    int t = 0;
    for(int i = num1 + num2 + 2; i < total; i++){ 
        right->keys[t] = temp_keys[i];
        right->rids[t] = temp_rids[i];
        right->children[t] = temp_children[i];
        if(!right->isleaf && right->children[t] != nullptr) right->children[t]->parent = right;
        t++;
    }
    right->current_count = t;
    right->children[t] = temp_children[total];
    if(!right->isleaf && right->children[t] != nullptr) right->children[t]->parent = right;

    // 부모 노드
    parent->children[parent->current_count + 1] = parent->children[parent->current_count];
    for(int i = parent->current_count-1; i >= child_index+1; i--){ 
        parent->keys[i+1] = parent->keys[i];
        parent->rids[i+1] = parent->rids[i];
        parent->children[i+1] = parent->children[i];
    }

    parent->keys[child_index] = temp_keys[num1];
    parent->rids[child_index] = temp_rids[num1];

    parent->keys[child_index+1] = temp_keys[num1+num2+1];
    parent->rids[child_index+1] = temp_rids[num1+num2+1];
    
    parent->children[child_index+1] = new_node;
    new_node->parent = parent;
    parent->current_count++;

    // 임시 배열 메모리 해제
    delete[] temp_keys;
    delete[] temp_rids;
    delete[] temp_children;
    this->split_count++;
    
    return parent;
}
void b_star_tree::merge3to2(b_node* left, b_node* middle, b_node* right, b_node* parent, int child_index) {
    long long* temp_keys = new long long[3*d]; 
    int* temp_rids = new int[3*d];
    b_node** temp_children = new b_node*[3*d+3];

    int temp_k = 0;
    int temp_c = 0;

    // 왼쪽 노드 병합
    for(int i = 0; i < left->current_count; i++) {
        temp_keys[temp_k] = left->keys[i];
        temp_rids[temp_k++] = left->rids[i];
        temp_children[temp_c++] = left->children[i];
    }
    temp_children[temp_c++] = left->children[left->current_count];

    // 부모의 데이터 하나 병합
    temp_keys[temp_k] = parent->keys[child_index];
    temp_rids[temp_k++] = parent->rids[child_index];

    // 중간 노드 병합
    temp_children[temp_c++] = middle->children[0];
    for(int i = 0; i < middle->current_count; i++) {
        temp_keys[temp_k] = middle->keys[i];
        temp_rids[temp_k++] = middle->rids[i];
        temp_children[temp_c++] = middle->children[i + 1];
    }

    // 부모의 데이터 하나 병합
    temp_keys[temp_k] = parent->keys[child_index + 1];
    temp_rids[temp_k++] = parent->rids[child_index + 1];

    // 오른쪽 노드 병합
    temp_children[temp_c++] = right->children[0];
    for(int i = 0; i < right->current_count; i++) {
        temp_keys[temp_k] = right->keys[i];
        temp_rids[temp_k++] = right->rids[i];
        temp_children[temp_c++] = right->children[i + 1];
    }
    
    int total = temp_k; 
    int num1 = total/3; // 첫 번째 노드가 가져갈 데이터의 개수                      
    int num2 = (total-num1-1)/2; // 두 번째 노드가 가져갈 데이터의 개수

    // 첫 번째 노드
    for(int i = 0; i < num1; i++) {
        left->keys[i] = temp_keys[i];
        left->rids[i] = temp_rids[i];
        left->children[i] = temp_children[i];
        if (!left->isleaf && left->children[i] != nullptr) left->children[i]->parent = left;
    }
    left->current_count = num1;
    left->children[num1] = temp_children[num1];
    if (!left->isleaf && left->children[num1] != nullptr) left->children[num1]->parent = left;

    parent->keys[child_index] = temp_keys[num1];
    parent->rids[child_index] = temp_rids[num1];

    // 두 번째 노드
    middle->current_count = 0;
    for(int i = 0; i < num2; i++) {
        int k = num1 + 1 + i;
        middle->keys[i] = temp_keys[k];
        middle->rids[i] = temp_rids[k];
        middle->children[i] = temp_children[k];
        if (!middle->isleaf && middle->children[i] != nullptr) middle->children[i]->parent = middle;
        middle->current_count++;
    }
    middle->children[num2] = temp_children[total];
    if (!middle->isleaf && middle->children[num2] != nullptr) middle->children[num2]->parent = middle;

    // 부모 노드
    for(int i = child_index+1; i < parent->current_count-1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->rids[i] = parent->rids[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->current_count--;

    // 임시 배열 메모리 해제
    delete[] temp_keys; 
    delete[] temp_rids; 
    delete[] temp_children;

    // 오른쪽 노드 메모리 해제
    delete[] right->keys;
    delete[] right->rids;
    delete[] right->children;
    delete right;
}