#pragma once
#include "b-tree.h"
#include <vector>

class b_plus_tree : public btree{
    protected:
        b_node* split(b_node* curr);
        void borrow_Left(b_node* target, b_node* left, b_node* parent, int child_index);
        void borrow_Right(b_node* target, b_node* right, b_node* parent, int child_index);
        void merge(b_node* left, b_node* right, b_node* parent, int parent_index);
    public:
        b_plus_tree(int order);
        int search(long long key) override;
        void insert(long long key,int rid) override;
        void remove(long long key) override;
        std::vector<int> range_query(long long start_key,long long end_key) override;
};