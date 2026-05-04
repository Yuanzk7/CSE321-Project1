#pragma once
#include <vector>
//#include <ostream>
#include "b-tree.h"
#include "b_star_tree.h"
#include "b_plus_tree.h"
#include "student.h"

void evaluate_insertion(btree* tree, const std::vector<StudentRecord>& data,int type, bool printing/*, std::ostream& out*/);
void evaluate_point_search(btree* tree, const std::vector<StudentRecord>& data, int num_queries,int type/*, std::ostream& out*/);
void evaluate_range_query(btree* tree, const std::vector<StudentRecord>& data, long long low, long long high,int type/*, std::ostream& out*/);
void evaluate_deletion(btree* tree, const std::vector<StudentRecord>& data, int num_deletes, int type/*, std::ostream& out*/);