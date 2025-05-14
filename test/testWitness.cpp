#include "../src/witness.hpp"

int main() {
    vector<vector<int> > int_trace_matrix = {
        {0, 0, 0, 0, 0, 1, 0, 0, 1, 10, 0, 0},
        {10, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0},
        {10, 0, 0, 0, 0, 2, 1, 0, 0, 0, 2, 10},
        {10, 10, 0, 0, 0, 1, 0, 1, 0, 0, 3, 10},
        {20, 10, 0, 0, 0, 0, 0, 0, 0, 0, 4, 10},
        {20, 10, 0, 0, 0, 0, 0, 0, 0, 0, 5, 10}
    };

    vector<vector<FieldElement> > trace_matrix(int_trace_matrix.size(), vector<FieldElement>(int_trace_matrix[0].size()));
    for (size_t i = 0; i != int_trace_matrix.size(); i++) {
        for (size_t j = 0; j != int_trace_matrix[0].size(); j++) {
            trace_matrix[i][j] = FieldElement(int_trace_matrix[i][j]);
        }
    }

    string transcript;
    transcript = WITNESS::witness(trace_matrix);

    std::cout << transcript << std::endl;

}