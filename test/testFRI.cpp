#include "../src/FRI.hpp"
#include "../src/Field.hpp"
#include "../src/Polynomial.hpp"
#include <iostream>

using namespace std;

const void commit(void* codeword) {
    vector<FieldElement>* codeword_vec = static_cast<vector<FieldElement>*>(codeword);
    cout << "Prover Commit: ";
    for (size_t i = 0; i != codeword_vec->size(); i++) {
        cout << (*codeword_vec)[i] << " ";
    }
    cout << std::endl;
}


const void get_challenge(void* challenge) {
    FieldElement* challenge_ptr = static_cast<FieldElement*>(challenge);
    size_t alpha;
    cout << "Verifier Challenge: ";
    cin >> alpha;
    *challenge_ptr = FieldElement(static_cast<ttmath::ulint>(alpha));
}

const void get_colinearity_challenge(void* index) {
    size_t* index_ptr = static_cast<size_t*>(index);
    cout << "Verifier Colinearity Challenge: ";
    cin >> *index_ptr;
}

const void open_merkle(void* merkle) {
    // Placeholder for Merkle tree opening
    vector<FieldElement>* open_points = (vector<FieldElement>*)merkle;
    vector<FieldElement> open_points_x = {FieldElement(1), FieldElement(2), FieldElement(3)};
    vector<FieldElement> open_points_y = {FieldElement(4), FieldElement(5), FieldElement(6)};

    cout << "Doing colinearity test: " << (test_colinearity(open_points_x, open_points_y)? "Accept" : "Reject") << std::endl;
}

int main() {
    vector<int> codeword = {4, 1, 13, 10, 5, 6, 4, 10, 9, 10, 14, 8, 0, 1, 4, 7};
    vector<FieldElement> codeword_fe;
    for (size_t i = 0; i != codeword.size(); i++) {
        codeword_fe.push_back(FieldElement(static_cast<ttmath::ulint>(codeword[i])));
    }

    FieldElement omega(3);
    FieldElement offset(1);

    FRI::prove(codeword_fe, omega, offset, commit, get_challenge, get_colinearity_challenge, open_merkle);
}
