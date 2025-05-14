#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "src/witness.hpp"
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cin;

int main() {
    // Open a pipe to run the Python script
    string filename;
    cout << "filename for the code: ";
    cin >> filename;

    string command = "python3 interpreter.py " + filename;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to run Python script.\n";
        return 1;
    }

    // Read script output
    char buffer[1024];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    cout << "Result from Python script:\n" << result << endl;
    std::vector<std::vector<int>> data;
    std::istringstream full_stream(result);
    std::string line;
    while (std::getline(full_stream, line)) {
        std::istringstream iss(line);
        std::vector<int> row;
        int value;
        while (iss >> value) {
            row.push_back(value);
        }
        if (!row.empty()) {
            data.push_back(row);
        }
    }

    vector<vector<FieldElement> > trace_matrix(data.size(), vector<FieldElement>(data[0].size()));
    for (size_t i = 0; i != data.size(); i++) {
        for (size_t j = 0; j != data[0].size(); j++) {
            trace_matrix[i][j] = FieldElement(data[i][j]);
        }
    }

    WITNESS::witness(trace_matrix);

    cout << WITNESS::transcript << endl;

    return 0;
}
