#pragma once
#include <string>
#include <vector>

struct StudentRecord {
    long long student_id;
    std::string name;
    std::string gender;
    float gpa;
    float height;
    float weight;
};

void load_csv(const std::string& filename,std::vector<StudentRecord>& data);