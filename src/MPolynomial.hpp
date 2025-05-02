#ifndef MPOLYNOMIAL_HPP
#define MPOLYNOMIAL_HPP

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <bitset>
#include "Polynomial.hpp"
#include "utils.hpp"

using std::bitset;
using std::vector;
using std::string;
using std::map;
using std::max_element;

class MPolynomial {
    public:
    map<vector<BigInt>, FieldElement> dict;
    /*
    f(x, y, z) = 17 + 2xy + 42z - 19x^6 * y^3 * z^12 would be represented as:
    {
        (0, 0, 0) -> 17,
        (1, 1, 0) -> 2,
        (0, 0, 1) -> 42,
        (6, 3, 12) -> -19
    }
    */

    MPolynomial() {}
    MPolynomial(const map<vector<BigInt>, FieldElement>& dict) : dict(dict) {}
    MPolynomial(const FieldElement& c) {
        vector<BigInt> exp(1, (BigInt)0);
        dict[exp] = c;
    }
    MPolynomial(const Polynomial& poly, const size_t& index) {
        if (poly.degree() == -1) return;
        map<vector<BigInt>, FieldElement> res_dict;
        for (size_t i = 0; i != poly.coeffs.size(); i++) {
            vector<BigInt> exp(index + 1, (BigInt)0);
            exp[index] = static_cast<ttmath::ulint>(i);
            res_dict[exp] = poly.coeffs[i];
        }
        this->dict = res_dict;
    }

    MPolynomial operator+(const MPolynomial &other) const {
        map<vector<BigInt>, FieldElement> res_dict;
        vector<size_t> variable_count;
        for (const auto& pair : dict) {
            variable_count.push_back(pair.first.size());
        }
        for (const auto& pair : other.dict) {
            variable_count.push_back(pair.first.size());
        }
        size_t num_vars = *max_element(variable_count.begin(), variable_count.end());
        
        for (const auto& pair : dict) {
            vector<BigInt> padded_key = pair.first;
            padded_key = pad_vector(padded_key, num_vars - pair.first.size(), (BigInt)0);
            res_dict[padded_key] = pair.second;
        }

        for (const auto& pair : other.dict) {
            vector<BigInt> padded_key = pair.first;
            padded_key = pad_vector(padded_key, num_vars - pair.first.size(), (BigInt)0);
            if (res_dict.find(padded_key) != res_dict.end()) {
                res_dict[padded_key] = res_dict[padded_key] + pair.second;
            } else {
                res_dict[padded_key] = pair.second;
            }
        }
        return MPolynomial(res_dict);
    }

    MPolynomial operator*(const MPolynomial &other) const {
        map<vector<BigInt>, FieldElement> res_dict;
        vector<size_t> variable_count;
        for (const auto& pair : dict) {
            variable_count.push_back(pair.first.size());
        }
        for (const auto& pair : other.dict) {
            variable_count.push_back(pair.first.size());
        }
        size_t num_vars = *max_element(variable_count.begin(), variable_count.end());

        for (const auto& pair : dict) {
            for (const auto& pair2 : other.dict) {
                vector<BigInt> exponent(num_vars, (BigInt)0);
                for (size_t i = 0; i != pair.first.size(); i++) {
                    exponent[i] = exponent[i] + pair.first[i];
                }
                for (size_t i = 0; i != pair2.first.size(); i++) {
                    exponent[i] = exponent[i] + pair2.first[i];
                }
                if (res_dict.find(exponent) != res_dict.end()) {
                    res_dict[exponent] = res_dict[exponent] + (pair.second * pair2.second);
                } else {
                    res_dict[exponent] = pair.second * pair2.second;
                }
            }
        }
        return MPolynomial(res_dict);
    }

    MPolynomial operator-() const {
        map<vector<BigInt>, FieldElement> neg_dict;
        for (const auto& pair : dict) {
            neg_dict[pair.first] = -pair.second;
        }
        return MPolynomial(neg_dict);
    }

    MPolynomial operator-(const MPolynomial &other) const {
        return this->operator+(-other);
    }

    MPolynomial operator^(const BigInt &exponent) const {
        if (this->dict.empty()) return MPolynomial();
        size_t num_vars = this->dict.begin()->first.size();
        vector<BigInt> exp(num_vars, (BigInt)0);
        map<vector<BigInt>, FieldElement> res_dict;
        res_dict[exp] = FieldElement(1);
        MPolynomial res = MPolynomial(res_dict);
        MPolynomial base = *this;
        BigInt exp_copy = exponent;
        while (exp_copy != 0 && !base.is_zero()) {
            if (exp_copy % 2 == 1) res = res * base;
            base = base * base;
            exp_copy = exp_copy >> 1;
        }
        return res;
    }

    FieldElement operator[](const vector<FieldElement>& x) const {
        FieldElement result = FieldElement(0);
        for (const auto& pair : dict) {
            FieldElement prod = pair.second;
            for (size_t i = 0; i != pair.first.size(); i++) {
                prod = prod * (x[i]^(pair.first[i]));
            }
            result = result + prod;
        }
        return result;
    }

    Polynomial evaluate_symbolic(const vector<Polynomial>& x) const {
        Polynomial result;
        for (const auto& pair : dict) {
            Polynomial prod = Polynomial(vector<FieldElement>{pair.second});
            for (size_t i = 0; i != pair.first.size(); i++) {
                prod = prod * (x[i]^(pair.first[i]));
            }
            result = result + prod;
        }
        return result;
    }
    bool is_zero() const {
        if (this->dict.empty()) return true;
        for (const auto& pair: dict) {
            if (pair.second != FieldElement(0)) {
                return false;
            }
        }
        return true;
    }
};

vector<MPolynomial> identity(const size_t &num_vars) {
    vector<MPolynomial> result;
    for (size_t i = 0; i != num_vars; i++) {
        vector<BigInt> exp(num_vars, (BigInt)0);
        exp[i] = 1;
        map<vector<BigInt>, FieldElement> dict;
        dict[exp] = FieldElement(1);
        MPolynomial poly(dict);
        result.push_back(poly);
    }
    return result;
}

#endif