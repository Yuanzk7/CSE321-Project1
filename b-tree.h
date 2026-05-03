#pragma once
#include <vector>

struct b_node {
    long long* keys;
    int* rids;
    b_node** children;
    b_node* parent;
    int current_count;
    bool isleaf;
    b_node* next;

    b_node(int d,bool leaf,bool is_b_plus);
};

class btree {
    protected:
        b_node* root;
        int d;
        int split_count;
        virtual b_node* split(b_node* curr);
        b_node* find_Successor(b_node* curr, int index);
        void borrow_Left(b_node* target, b_node* left, b_node* parent, int index);
        void borrow_Right(b_node* target, b_node* right, b_node* parent, int index);
        void merge(b_node* left, b_node* right, b_node* parent, int index);
        void range_recursive(b_node* curr, long long start_key, long long end_key, std::vector<int>& result);
        void calculate_utilization(b_node* curr, long long& total_keys, long long& total_nodes);
    public:
        btree(int d);
        virtual ~btree();
        void clear(b_node* node);
        virtual void insert(long long key,int rid);
        virtual void remove(long long key);
        virtual int search(long long key);
        virtual std::vector<int> range_query(long long start_key, long long end_key);
        double get_node_utilization();
        int get_split_count();
};