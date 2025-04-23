#include "gtest/gtest.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept> // For std::invalid_argument in division by zero test
#include <cassert>

// --- Redefine necessary components from Field.hpp for testing ---
// NOTE: This duplicates parts of your code. A better long-term solution
// might involve templating FieldElement or using dependency injection for P.

// Using a simpler BigInt for testing - std::int64_t should suffice for P_TEST=97
// If you were testing with a larger P_TEST, you might need ttmath or GMP here.
#define BigInt int64_t
const BigInt P_TEST = 97; // Our smaller prime for testing

// Redefine xgcd for the testing BigInt type
void test_xgcd(BigInt x, BigInt y, BigInt &a, BigInt &b, BigInt &g) {
    BigInt old_r = x;
    BigInt r = y;
    BigInt old_s = 1;
    BigInt s = 0;
    BigInt old_t = 0;
    BigInt t = 1;

    while (r != 0) {
        if (r == 0) throw std::invalid_argument("Division by zero in xgcd"); // Basic check
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
    }

    // Adjust 'a' to be positive modulo y (which is P_TEST here)
    a = old_s % P_TEST;
    if (a < 0) {
        a += P_TEST;
    }
    b = old_t;
    g = old_r; // Should be gcd(x, y)
}


// Redefine FieldElement using P_TEST
class TestFieldElement {
public:
    BigInt value;

    // Use P_TEST for the modulus
    TestFieldElement(BigInt v = 0) {
        value = v % P_TEST;
        if (value < 0) { // Ensure value is always non-negative
            value += P_TEST;
        }
    }

    TestFieldElement operator+(const TestFieldElement& other) const {
        BigInt res = value + other.value;
        // No need for if (res >= P_TEST) res -= P_TEST; due to constructor %
        return TestFieldElement(res);
    }

    TestFieldElement operator-(const TestFieldElement& other) const {
        // Ensure intermediate result is non-negative before modulo
        BigInt res = value - other.value;
        return TestFieldElement(res); // Constructor handles modulo and sign
    }

    TestFieldElement operator*(const TestFieldElement& other) const {
        return TestFieldElement(value * other.value);
    }

    TestFieldElement operator/(const TestFieldElement& other) const {
        // Use ASSERT_NE in tests rather than assert here for better test reporting
        if (other.value == 0) {
             throw std::invalid_argument("Division by zero in FieldElement");
        }
        BigInt a, b, g;
        test_xgcd(other.value, P_TEST, a, b, g);
        // Ensure inverse 'a' is correct (g should be 1 if other.value is not 0 mod P_TEST)
         if (g != 1) {
             // This case should ideally not happen if other.value != 0 and P_TEST is prime
             throw std::runtime_error("Inverse does not exist - gcd is not 1");
         }
        // 'a' from xgcd is the modular inverse
        return TestFieldElement(value * a);
    }

     TestFieldElement operator-() const {
        // Handle the case where value is 0 correctly
         if (value == 0) return TestFieldElement(0);
         return TestFieldElement(P_TEST - value);
     }


    TestFieldElement inv() const {
       if (value == 0) {
             throw std::invalid_argument("Inverse of zero requested");
        }
        BigInt a, b, g;
        test_xgcd(value, P_TEST, a, b, g);
         if (g != 1) {
             throw std::runtime_error("Inverse does not exist - gcd is not 1");
         }
        return TestFieldElement(a);
    }

    // Simplified pow for int exponent for testing
    TestFieldElement operator^(int exp) const {
        if (exp == 0) return TestFieldElement(1);
        if (exp < 0) {
            // Handle negative exponent using inverse
             if (value == 0) {
                 throw std::invalid_argument("Cannot raise zero to a negative power");
             }
            return this->inv() ^ (-exp);
        }
        if (exp == 1) return TestFieldElement(value);

        BigInt base = value;
        BigInt result = 1;
        BigInt current_exp = exp; // Use a copy to modify

        while (current_exp > 0) {
            if (current_exp % 2 == 1) result = (result * base) % P_TEST;
            base = (base * base) % P_TEST;
            current_exp >>= 1;
        }
        return TestFieldElement(result);
    }

    bool operator==(const TestFieldElement& other) const {
        // Ensure both values are reduced modulo P_TEST before comparison
        return (value % P_TEST) == (other.value % P_TEST);
    }

    bool operator!=(const TestFieldElement& other) const {
        return !(*this == other);
    }

    // Helper for debugging tests
    friend std::ostream& operator<<(std::ostream& os, const TestFieldElement& x) {
        os << x.value;
        return os;
    }
};
// --- End of Redefined components ---


