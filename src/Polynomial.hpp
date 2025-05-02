#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP

#include <vector>
#include <string>
#include "ttmath/ttmath.h"
#include "Field.hpp"

using std::vector;
using std::string;

class Polynomial{

public:
    vector<FieldElement> coeffs;
    Polynomial() {coeffs = vector<FieldElement>();}
    Polynomial(const vector<FieldElement>& coeffs) : coeffs(coeffs) {}

    int64_t degree() const {
        if (coeffs.empty()) return -1;

        bool flag = true;
        for (auto& coeff : coeffs) {
            if (coeff != FieldElement(0)) {
                flag = false;
                break;
            }
        }
        if (flag) return -1;
        FieldElement zero = FieldElement(0);
        int64_t max_index = 0;
        if (coeffs[0] != zero) max_index = 0;
        for (int64_t i = 0; i < coeffs.size(); i++) {
            if (coeffs[i] != zero) {
                max_index = i;
            }
        }
        return max_index;
    }

    Polynomial operator-() const {
        vector<FieldElement> neg_coeffs;
        for (const auto& coeff : coeffs) {
            neg_coeffs.push_back(-coeff);
        }
        return Polynomial(neg_coeffs);
    }

    Polynomial operator+(const Polynomial &other) const {
        if (this->degree() == -1) return other;
        else if (other.degree() == -1) return *this;
        
        vector<FieldElement> new_coeffs(std::max(this->coeffs.size(), other.coeffs.size()), FieldElement(0));
        for (size_t i = 0; i != this->coeffs.size(); i++) {
            new_coeffs[i] = this->coeffs[i];
        }
        for (size_t i = 0; i != other.coeffs.size(); i++) {
            new_coeffs[i] = new_coeffs[i] + other.coeffs[i];
        }

        return Polynomial(new_coeffs);
    }   

    Polynomial operator-(const Polynomial &other) const {
        return this->operator+(-other);
    }

    Polynomial operator*(const Polynomial &other) const {
        if (this->degree() == -1 || other.degree() == -1) return Polynomial();
        vector<FieldElement> new_coeffs(this->coeffs.size() + other.coeffs.size() - 1, FieldElement(0));
        for (size_t i = 0; i != this->coeffs.size(); i++) {
            if (this->coeffs[i] == FieldElement(0)) continue;
            for (size_t j = 0; j != other.coeffs.size(); j++) {
                new_coeffs[i + j] = new_coeffs[i + j] + (this->coeffs[i] * other.coeffs[j]);
            }
        }
        return Polynomial(new_coeffs);
    }

    bool operator==(const Polynomial &other) const {
        if (this->degree() != other.degree()) return false;
        if (this->degree() == -1) return true;

        for (size_t i = 0; i != this->degree() + 1; i++) {
            if (this->coeffs[i] != other.coeffs[i]) return false;
        }
        return true;
    }

    bool operator!=(const Polynomial &other) const {
        return !(this->operator==(other));
    }

    FieldElement leading_coefficient() const {
        return this->coeffs[static_cast<size_t>(this->degree())];
    }

    void divided_by(const Polynomial& other, Polynomial& quotient, Polynomial &remainder) const {
        if (other.degree() == -1) {
            throw std::invalid_argument("Divisor cannot be zero polynomial");
        }
        if (this->degree() < other.degree()) {
            quotient = Polynomial();
            remainder = *this;
            return;
        }

        Polynomial tmpremainder = *this;
        vector<FieldElement> quotient_coeffs(this->degree() - other.degree() + 1, FieldElement(0));
        for (size_t i = 0; i != this->degree() - other.degree() + 1; i++) {
            if (tmpremainder.degree() < other.degree()) break;
            FieldElement coeff = tmpremainder.leading_coefficient() / other.leading_coefficient();
            size_t degree_shift = tmpremainder.degree() - other.degree();
            vector<FieldElement> subtractee_vec(degree_shift, FieldElement(0));
            subtractee_vec.push_back(coeff);
            Polynomial subtractee = Polynomial(subtractee_vec) * other;
            quotient_coeffs[degree_shift] = coeff;
            tmpremainder = tmpremainder - subtractee;
        }
        quotient = Polynomial(quotient_coeffs);
        remainder = tmpremainder;
        if (remainder.degree() == -1) {
            remainder = Polynomial(vector<FieldElement>{FieldElement(0)});
        }
        return;
    }

