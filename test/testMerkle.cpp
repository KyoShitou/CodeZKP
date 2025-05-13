#include "../src/merklecpp.h"
#include "../src/ttmath/ttmath.h"
#include "../src/Field.hpp"
#include <openssl/sha.h>
#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

using std::vector;
using std::string;
using std::array;

using std::cout;
using std::endl;


merkle::Hash hash_from_FieldElement(const FieldElement& fe) {
    string rep = (string)fe;
    merkle::Hash h;
    SHA256(reinterpret_cast<const uint8_t*>(rep.data()), rep.size(), h.bytes);
    return h;
}

vector<FieldElement> from_vector(const vector<int>& vec) {
    vector<FieldElement> res;
    for (size_t i = 0; i != vec.size(); i++) {
        res.push_back(FieldElement(static_cast<ttmath::ulint>(vec[i])));
    }
    return res;
}

bool verify_index(const merkle::Hash& Commit, const FieldElement& claim, vector<uint8_t>& path_bytes, size_t index) {
    auto start = std::chrono::high_resolution_clock::now();
    auto path = merkle::Path(path_bytes);
    auto leaf = path.leaf();
    if (leaf != hash_from_FieldElement(claim)) {
        return false;
    }
    if (path.verify(Commit)) {
    } else {        return false;
    }
    auto leaf_index = path.leaf_index();
    if (leaf_index != index) {
        return false;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    cout << "Verification time: " << duration.count() << " ms" << endl;
    return true;
}

int main() {

    vector<int> codeword;
    for (size_t i = 0; i != 1E5; i++) {
        codeword.push_back(i);
    }

    cout << "Codeword size: " << codeword.size() << endl;

    vector<FieldElement> testing = from_vector(codeword);
    merkle::Tree tree;
    for (const auto &fe : testing) {
        tree.insert(hash_from_FieldElement(fe));
    }

    merkle::Hash root = tree.root();

    for (size_t i = 0; i != testing.size(); i++) {
        vector<uint8_t> path_bytes;
        auto path = tree.path(i);
        path->serialise(path_bytes);
        if (!verify_index(root, testing[i], path_bytes, i)) {
            cout << "Test failed for index " << i << endl;
            return 1;
        }
    }
    cout << "All tests passed!" << endl;
}