// --- Test Fixture ---
class FieldElementTest : public ::testing::Test {
protected:
    // Define some elements for convenience in tests
    TestFieldElement zero{0};
    TestFieldElement one{1};
    TestFieldElement two{2};
    TestFieldElement five{5};
    TestFieldElement ten{10};
    TestFieldElement large{100}; // Will be reduced to 100 % 97 = 3
    TestFieldElement neg_one_equiv{P_TEST - 1}; // Equivalent to -1 mod P_TEST (96)
};

// --- Test Cases ---

TEST_F(FieldElementTest, Constructor) {
    EXPECT_EQ(zero.value, 0);
    EXPECT_EQ(one.value, 1);
    EXPECT_EQ(TestFieldElement(P_TEST).value, 0); // Test reduction P_TEST -> 0
    EXPECT_EQ(TestFieldElement(P_TEST + 5).value, 5); // Test reduction P_TEST + 5 -> 5
    EXPECT_EQ(large.value, 3); // Test reduction 100 -> 3
    EXPECT_EQ(TestFieldElement(-1).value, P_TEST - 1); // Test negative input -1 -> 96
    EXPECT_EQ(TestFieldElement(-98).value, 96); // Test negative input -98 = -1 * 97 - 1 -> 96
}

TEST_F(FieldElementTest, Addition) {
    EXPECT_EQ((two + five).value, 7);
    EXPECT_EQ((ten + neg_one_equiv).value, 9); // 10 + 96 = 106 -> 9 mod 97
    EXPECT_EQ((five + zero).value, 5);
    EXPECT_EQ((neg_one_equiv + one).value, 0); // 96 + 1 = 97 -> 0 mod 97
}

TEST_F(FieldElementTest, Subtraction) {
    EXPECT_EQ((five - two).value, 3);
    EXPECT_EQ((two - five).value, (P_TEST - 3)); // 2 - 5 = -3 -> 94 mod 97
    EXPECT_EQ((ten - zero).value, 10);
    EXPECT_EQ((zero - one).value, neg_one_equiv.value); // 0 - 1 = -1 -> 96 mod 97
    EXPECT_EQ((five - five).value, 0);
}

TEST_F(FieldElementTest, Multiplication) {
    EXPECT_EQ((two * five).value, 10);
    EXPECT_EQ((ten * ten).value, 3); // 10 * 10 = 100 -> 3 mod 97
    EXPECT_EQ((five * zero).value, 0);
    EXPECT_EQ((five * one).value, 5);
    EXPECT_EQ((neg_one_equiv * two).value, (P_TEST - 2)); // 96 * 2 = 192 = 1*97 + 95 -> 95 mod 97 (-1 * 2 = -2)
}

TEST_F(FieldElementTest, Negation) {
    EXPECT_EQ((-one).value, neg_one_equiv.value);
    EXPECT_EQ((-neg_one_equiv).value, one.value); // -(-1) = 1
    EXPECT_EQ((-zero).value, 0);
    EXPECT_EQ((-five).value, (P_TEST - 5)); // -5 -> 92 mod 97
    EXPECT_EQ(five + (-five), zero);
}

TEST_F(FieldElementTest, Inverse) {
    // 10 * 10 = 100 = 3 mod 97. So inv(10) should be 10 * inv(3).
    // Let's find inv(10): 10 * x = 1 mod 97. 10 * 10 = 100 = 3. 10 * (-29) = -290 = -3*97 + 1 => -29 = 68 mod 97.
    // Check: 10 * 68 = 680. 680 / 97 = 7.01... 7 * 97 = 679. 680 - 679 = 1. Correct.
    EXPECT_EQ(ten.inv().value, 68);
    EXPECT_EQ((ten * ten.inv()), one);

    // inv(2): 2 * x = 1 mod 97. 2 * 49 = 98 = 1 mod 97. inv(2) = 49.
    EXPECT_EQ(two.inv().value, 49);
    EXPECT_EQ((two * two.inv()), one);

    EXPECT_EQ(one.inv().value, 1);
    EXPECT_EQ(neg_one_equiv.inv().value, neg_one_equiv.value); // inv(-1) = -1

    // Test inverse of zero throws
    EXPECT_THROW(zero.inv(), std::invalid_argument);
}

