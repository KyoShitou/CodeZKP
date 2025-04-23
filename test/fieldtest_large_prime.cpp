#include "../src/Field.hpp" // Include your original header
#include "gtest/gtest.h"
#include <string>
#include <stdexcept> // For std::runtime_error

// --- Test Fixture ---
// Uses the actual FieldElement class from Field.hpp
class FieldElementLargePrimeTest : public ::testing::Test {
protected:
    // Define some useful FieldElement constants using BigInt/string initialization
    // Note: Using string literals for large numbers is generally easier with BigInt libraries.

    FieldElement zero{BigInt(0)};
    FieldElement one{BigInt(1)};
    FieldElement two{BigInt(2)};
    FieldElement three{BigInt(3)};

    // Value P-1 (equivalent to -1 mod P)
    FieldElement neg_one{P - 1};

    // Some arbitrary large-ish values less than P
    FieldElement val1{"123456789012345678901234567890123456789"};
    FieldElement val2{"987654321098765432109876543210987654321"};

    // The generator g from Field.hpp
    FieldElement gen = generator();

    // P itself (should construct to zero)
    FieldElement p_val{P};

    // P+1 (should construct to one)
    FieldElement p_plus_one{P + 1};

};

// --- Test Cases ---

TEST_F(FieldElementLargePrimeTest, ConstructorAndEquality) {
    EXPECT_EQ(zero.value, BigInt(0));
    EXPECT_EQ(one.value, BigInt(1));
    EXPECT_EQ(p_val.value, BigInt(0)); // P mod P should be 0
    EXPECT_EQ(p_plus_one.value, BigInt(1)); // (P+1) mod P should be 1

    EXPECT_EQ(zero, p_val);
    EXPECT_EQ(one, p_plus_one);
    EXPECT_NE(one, zero);
    EXPECT_NE(val1, val2);

    // Test copy constructor and assignment (implicitly tested by fixture setup and EXPECT_EQ)
    FieldElement val1_copy = val1;
    EXPECT_EQ(val1_copy, val1);
    FieldElement val2_assigned;
    val2_assigned = val2;
    EXPECT_EQ(val2_assigned, val2);
}

TEST_F(FieldElementLargePrimeTest, Addition) {
    // Test identity
    EXPECT_EQ(val1 + zero, val1);
    EXPECT_EQ(zero + val1, val1);

    // Test commutativity
    EXPECT_EQ(val1 + val2, val2 + val1);

    // Test associativity (a + b) + c == a + (b + c)
    EXPECT_EQ((val1 + val2) + three, val1 + (val2 + three));

    // Test wrapping around P
    EXPECT_EQ(neg_one + one, zero); // (P-1) + 1 = P = 0 mod P
    EXPECT_EQ(neg_one + two, one);  // (P-1) + 2 = P+1 = 1 mod P
}

TEST_F(FieldElementLargePrimeTest, Subtraction) {
    // Test identity
    EXPECT_EQ(val1 - zero, val1);

    // Test inverse relationship with addition
    EXPECT_EQ(val1 - val2, val1 + (-val2));
    EXPECT_EQ(val1 - val1, zero);
    EXPECT_EQ(zero - val1, -val1);

    // Test wrapping
    EXPECT_EQ(one - two, neg_one); // 1 - 2 = -1 = P-1 mod P
    EXPECT_EQ(zero - one, neg_one);
}

TEST_F(FieldElementLargePrimeTest, Negation) {
    EXPECT_EQ(-zero, zero);
    EXPECT_EQ(-one, neg_one);
    EXPECT_EQ(-neg_one, one); // -(-1) = 1
    EXPECT_EQ(val1 + (-val1), zero);
    EXPECT_EQ(-(-val1), val1);
}

TEST_F(FieldElementLargePrimeTest, Multiplication) {
    // Test identity
    EXPECT_EQ(val1 * one, val1);
    EXPECT_EQ(one * val1, val1);

    // Test zero property
    EXPECT_EQ(val1 * zero, zero);
    EXPECT_EQ(zero * val1, zero);

    // Test commutativity
    EXPECT_EQ(val1 * val2, val2 * val1);

    // Test associativity (a * b) * c == a * (b * c)
    EXPECT_EQ((val1 * val2) * three, val1 * (val2 * three));

    // Test distributivity a * (b + c) == (a * b) + (a * c)
    EXPECT_EQ(val1 * (val2 + three), (val1 * val2) + (val1 * three));

    // Test multiplication by -1
    EXPECT_EQ(val1 * neg_one, -val1);
}

