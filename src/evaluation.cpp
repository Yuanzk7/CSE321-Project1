#include "evaluation.h"
#include <iostream>
#include <ostream>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;

void evaluate_insertion(btree* tree, const vector<StudentRecord>& data,int type,bool printing, ostream& out) {
    if(!printing){
        for(int i = 0; i < 100000; i++) {
            tree->insert(data[i].student_id, i); 
        }
        return;
    }
    switch(type){
        case 1:
            out << "--- [B-tree Insertion Evaluation] ---" << endl;
            //cout << "--- [B-tree Insertion Evaluation] ---" << endl;
            break;
        case 2:
            out << "--- [B+-tree Insertion Evaluation] ---" << endl;
            //cout << "--- [B+-tree Insertion Evaluation] ---" << endl;
            break;
        case 3:
            out << "--- [B*-tree Insertion Evaluation] ---" << endl;
            //cout << "--- [B*-tree Insertion Evaluation] ---" << endl;
            break;
    }
    auto start = chrono::high_resolution_clock::now();
    
    for(int i = 0; i < 100000; i++) {
        tree->insert(data[i].student_id, i); 
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    
    out << "Execution Time: " << duration.count() << " ms" << "\n";
    // cout << "Execution Time: " << duration.count() << " ms" << "\n";
    out << "Total Splits: " << tree->get_split_count() << "\n";
    // cout << "Total Splits: " << tree->get_split_count() << "\n";
    out << "Node Utilization: " << tree->get_node_utilization() << " %" << endl;
    // cout << "Node Utilization: " << tree->get_node_utilization() << " %" << endl;
}

void evaluate_point_search(btree* tree, const vector<StudentRecord>& data, int num_queries,int type, ostream& out) {
    switch(type){
        case 1:
            //cout << "--- [B-tree Point Search Evaluation] ---" << endl;
            out << "--- [B-tree Point Search Evaluation] ---" << endl;
            break;
        case 2:
            //cout << "--- [B+-tree Point Search Evaluation] ---" << endl;
            out << "--- [B+-tree Point Search Evaluation] ---" << endl;
            break;
        case 3:
            //cout << "--- [B*-tree Point Search Evaluation] ---" << endl;
            out << "--- [B*-tree Point Search Evaluation] ---" << endl;
            break;
        default:
            return;
    }
    vector<long long> search_keys;
    mt19937 gen(42); // MT19937 난수 엔진
    uniform_int_distribution<int> dis(0, data.size() - 1);
    for(int i = 0; i < num_queries; i++) {
        search_keys.push_back(data[dis(gen)].student_id);
    }

    auto start = chrono::high_resolution_clock::now();
    for(long long key : search_keys) {
        tree->search(key);
    }
    auto end = chrono::high_resolution_clock::now();
    
    chrono::duration<double, micro> total_duration = end - start;
    double mean_time = total_duration.count() / num_queries;

    //cout << "Mean Execution Time: " << mean_time << " ms" << endl;
    out << "Mean Execution Time: " << mean_time << " us" << endl;
}

void evaluate_range_query(btree* tree, const vector<StudentRecord>& data, long long low, long long high,int type, ostream& out) {
    switch(type){
        case 1:
            //cout << "--- [B-tree Range Query Evaluation] ---" << endl;
            out << "--- [B-tree Range Query Evaluation] ---" << endl;
            break;
        case 2:
            //cout << "--- [B+-tree Range Query Evaluation] ---" << endl;
            out << "--- [B+-tree Range Query Evaluation] ---" << endl;
            break;
        case 3:
            //cout << "--- [B*-tree Range Query Evaluation] ---" << endl;
            out << "--- [B*-tree Range Query Evaluation] ---" << endl;
            break;
        default:
            return;
    }
    vector<int> result_rids;
    double total_gpa = 0;
    double total_height = 0;
    int male_count = 0;

    auto start = chrono::high_resolution_clock::now();
    result_rids = tree->range_query(low, high); 
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;

    for(int rid : result_rids) {
        const auto& student = data[rid];
        if(student.gender == "Male") {
            total_gpa += student.gpa;
            total_height += student.height;
            male_count++;
        }
    }

    //cout << "Execution Time : " << duration.count() << " ms" << "\n";
    out << "Execution Time : " << duration.count() << " ms" << "\n";
    if(male_count > 0) {
        //cout << "Average GPA    : " << total_gpa / male_count << "\n";
        out << "Average GPA    : " << total_gpa / male_count << "\n";
        //cout << "Average Height : " << total_height / male_count << " cm" << endl;
        out << "Average Height : " << total_height / male_count << " cm" << endl;
    }
}

void evaluate_deletion(btree* tree, const vector<StudentRecord>& data, int num_deletes, int type, ostream& out) {
    switch(type){
        case 1:
            //cout << "--- [B-tree Deletion Evaluation] ---" << endl;
            out << "--- [B-tree Deletion Evaluation] ---" << endl;
            break;
        case 2:
            //cout << "--- [B+-tree Deletion Evaluation] ---" << endl;
            out << "--- [B+-tree Deletion Evaluation] ---" << endl;
            break;
        case 3:
            //cout << "--- [B*-tree Deletion Evaluation] ---" << endl;
            out << "--- [B*-tree Deletion Evaluation] ---" << endl;
            break;
        default:
            return;
    }

    vector<int> indices(data.size());
    iota(indices.begin(), indices.end(), 0);

    mt19937 gen(42);
    shuffle(indices.begin(), indices.end(), gen);

    vector<long long> keys_deletes;
    keys_deletes.reserve(num_deletes);

    for(int i = 0; i < num_deletes; i++){
        keys_deletes.push_back(data[indices[i]].student_id);
    }

    auto start = chrono::high_resolution_clock::now();
    for(long long key : keys_deletes){
        tree->remove(key); 
    }
    auto end = chrono::high_resolution_clock::now();
    
    chrono::duration<double, milli> duration = end - start;

    //cout << "Execution Time : " << duration.count() << " ms" << endl;
    out << "Execution Time : " << duration.count() << " ms" << endl;

    bool is_valid = true;
    for(long long key : keys_deletes){
        if(tree->search(key) != -1) { 
            is_valid = false;
            break;
        }
    }
    if(is_valid){
        //cout << "Structural integrity: Pass " << endl;
        out << "Structural integrity: Pass " << endl;
    }
    else{
        //cout << "Structural integrity: Fail " << endl;
        out << "Structural integrity: Fail " << endl;
    }
}