TEST_F(FieldElementTest, Division) {
    // ten / two = 10 / 2 = 5
    EXPECT_EQ((ten / two), five);

    // two / ten = 2 * inv(10) = 2 * 68 = 136 = 1*97 + 39 -> 39 mod 97
    EXPECT_EQ((two / ten).value, 39);

    // Check consistency: (a/b) * b = a
    EXPECT_EQ(((two / ten) * ten), two);
    EXPECT_EQ(((ten / two) * two), ten);

    EXPECT_EQ((ten / one), ten);
    EXPECT_EQ((ten / neg_one_equiv), -ten); // 10 / (-1) = -10 -> 87 mod 97
    EXPECT_EQ((ten / neg_one_equiv).value, (P_TEST - 10));


    // Test division by zero throws
    EXPECT_THROW(one / zero, std::invalid_argument);
}


TEST_F(FieldElementTest, Exponentiation) {
    // 2^0 = 1
    EXPECT_EQ((two ^ 0), one);
    // 2^1 = 2
    EXPECT_EQ((two ^ 1), two);
    // 2^2 = 4
    EXPECT_EQ((two ^ 2).value, 4);
    // 2^3 = 8
    EXPECT_EQ((two ^ 3).value, 8);
     // 10^2 = 100 = 3 mod 97
    EXPECT_EQ((ten ^ 2).value, 3);
     // 10^3 = 1000. 1000 / 97 = 10.3... 10 * 97 = 970. 1000 - 970 = 30.
    EXPECT_EQ((ten ^ 3).value, 30);
    // Test Fermat's Little Theorem: a^(P-1) = 1 mod P (for P prime, a != 0)
    // 2^(96) = 1 mod 97
    EXPECT_EQ((two ^ (P_TEST - 1)), one);
    EXPECT_EQ((ten ^ (P_TEST - 1)), one);

    // Test negative exponent
    // 2^-1 = inv(2) = 49 mod 97
    EXPECT_EQ((two ^ -1), two.inv());
    // 10^-2 = inv(10^2) = inv(3) mod 97
    // 3 * x = 1 mod 97. 3 * 33 = 99 = 2. 3 * (-32) = -96 = 1 mod 97. -32 = 65 mod 97. inv(3) = 65.
    EXPECT_EQ((ten ^ -2).value, 65);
    EXPECT_EQ(((ten^2).inv()).value, 65);

    // Test zero exponentiation edge cases
    EXPECT_THROW(zero ^ -1, std::invalid_argument);
    EXPECT_EQ((zero ^ 0), one); // Mathematically debatable, but often defined as 1 in this context
    EXPECT_EQ((zero ^ 1), zero);
    EXPECT_EQ((zero ^ 2), zero);
}


TEST_F(FieldElementTest, Comparison) {
    EXPECT_TRUE(five == TestFieldElement(5));
    EXPECT_TRUE(five == TestFieldElement(5 + P_TEST));
    EXPECT_TRUE(zero == TestFieldElement(P_TEST));
    EXPECT_FALSE(one == zero);
    EXPECT_TRUE(one != zero);
    EXPECT_TRUE(large == TestFieldElement(3)); // 100 mod 97 = 3
    EXPECT_FALSE(large == five);
}

// Example test for xgcd (optional but good)
TEST(XgcdTest, BasicCases) {
    BigInt a, b, g;
    // gcd(10, 97) = 1. 10*a + 97*b = 1. We found a=68 above.
    test_xgcd(10, P_TEST, a, b, g);
    EXPECT_EQ(g, 1);
    EXPECT_EQ(a, 68); // a should be the inverse mod P_TEST

    // gcd(0, 97) = 97
    test_xgcd(0, P_TEST, a, b, g);
    EXPECT_EQ(g, P_TEST);

     // gcd(97, 10) = 1.
    test_xgcd(P_TEST, 10, a, b, g);
    EXPECT_EQ(g, 1);
    // Here 'a' will be inverse of 97 mod 10 (which is 0), 'b' will be inverse of 10 mod 97 (which is 68).
    // The roles of a and b depend on the order of inputs to xgcd.
    // Let's re-run with expected output based on implementation:
    // 97 = 9*10 + 7
    // 10 = 1*7 + 3
    // 7 = 2*3 + 1
    // 1 = 7 - 2*3
    // 1 = 7 - 2*(10 - 1*7) = 3*7 - 2*10
    // 1 = 3*(97 - 9*10) - 2*10 = 3*97 - 27*10 - 2*10 = 3*97 - 29*10
    // So, a=3, b=-29 for x=97, y=10
    EXPECT_EQ(a, 3);
    EXPECT_EQ(b, -29);
}


// --- Main function to run tests ---
// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }