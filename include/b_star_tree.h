#pragma once
#include "b-tree.h"

class b_star_tree : public btree {
    protected:
        void give_Left(b_node* curr, b_node* left, b_node* parent, int index);
        void give_Right(b_node* curr, b_node* right, b_node* parent, int index);
        b_node* split2to3(b_node* left, b_node* right, b_node* parent, int child_index);
        void merge3to2(b_node* left, b_node* target, b_node* right, b_node* p, int index);
    public:
        b_star_tree(int order);
        void insert(long long key,int rid) override;
        void remove(long long key) override;
};