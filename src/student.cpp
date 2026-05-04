#include "student.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

void load_csv(const string& filename, vector<StudentRecord>& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: cannot open file " << filename << endl;
        return;
    }

    string line;
    getline(file, line); 

    while (getline(file, line)) {
        stringstream one_line(line);
        string sub_line;
        StudentRecord record;

        getline(one_line, sub_line, ',');
        record.student_id = stoll(sub_line); 

        getline(one_line, record.name, ',');
        getline(one_line, record.gender, ',');

        getline(one_line, sub_line, ',');
        record.gpa = stof(sub_line); 

        getline(one_line, sub_line, ',');
        record.height = stof(sub_line);

        getline(one_line, sub_line, ',');
        record.weight = stof(sub_line);

        data.push_back(record);
    }

    file.close();
}