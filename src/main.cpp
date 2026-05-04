#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "b_plus_tree.h"
#include "b_star_tree.h"
#include "b-tree.h"
#include "evaluation.h"
#include "student.h"

using namespace std;

int main(int argc, char* argv[]) {

    string mode = argv[1];

    string filename = "data/student.csv";
    vector<StudentRecord> data;
    load_csv(filename, data);
    
    if(data.empty()) {
        cout << "Error: cannot open file" << endl;
        return -1;
    }
    

    ofstream insert_file("result/insert.txt",ios::app);
    ofstream search_file("result/search.txt",ios::app);
    ofstream range_file("result/range.txt",ios::app);
    ofstream delete_file("result/delete.txt",ios::app);

    for(int d: {50,10,5}){
        //cout << "[ Order = " << d << " ]" << "\n";
        btree* new_btree = new btree(d);
        b_plus_tree* new_b_plus_tree = new b_plus_tree(d);
        b_star_tree* new_b_star_tree = new b_star_tree(d);

        if(mode == "insert") {
            insert_file << "==========================\n";
            insert_file << "[ Order = " << d << " ]\n";
            insert_file << "==========================\n";
            evaluate_insertion(new_btree, data, 1, true,insert_file);
            insert_file << "\n";
            evaluate_insertion(new_b_plus_tree, data, 2, true,insert_file);
            insert_file << "\n";
            evaluate_insertion(new_b_star_tree, data, 3, true,insert_file);
            //cout << "\n";
        }
        if(mode != "insert"){ // insert가 아닐때는 단지 삽입만 진행(출력X)
            evaluate_insertion(new_btree, data, 1, false,insert_file);
            evaluate_insertion(new_b_plus_tree, data, 2, false,insert_file);
            evaluate_insertion(new_b_star_tree, data, 3, false,insert_file);
        }
        if(mode == "search") {
            search_file << "==========================\n";
            search_file << "[ Order = " << d << " ]\n";
            search_file << "==========================\n";
            evaluate_point_search(new_btree, data, 10000, 1, search_file);
            search_file << "\n";
            evaluate_point_search(new_b_plus_tree, data, 10000, 2, search_file);
            search_file << "\n";
            evaluate_point_search(new_b_star_tree, data, 10000, 3, search_file);
            //cout << "\n";
        }

        if(mode == "range") {
            range_file << "==========================\n";
            range_file << "[ Order = " << d << " ]\n";
            range_file << "==========================\n";
            evaluate_range_query(new_btree, data, 202100000, 202300000, 1, range_file);
            range_file << "\n";
            evaluate_range_query(new_b_plus_tree, data, 202100000, 202300000, 2, range_file);
            range_file << "\n";
            evaluate_range_query(new_b_star_tree, data, 202100000, 202300000, 3, range_file);
            //cout << "\n";
        }

        if(mode == "delete") {
            delete_file << "==========================\n";
            delete_file << "[ Order = " << d << " ]\n";
            delete_file << "==========================\n";
            evaluate_deletion(new_btree, data, 2000, 1, delete_file);
            delete_file << "\n";
            evaluate_deletion(new_b_plus_tree, data, 2000, 2, delete_file);
            delete_file << "\n";
            evaluate_deletion(new_b_star_tree, data, 2000, 3, delete_file);
            //cout << "\n";
        }
        delete new_btree;
        delete new_b_plus_tree;
        delete new_b_star_tree;
    }
    insert_file.close();
    return 0;
}