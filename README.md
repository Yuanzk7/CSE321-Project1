# CSE321 : Database Systems (Project1)
This project implements and analyzes three tree-based index structures: B-tree, B*-tree, and B+-tree to support search, insertion, and deletion operations.

## 📂 Project Structure
- `src/`: Source code files (`.cpp`)
- `include/`: Header files (`.h`)
- `data/`: Contains `student.csv`
- `result/`: Directory where output files will be saved(`.txt`)
- `Makefile`: Build code

## 🛠️ Environment
* **OS:** Ubuntu 24.04.1 LTS
* **Language:** C++17
* **Compiler:** g++ (GCC)
* **Build Tool:** Make
* **Dataset:** `data/student.csv` (100,000 student records)

## 🔨 How to Compile
To compile the source code and generate the executable named `btree`, run `Makefile`:
```
make
```
## 🚀 How to Execute
The results directly will be saved into the corresponding file in the `result/`.
You can change the order in 'main.cpp'.
- To evaluate Insertion
```
./btree insert
```
(Output saved to result/insert.txt)
- To evaluate Point search
```
./btree search
```
(Output saved to result/search.txt)
- To evaluate Range Query
```
./btree range
```
(Output saved to result/range.txt)
- To evaluate Deletion
```
./btree delete
```
(Output saved to result/delete.txt)
