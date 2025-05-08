#ifndef FIELD_HPP
#define FIELD_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include "ttmath/ttmath.h"

using std::vector;
using std::string;
using std::uint64_t;
using ttmath::Int;

#define BigInt Int<32>
#define ull unsigned long long

const BigInt P = "270497897142230380135924736767050121217"; // 1 + 407 * ( 1 << 119 )
// const BigInt P = "17"; // testing prime

void xgcd(BigInt x, BigInt y, BigInt &a, BigInt &b, BigInt &g) { // Extended Euclidean algorithm
    BigInt old_r = x;
    BigInt r = y;
    BigInt old_s = 1;
    BigInt s = 0;
    BigInt old_t = 0;
    BigInt t = 1;

    // std::cout << "XGCD: " << x << ", " << y << std::endl;

    while (r != 0) {
        BigInt quotient = old_r / r;
        BigInt temp_r = r;
        r = old_r - quotient * r;
        old_r = temp_r;

        BigInt temp_s = s;
        s = old_s - quotient * s;
        old_s = temp_s;

        BigInt temp_t = t;
        t = old_t - quotient * t;
        old_t = temp_t;
        // std::cout << old_r << ", " << old_s << ", " << old_t << std::endl;
        // std::cout << r << ", " << s << ", " << t << std::endl;
    }

    a = old_s;
    b = old_t;
    g = old_r;
    return;
}

class FieldElement {
    public:
        BigInt value;
    
        FieldElement(BigInt v = 0) : value((v + P) % P) {}

        FieldElement operator+(const FieldElement& other) const {
            BigInt res = value + other.value;
            if (res >= P) res -= P;
            return FieldElement(res);
        }

        FieldElement operator-(const FieldElement& other) const {
            return FieldElement((value + P - other.value) % P);
        }

        FieldElement operator*(const FieldElement& other) const {
            return FieldElement((value * other.value) % P);
        }

        FieldElement operator/(const FieldElement& other) const {
            assert(other.value != 0);
            BigInt a, b, g;
            xgcd(other.value, P, a, b, g);
            return FieldElement(value * a % P);
        }

        FieldElement operator-() const {
            return FieldElement((P - value) % P);
        }

        FieldElement inv() const {
            BigInt a, b, g;
            xgcd(value, P, a, b, g);
            return FieldElement(a);
        }

        FieldElement operator^(const FieldElement& other) const {
            return this->operator^(other.value);
        }

        FieldElement operator^(BigInt exp) const {
            BigInt zero = 0;
            if (exp == 0) return FieldElement(1);
            if (exp == 1) return FieldElement(value);
            FieldElement result(1);
            FieldElement base(value);

            while (exp != zero && base != FieldElement(1)) {
                if (exp % 2 == 1) result = result * base;
                base = base * base;
                exp = exp >> 1;
            }
            return result;
        }

        FieldElement operator^(unsigned long long exp) const {
            BigInt _exp = static_cast<ttmath::ulint>(exp);
            return this->operator^(_exp);
        }

        bool operator==(const FieldElement& other) const {
            return value == other.value;
        }

        bool operator!=(const FieldElement& other) const {
            return value != other.value;
        }

        operator string() const {
            return value.ToString(10);
        }

        friend std::ostream& operator<<(std::ostream& os, const FieldElement& x) {
            os << x.value;
            return os;
        }
};

FieldElement generator() {
    BigInt g = "85408008396924667383611388730472331217";
    FieldElement gen(g);
    return gen;
}

FieldElement primitive_nth_root(BigInt n) {
    FieldElement g = generator();
    BigInt order = 1;
    order <<= 119;

    while (order != n) {
        g = g^2;
        order = order / 2;
    }
    return g;
}

FieldElement sample(vector<uint8_t> &random_bytes) {
    BigInt acc = 0;
    for (uint8_t byte : random_bytes) {
        acc = (acc << 8) ^ byte;
    }
    return FieldElement(acc);
}

#endif