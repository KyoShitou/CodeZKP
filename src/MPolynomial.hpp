#ifndef MPOLYNOMIAL_HPP
#define MPOLYNOMIAL_HPP

#include <vector>
#include <string>
#include <map>
#include "Polynomial.hpp"

using std::vector;
using std::string;
using std::map;

class MPolynomial {
    public:
    map<vector<BigInt>, FieldElement> dict; // e.g x_0 + 2X_1 + 3x_1^2x_2 -> {[1]: 1, [0, 2]: 2, [0, 2, 1]: 3}

    MPolynomial() {}
    MPolynomial(const map<vector<BigInt>, FieldElement>& dict) : dict(dict) {}

    MPolynomial operator+(const MPolynomial &other) const {
        MPolynomial result;
        for (const auto& [key, value] : dict) {
            result.dict[key] = value;
        }
        for (const auto& [key, value] : other.dict) {
            result.dict[key] = result.dict[key] + value;
        }
        return result;
    }
};

#endif