TEST_F(FieldElementLargePrimeTest, Inverse) {
    // Test inverse of 1
    EXPECT_EQ(one.inv(), one);

    // Test inverse of -1
    EXPECT_EQ(neg_one.inv(), neg_one);

    // Test inverse relationship with multiplication
    FieldElement val1_inv = val1.inv();
    EXPECT_EQ(val1 * val1_inv, one);

    FieldElement val2_inv = val2.inv();
    EXPECT_EQ(val2 * val2_inv, one);

    // Test inv(inv(a)) == a
    EXPECT_EQ((val1.inv()).inv(), val1);

    // Test inverse of zero (should fail - Field.hpp uses assert, gtest can't catch it directly)
    // We can't easily test the assert directly with EXPECT_DEATH or similar
    // without modifications, because assert usually terminates in debug builds.
    // If assert is disabled (NDEBUG), division by zero might lead to undefined behavior
    // or incorrect results depending on xgcd behavior with 0.
    // EXPECT_THROW(zero.inv(), std::exception); // This won't work for assert()

    // Test inverse of generator
    FieldElement gen_inv = gen.inv();
    EXPECT_EQ(gen * gen_inv, one);
}


TEST_F(FieldElementLargePrimeTest, Division) {
    // Test identity
    EXPECT_EQ(val1 / one, val1);

    // Test division by self
    EXPECT_EQ(val1 / val1, one);

    // Test relationship with inverse
    EXPECT_EQ(val1 / val2, val1 * val2.inv());

    // Test (a / b) * b == a
    EXPECT_EQ((val1 / val2) * val2, val1);
    EXPECT_EQ((val2 / val1) * val1, val2);

    // Test division by -1
    EXPECT_EQ(val1 / neg_one, -val1);

    // Test division of zero
    EXPECT_EQ(zero / val1, zero);

    // Test division by zero (similar issue to inverse of zero - relies on assert)
    // EXPECT_THROW(one / zero, std::exception); // Won't work for assert()
}

TEST_F(FieldElementLargePrimeTest, ExponentiationInt) {
    // Test exponent 0
    EXPECT_EQ(val1 ^ 0, one);
    EXPECT_EQ(zero ^ 0, one); // 0^0 is typically 1 in field contexts

    // Test exponent 1
    EXPECT_EQ(val1 ^ 1, val1);
    EXPECT_EQ(zero ^ 1, zero);
    EXPECT_EQ(one ^ 1, one);

    // Test small powers
    EXPECT_EQ(two ^ 2, FieldElement(4));
    EXPECT_EQ(two ^ 3, FieldElement(8));
    EXPECT_EQ(three ^ 2, FieldElement(9));

    // Test properties: (a^b)^c == a^(b*c)
    EXPECT_EQ((two ^ 2) ^ 3, two ^ 6); // (2^2)^3 = 4^3 = 64; 2^6 = 64
    EXPECT_EQ((three ^ 2) ^ 2, three ^ 4); // (3^2)^2 = 9^2 = 81; 3^4 = 81

    // Test properties: a^b * a^c == a^(b+c)
    EXPECT_EQ((two ^ 2) * (two ^ 3), two ^ 5); // 4 * 8 = 32; 2^5 = 32

    // Test Fermat's Little Theorem: a^(P-1) = 1 mod P (for a != 0)
    // This might be slow for large 'a', but should work for small 'a'.
    // Note: Exponentiation uses BigInt internally, but the exponent here is int.
    // The operator^(FieldElement) takes a BigInt exponent, which would be needed
    // for the full P-1 exponentiation if P-1 doesn't fit in int.
    // Let's try with a small base.
    // We need P-1 as a BigInt to use the correct operator^ overload.
    // However, Field.hpp only provides operator^(int).
    // To test Fermat's Little Theorem properly, we'd need operator^(BigInt) or
    // modify the test to use operator^(FieldElement(P-1)).

    // Let's test with operator^(FieldElement) instead:
    FieldElement p_minus_1_fe{P - 1};
    EXPECT_EQ(two ^ p_minus_1_fe, one);
    EXPECT_EQ(three ^ p_minus_1_fe, one);
    EXPECT_EQ(val1 ^ p_minus_1_fe, one); // This might be slow

    // Test negative exponent (requires operator^(int) which isn't in Field.hpp)
    // If operator^(int exp) handled negative exp using inv(), we could test:
    // EXPECT_EQ(two ^ -1, two.inv());
}

TEST_F(FieldElementLargePrimeTest, ExponentiationFieldElement) {
    FieldElement exp2{BigInt(2)};
    FieldElement exp3{BigInt(3)};
    FieldElement exp4{BigInt(4)};
    FieldElement exp5{BigInt(5)};
    FieldElement exp6{BigInt(6)};

    // Test exponent 0
    EXPECT_EQ(val1 ^ zero, one);
    EXPECT_EQ(zero ^ zero, one);

    // Test exponent 1
    EXPECT_EQ(val1 ^ one, val1);
    EXPECT_EQ(zero ^ one, zero);
    EXPECT_EQ(one ^ one, one);

    // Test small powers
    EXPECT_EQ(two ^ exp2, FieldElement(4));
    EXPECT_EQ(two ^ exp3, FieldElement(8));
    EXPECT_EQ(three ^ exp2, FieldElement(9));

    // Test properties: (a^b)^c == a^(b*c)
    // Note: Requires FieldElement multiplication for exponents
    EXPECT_EQ((two ^ exp2) ^ exp3, two ^ (exp2 * exp3)); // (2^2)^3 = 4^3 = 64; 2^(2*3) = 2^6 = 64
    EXPECT_EQ((three ^ exp2) ^ exp2, three ^ (exp2 * exp2)); // (3^2)^2 = 9^2 = 81; 3^(2*2) = 3^4 = 81

    // Test properties: a^b * a^c == a^(b+c)
    EXPECT_EQ((two ^ exp2) * (two ^ exp3), two ^ (exp2 + exp3)); // 4 * 8 = 32; 2^(2+3) = 2^5 = 32

    // Test Fermat's Little Theorem: a^(P-1) = 1 mod P (for a != 0)
    FieldElement p_minus_1_fe{P - 1};
    EXPECT_EQ(two ^ p_minus_1_fe, one);
    EXPECT_EQ(val1 ^ p_minus_1_fe, one); // This might be slow
    EXPECT_EQ(gen ^ p_minus_1_fe, one); // Generator property
}