    Polynomial operator/(const Polynomial& other) const {
        Polynomial quotient, remainder;
        this->divided_by(other, quotient, remainder);
        if (remainder.degree() != -1) {
            throw std::invalid_argument("Remainder is not zero");
        }
        return quotient;
    }

    Polynomial operator/(const FieldElement& other) const {
        if (other == FieldElement(0)) {
            throw std::invalid_argument("Divisor cannot be zero");
        }
        vector<FieldElement> new_coeffs;
        for (const auto& coeff : this->coeffs) {
            new_coeffs.push_back(coeff / other);
        }
        return Polynomial(new_coeffs);
    }

    Polynomial operator%(const Polynomial& other) const {
        Polynomial quotient, remainder;
        this->divided_by(other, quotient, remainder);
        return remainder;
    }

    Polynomial operator^(const BigInt& other) const {
        if (this->degree() == -1 && other == 0) throw std::invalid_argument("Zero polynomial cannot be raised to power 0");
        if (this->degree() == -1) return Polynomial();

        if (other == 0) return Polynomial(vector<FieldElement>(1, FieldElement(1)));

        Polynomial result = Polynomial(vector<FieldElement>(1, FieldElement(1)));
        Polynomial base = *this;
        BigInt exp = other;
        while (exp != 0 && base != Polynomial(vector<FieldElement>(1, FieldElement(1)))) {
            if (exp % 2 == 1) result = result * base;
            base = base * base;
            exp = exp >> 1;
        }
        return result;
    }

    FieldElement operator[](const FieldElement& x) const { // evaluate polynomial at a given point
        FieldElement result = FieldElement(0);
        FieldElement x_i = FieldElement(1);
        for (auto & coeff : coeffs) {
            result = result + (coeff * x_i);
            x_i = x_i * x;
        }
        return result;
    }

    vector<FieldElement> evaluate_domain(const vector<FieldElement>& domain) const {
        vector<FieldElement> result;
        for (auto & x : domain) {
            result.push_back(this->operator[](x));
        }
        return result;
    }

    Polynomial scale(const FieldElement& scalar) const {
        vector<FieldElement> new_coeffs;
        for (const auto& coeff : this->coeffs) {
            new_coeffs.push_back(coeff * scalar);
        }
        return Polynomial(new_coeffs);
    }

    operator string() const {
        string result = "";
        for (size_t i = 0; i != coeffs.size(); i++) {
            if (i == 0) {
                result += (string)coeffs[i];
            } else {
                result += " + " + (string)coeffs[i] + "x^" + std::to_string(i);
            }
        }
        return result;
    }

};

Polynomial interpolate_domain(const vector<FieldElement> &domain, const vector<FieldElement> &values) {
    if (domain.size() != values.size()) {
        throw std::invalid_argument("Domain and values must have the same size");
    }
    Polynomial result;
    Polynomial x(vector<FieldElement>{FieldElement(0), FieldElement(1)});
    
    for (size_t i = 0; i != domain.size(); i++) {
        Polynomial prod = Polynomial(vector<FieldElement>{FieldElement(values[i])});
        for (size_t j = 0; j != domain.size(); j++) {
            if (j == i) continue;
            prod = prod * (x - Polynomial(vector<FieldElement>{(domain[j])})) * Polynomial(vector<FieldElement>{(domain[i] - domain[j]).inv()});
        }
        result = result + prod;
    }
    return result;
}

Polynomial zerofier_domain(vector<FieldElement> domain) {
    Polynomial x(vector<FieldElement>{FieldElement(0), FieldElement(1)});
    Polynomial result(vector<FieldElement>{FieldElement(1)});

    for (auto & x_i : domain) {
        result = result * (x - Polynomial(vector<FieldElement>{x_i}));
    }
    return result;
}

bool test_colinearity(const vector<FieldElement>& domain, const vector<FieldElement>& values) {
    if (domain.size() != values.size()) {
        throw std::invalid_argument("Domain and values must have the same size");
    }
    if (domain.size() < 2) return true;
    
    Polynomial p = interpolate_domain(domain, values);
    return p.degree() <= 1;
}

#endif