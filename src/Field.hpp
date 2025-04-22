#include <cstdint>
#include <iostream>

using std::uint64_t;

template <uint64_t P>
class Fp {
    static_assert(P > 1, "Modulus must be > 1");
    static_assert(P < (1ULL << 63), "Modulus too large for safe 64-bit ops");

public:
    uint64_t value;

    Fp(uint64_t v = 0) : value(v % P) {}

    Fp operator+(const Fp& other) const {
        uint64_t res = value + other.value;
        if (res >= P) res -= P;
        return Fp(res);
    }

    Fp operator-(const Fp& other) const {
        return Fp((value + P - other.value) % P);
    }

    Fp operator*(const Fp& other) const {
        return Fp((__uint128_t(value) * other.value) % P);
    }

    Fp pow(uint64_t exp) const {
        uint64_t base = value, result = 1;
        while (exp > 0) {
            if (exp & 1) result = (__uint128_t(result) * base) % P;
            base = (__uint128_t(base) * base) % P;
            exp >>= 1;
        }
        return Fp(result);
    }

    Fp inv() const {
        return pow(P - 2);  // Fermat's little theorem (only valid if P is prime)
    }

    Fp operator/(const Fp& other) const {
        return *this * other.inv();
    }

    bool operator==(const Fp& other) const {
        return value == other.value;
    }

    friend std::ostream& operator<<(std::ostream& os, const Fp& x) {
        os << x.value;
        return os;
    }
};