// Basic test for the generator function
TEST_F(FieldElementLargePrimeTest, Generator) {
    // A generator g must not be 0 or 1
    EXPECT_NE(gen, zero);
    EXPECT_NE(gen, one);

    // Check g^(P-1) == 1 (already tested in ExponentiationFieldElement)
    FieldElement p_minus_1_fe{P - 1};
    EXPECT_EQ(gen ^ p_minus_1_fe, one);

    // A stronger test would be to check g^((P-1)/q) != 1 for all prime factors q of P-1.
    // P-1 = 407 * 2^119 = (11 * 37) * 2^119
    // This requires calculating large powers and might be too slow for a unit test.
    // Example (might be slow):
    // BigInt exp_p_minus_1_div_2 = (P - 1) / 2;
    // EXPECT_NE(gen ^ FieldElement(exp_p_minus_1_div_2), one);
    // BigInt exp_p_minus_1_div_11 = (P - 1) / 11;
    // EXPECT_NE(gen ^ FieldElement(exp_p_minus_1_div_11), one);
    // BigInt exp_p_minus_1_div_37 = (P - 1) / 37;
    // EXPECT_NE(gen ^ FieldElement(exp_p_minus_1_div_37), one);
}

// Basic test for primitive_nth_root
TEST_F(FieldElementLargePrimeTest, PrimitiveRoot) {
    // P = 1 + 407 * 2^119. The order is 2^119 * 407.
    // The function seems designed for powers of 2.
    BigInt order_pow2 = BigInt(1) << 119; // 2^119

    // Get the 2^119-th primitive root of unity
    FieldElement root_pow2 = primitive_nth_root(order_pow2);

    // Check property: root^n == 1
    EXPECT_EQ(root_pow2 ^ FieldElement(order_pow2), one);

    // Check property: root^(n/2) != 1 (primitive root property)
    // This ensures it's not a lower-order root.
    BigInt order_half_pow2 = order_pow2 >> 1; // 2^118
    EXPECT_NE(root_pow2 ^ FieldElement(order_half_pow2), one);

    // Test another power of 2 (e.g., 2^118)
    BigInt order_pow2_minus_1 = BigInt(1) << 118;
    FieldElement root_pow2_minus_1 = primitive_nth_root(order_pow2_minus_1);
    EXPECT_EQ(root_pow2_minus_1 ^ FieldElement(order_pow2_minus_1), one);
    EXPECT_NE(root_pow2_minus_1 ^ FieldElement(order_pow2_minus_1 >> 1), one);

    // Check relationship: primitive_nth_root(N)^2 == primitive_nth_root(N/2)
    // This assumes N is a power of 2 >= 2 in the implementation logic
    EXPECT_EQ(root_pow2 ^ 2, root_pow2_minus_1);
}

// Test xgcd indirectly via inverse/division, but a direct check is also good.
TEST(XgcdLargePrimeTest, BasicCases) {
    BigInt a, b, g;
    BigInt test_val = 12345;
    BigInt zero = 0;
    BigInt one = 1;

    // gcd(test_val, P) should be 1 since P is prime and test_val < P
    xgcd(test_val, P, a, b, g);
    EXPECT_EQ(g, one);
    // Verify Bezout's identity: a*test_val + b*P = g = 1
    // We need to handle potential negative 'a' from xgcd for modular inverse property
    FieldElement a_fe = FieldElement(a); // Reduce 'a' mod P correctly
    EXPECT_EQ(a_fe * FieldElement(test_val), FieldElement(one)); // Check if a mod P is the inverse

    // gcd(0, P) = P
    xgcd(zero, P, a, b, g);
    EXPECT_EQ(g, P);

    // gcd(P, test_val) = 1
    xgcd(P, test_val, a, b, g);
    EXPECT_EQ(g, one);
    // Check Bezout: a*P + b*test_val = 1 => (b mod P) * test_val = 1 mod P
    FieldElement b_fe = FieldElement(b);
    EXPECT_EQ(b_fe * FieldElement(test_val), FieldElement(one));
}
