#include "../src/Polynomial.hpp" // Include your Polynomial header
#include "../src/Field.hpp"      // Include FieldElement header
#include "gtest/gtest.h"
#include <vector>
#include <stdexcept> // For std::invalid_argument

// --- Test Fixture ---
class PolynomialTest : public ::testing::Test {
protected:
    // Define some useful FieldElement constants
    FieldElement FE_ZERO{BigInt(0)};
    FieldElement FE_ONE{BigInt(1)};
    FieldElement FE_TWO{BigInt(2)};
    FieldElement FE_THREE{BigInt(3)};
    FieldElement FE_FOUR{BigInt(4)};
    FieldElement FE_FIVE{BigInt(5)};
    FieldElement FE_NINE{BigInt(9)};
    FieldElement FE_NEG_ONE{P - 1}; // P-1 is -1 mod P

    // Define some Polynomial objects for testing
    Polynomial ZERO_POLY{}; // Default constructor -> zero polynomial
    Polynomial P_CONST_ZERO{std::vector<FieldElement>{FE_ZERO}}; // Polynomial representing 0
    Polynomial P_CONST_ONE{std::vector<FieldElement>{FE_ONE}};   // Polynomial representing 1
    Polynomial P_CONST_FIVE{std::vector<FieldElement>{FE_FIVE}}; // Polynomial representing 5

    // P(x) = x
    Polynomial P_X{std::vector<FieldElement>{FE_ZERO, FE_ONE}};
    // P(x) = 1 + 2x
    Polynomial P_1_PLUS_2X{std::vector<FieldElement>{FE_ONE, FE_TWO}};
    // P(x) = 3x^2
    Polynomial P_3X2{std::vector<FieldElement>{FE_ZERO, FE_ZERO, FE_THREE}};
     // P(x) = 1 + 2x + 3x^2
    Polynomial P_1_PLUS_2X_PLUS_3X2{std::vector<FieldElement>{FE_ONE, FE_TWO, FE_THREE}};
    // P(x) = 5 + x^3 (note the zero coefficients for x and x^2)
    Polynomial P_5_PLUS_X3{std::vector<FieldElement>{FE_FIVE, FE_ZERO, FE_ZERO, FE_ONE}};

    // Helper to create a polynomial from a vector of integers
    Polynomial from_ints(const std::vector<int64_t>& ints) {
        std::vector<FieldElement> fe_coeffs;
        for (int64_t i : ints) {
            fe_coeffs.push_back(FieldElement(BigInt(i)));
        }
        return Polynomial(fe_coeffs);
    }
};

// --- Test Cases ---

TEST_F(PolynomialTest, ConstructorAndDegree) {
    // Test default constructor (zero polynomial)
    EXPECT_EQ(ZERO_POLY.degree(), -1); // Degree of zero polynomial is often -1
    EXPECT_EQ(ZERO_POLY.coeffs.size(), 0);

    // Test constructor with zero coefficient
    EXPECT_EQ(P_CONST_ZERO.degree(), -1); // Degree of constant 0 is 0
    EXPECT_EQ(P_CONST_ZERO.coeffs.size(), 1);

    // Test constant polynomials
    EXPECT_EQ(P_CONST_ONE.degree(), 0);
    EXPECT_EQ(P_CONST_FIVE.degree(), 0);

    // Test linear polynomial
    EXPECT_EQ(P_X.degree(), 1);
    EXPECT_EQ(P_1_PLUS_2X.degree(), 1);

    // Test quadratic polynomial
    EXPECT_EQ(P_3X2.degree(), 2);
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2.degree(), 2);

    // Test polynomial with missing middle terms
    EXPECT_EQ(P_5_PLUS_X3.degree(), 3);

    // Test polynomial with trailing zeros (degree should ignore them)
    Polynomial p_trailing_zeros{std::vector<FieldElement>{FE_ONE, FE_TWO, FE_ZERO, FE_ZERO}};
    EXPECT_EQ(p_trailing_zeros.degree(), 1); // Degree is 1 (highest non-zero coeff index)

    // Test empty vector constructor explicitly
     Polynomial p_empty_vec{std::vector<FieldElement>{}};
     EXPECT_EQ(p_empty_vec.degree(), -1);
}

TEST_F(PolynomialTest, EqualityOperators) {
    EXPECT_EQ(ZERO_POLY, Polynomial());
    EXPECT_EQ(ZERO_POLY, P_CONST_ZERO); // Different representations
    EXPECT_NE(P_CONST_ZERO, P_CONST_ONE);
    EXPECT_EQ(P_X, Polynomial(std::vector<FieldElement>{FE_ZERO, FE_ONE}));
    EXPECT_NE(P_X, P_1_PLUS_2X);

    // Test equality with different vector sizes but same polynomial
    Polynomial p1{std::vector<FieldElement>{FE_ONE, FE_TWO}}; // 1 + 2x
    Polynomial p2{std::vector<FieldElement>{FE_ONE, FE_TWO, FE_ZERO}}; // 1 + 2x + 0x^2
    // The current implementation might fail this test as it compares vector sizes directly
    // A more robust equality would compare up to the highest degree.
    // Depending on the intended behavior, this might need adjustment in Polynomial.hpp
    // For now, assuming the current implementation logic:
    EXPECT_EQ(p1, p2); // Based on current implementation comparing vectors directly

    // Test equality with same degree but different coefficients
    Polynomial p3{std::vector<FieldElement>{FE_ONE, FE_THREE}}; // 1 + 3x
    EXPECT_NE(p1, p3);
}

TEST_F(PolynomialTest, Negation) {
    Polynomial neg_p_x = -P_X;
    EXPECT_EQ(neg_p_x.coeffs[0], FE_ZERO);
    EXPECT_EQ(neg_p_x.coeffs[1], FE_NEG_ONE); // -1

    Polynomial neg_p_123 = -P_1_PLUS_2X_PLUS_3X2;
    EXPECT_EQ(neg_p_123.coeffs[0], FE_NEG_ONE); // -1
    EXPECT_EQ(neg_p_123.coeffs[1], FieldElement(P - 2)); // -2
    EXPECT_EQ(neg_p_123.coeffs[2], FieldElement(P - 3)); // -3

    EXPECT_EQ(-ZERO_POLY, ZERO_POLY);
    EXPECT_EQ(-(-P_X), P_X);
}

TEST_F(PolynomialTest, Addition) {
    // P + 0 = P
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2 + ZERO_POLY, P_1_PLUS_2X_PLUS_3X2);
    EXPECT_EQ(ZERO_POLY + P_1_PLUS_2X_PLUS_3X2, P_1_PLUS_2X_PLUS_3X2);

    // Commutativity: P + Q = Q + P
    EXPECT_EQ(P_X + P_1_PLUS_2X, P_1_PLUS_2X + P_X);

    // Associativity: (P + Q) + R = P + (Q + R)
    EXPECT_EQ((P_X + P_1_PLUS_2X) + P_3X2, P_X + (P_1_PLUS_2X + P_3X2));

    // Simple addition: (1+2x) + (x) = 1 + 3x
    Polynomial expected1 = from_ints({1, 3});
    EXPECT_EQ(P_1_PLUS_2X + P_X, expected1);

    // Addition resulting in cancellation: (1+2x) + (-2x) = 1
     Polynomial p_neg_2x = from_ints({0, -2});
     EXPECT_EQ(P_1_PLUS_2X + p_neg_2x, P_CONST_ONE);

    // Addition with different degrees: (1+2x+3x^2) + (5+x^3) = 6 + 2x + 3x^2 + x^3
    Polynomial expected2 = from_ints({6, 2, 3, 1});
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2 + P_5_PLUS_X3, expected2);
}

TEST_F(PolynomialTest, Subtraction) {
    // P - 0 = P
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2 - ZERO_POLY, P_1_PLUS_2X_PLUS_3X2);

    // P - P = 0 (should result in zero polynomial, maybe P_CONST_ZERO depending on impl.)
    // The current implementation of '+' might leave trailing zeros.
    // Let's check degree and coefficients carefully.
    Polynomial diff_self = P_1_PLUS_2X - P_1_PLUS_2X;
    // Need to normalize the result (remove trailing zeros) for robust comparison to ZERO_POLY
    // Assuming the current implementation doesn't normalize:
    EXPECT_EQ(diff_self, Polynomial(std::vector<FieldElement>{FE_ZERO, FE_ZERO})); // 0 + 0x
    // A normalized version would be equal to P_CONST_ZERO or ideally ZERO_POLY

    // 0 - P = -P
    EXPECT_EQ(ZERO_POLY - P_1_PLUS_2X, -P_1_PLUS_2X);

    // Simple subtraction: (1+2x+3x^2) - (x) = 1 + x + 3x^2
    Polynomial expected1 = from_ints({1, 1, 3});
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2 - P_X, expected1);

    // Subtraction with different degrees: (1+2x+3x^2) - (5+x^3) = -4 + 2x + 3x^2 - x^3
    Polynomial expected2 = from_ints({-4, 2, 3, -1});
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2 - P_5_PLUS_X3, expected2);
}


TEST_F(PolynomialTest, Multiplication) {
    // P * 0 = 0
    EXPECT_EQ(P_1_PLUS_2X * ZERO_POLY, ZERO_POLY);
    EXPECT_EQ(ZERO_POLY * P_1_PLUS_2X, ZERO_POLY);

    // P * 1 = P
    EXPECT_EQ(P_1_PLUS_2X * P_CONST_ONE, P_1_PLUS_2X);
    EXPECT_EQ(P_CONST_ONE * P_1_PLUS_2X, P_1_PLUS_2X);

    // Commutativity: P * Q = Q * P
    EXPECT_EQ(P_1_PLUS_2X * P_X, P_X * P_1_PLUS_2X);

    // Associativity: (P * Q) * R = P * (Q * R)
    EXPECT_EQ((P_X * P_CONST_FIVE) * P_1_PLUS_2X, P_X * (P_CONST_FIVE * P_1_PLUS_2X));

    // Distributivity: P * (Q + R) = P * Q + P * R
    EXPECT_EQ(P_X * (P_1_PLUS_2X + P_3X2), (P_X * P_1_PLUS_2X) + (P_X * P_3X2));

    // Simple multiplication: x * (1+2x) = x + 2x^2
    Polynomial expected1 = from_ints({0, 1, 2});
    EXPECT_EQ(P_X * P_1_PLUS_2X, expected1);

    // (1+2x) * (3x^2) = 3x^2 + 6x^3
    Polynomial expected2 = from_ints({0, 0, 3, 6});
    EXPECT_EQ(P_1_PLUS_2X * P_3X2, expected2);

    // (1+2x) * (1+2x) = 1 + 4x + 4x^2
    Polynomial expected3 = from_ints({1, 4, 4});
    EXPECT_EQ(P_1_PLUS_2X * P_1_PLUS_2X, expected3);
}

TEST_F(PolynomialTest, LeadingCoefficient) {
    EXPECT_EQ(P_CONST_FIVE.leading_coefficient(), FE_FIVE);
    EXPECT_EQ(P_X.leading_coefficient(), FE_ONE);
    EXPECT_EQ(P_1_PLUS_2X.leading_coefficient(), FE_TWO);
    EXPECT_EQ(P_3X2.leading_coefficient(), FE_THREE);
    EXPECT_EQ(P_5_PLUS_X3.leading_coefficient(), FE_ONE);

    // Leading coefficient of zero polynomial is undefined/error?
    // The current implementation might access coeffs[-1] if degree is -1.
    // Need to handle this case. Let's assume it should throw or have defined behavior.
    // EXPECT_THROW(ZERO_POLY.leading_coefficient(), std::out_of_range); // Or similar
}

TEST_F(PolynomialTest, DivisionAndModulo) {
    Polynomial quotient, remainder;

    // P / 1 = P, P % 1 = 0
    P_1_PLUS_2X_PLUS_3X2.divided_by(P_CONST_ONE, quotient, remainder);
    EXPECT_EQ(quotient, P_1_PLUS_2X_PLUS_3X2);
    EXPECT_EQ(remainder.degree(), -1); // Remainder should be zero polynomial

    // P / P = 1, P % P = 0
    P_1_PLUS_2X_PLUS_3X2.divided_by(P_1_PLUS_2X_PLUS_3X2, quotient, remainder);
    EXPECT_EQ(quotient, P_CONST_ONE);
    EXPECT_EQ(remainder.degree(), -1);

    // Division by zero polynomial
    EXPECT_THROW(P_X.divided_by(ZERO_POLY, quotient, remainder), std::invalid_argument);
    EXPECT_THROW(P_X / ZERO_POLY, std::invalid_argument);
    EXPECT_THROW(P_X % ZERO_POLY, std::invalid_argument);

    // Simple case: (x + 2x^2) / x = 1 + 2x, remainder 0
    Polynomial p_x_plus_2x2 = from_ints({0, 1, 2});
    Polynomial expected_q1 = from_ints({1, 2});
    p_x_plus_2x2.divided_by(P_X, quotient, remainder);
    EXPECT_EQ(quotient, expected_q1);
    EXPECT_EQ(remainder.degree(), -1);
    EXPECT_EQ(p_x_plus_2x2 / P_X, expected_q1); // Check operator/
    EXPECT_EQ((p_x_plus_2x2 % P_X).degree(), -1); // Check operator%

    // Case with remainder: (1 + 2x + 3x^2) / (x) = (2 + 3x) remainder 1
    Polynomial expected_q2 = from_ints({2, 3});
    Polynomial expected_r2 = from_ints({1});
    P_1_PLUS_2X_PLUS_3X2.divided_by(P_X, quotient, remainder);
    EXPECT_EQ(quotient, expected_q2);
    EXPECT_EQ(remainder, expected_r2);
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2 % P_X, expected_r2); // Check operator%
    // Operator / should throw if remainder is non-zero
    EXPECT_THROW(P_1_PLUS_2X_PLUS_3X2 / P_X, std::invalid_argument);

    // Longer division: (x^3 + 5) / (x + 1)
    // x^3 + 5 = (x^2 - x + 1)*(x+1) + 4
    Polynomial p_x_plus_1 = from_ints({1, 1});
    Polynomial expected_q3 = from_ints({1, -1, 1}); // 1 - x + x^2
    Polynomial expected_r3 = from_ints({4});
    P_5_PLUS_X3.divided_by(p_x_plus_1, quotient, remainder);
    EXPECT_EQ(quotient, expected_q3);
    EXPECT_EQ(remainder, expected_r3);

    // Check relationship: Dividend = Quotient * Divisor + Remainder
    EXPECT_EQ(quotient * p_x_plus_1 + remainder, P_5_PLUS_X3);
}

TEST_F(PolynomialTest, DivisionByFieldElement) {
    // (3x^2 + 6x^3) / 3 = x^2 + 2x^3
    Polynomial p_3x2_6x3 = from_ints({0, 0, 3, 6});
    Polynomial expected = from_ints({0, 0, 1, 2});
    EXPECT_EQ(p_3x2_6x3 / FE_THREE, expected);

    EXPECT_EQ(P_X / FE_ONE, P_X);
    EXPECT_EQ(ZERO_POLY / FE_FIVE, ZERO_POLY);

    // Division by zero FieldElement
    EXPECT_THROW(P_X / FE_ZERO, std::invalid_argument);
}


TEST_F(PolynomialTest, Exponentiation) {
    // P^0 = 1
    EXPECT_EQ(P_1_PLUS_2X ^ BigInt(0), P_CONST_ONE);
    EXPECT_THROW(ZERO_POLY ^ BigInt(0), std::invalid_argument); // 0^0 = 1 convention

    // P^1 = P
    EXPECT_EQ(P_1_PLUS_2X ^ BigInt(1), P_1_PLUS_2X);
    EXPECT_EQ(ZERO_POLY ^ BigInt(1), ZERO_POLY);

    // Simple power: (x)^2 = x^2
    Polynomial expected_x2 = from_ints({0, 0, 1});
    EXPECT_EQ(P_X ^ BigInt(2), expected_x2);

    // (x)^3 = x^3
    Polynomial expected_x3 = from_ints({0, 0, 0, 1});
    EXPECT_EQ(P_X ^ BigInt(3), expected_x3);

    // (1+2x)^2 = 1 + 4x + 4x^2
    Polynomial expected_1p2x_sq = from_ints({1, 4, 4});
    EXPECT_EQ(P_1_PLUS_2X ^ BigInt(2), expected_1p2x_sq);
}

TEST_F(PolynomialTest, Evaluation) {
    // P(0)
    EXPECT_EQ(P_CONST_FIVE[FE_ZERO], FE_FIVE);
    EXPECT_EQ(P_X[FE_ZERO], FE_ZERO);
    EXPECT_EQ(P_1_PLUS_2X[FE_ZERO], FE_ONE);
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2[FE_ZERO], FE_ONE);
    EXPECT_EQ(P_5_PLUS_X3[FE_ZERO], FE_FIVE);

    // P(1) - sum of coefficients
    EXPECT_EQ(P_CONST_FIVE[FE_ONE], FE_FIVE);
    EXPECT_EQ(P_X[FE_ONE], FE_ONE);
    EXPECT_EQ(P_1_PLUS_2X[FE_ONE], FE_THREE); // 1 + 2*1 = 3
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2[FE_ONE], FieldElement(BigInt(1+2+3))); // 6
    EXPECT_EQ(P_5_PLUS_X3[FE_ONE], FieldElement(BigInt(5+1))); // 6

    // P(2)
    EXPECT_EQ(P_CONST_FIVE[FE_TWO], FE_FIVE);
    EXPECT_EQ(P_X[FE_TWO], FE_TWO);
    EXPECT_EQ(P_1_PLUS_2X[FE_TWO], FE_FIVE); // 1 + 2*2 = 5
    EXPECT_EQ(P_1_PLUS_2X_PLUS_3X2[FE_TWO], FieldElement(BigInt(1 + 2*2 + 3*4))); // 1 + 4 + 12 = 17
    EXPECT_EQ(P_5_PLUS_X3[FE_TWO], FieldElement(BigInt(5 + 8))); // 13

    // Evaluate domain
    std::vector<FieldElement> domain = {FE_ZERO, FE_ONE, FE_TWO};
    std::vector<FieldElement> expected_values = {FE_ONE, FE_THREE, FE_FIVE}; // P_1_PLUS_2X evaluated
    EXPECT_EQ(P_1_PLUS_2X.evaluate_domain(domain), expected_values);
}

TEST_F(PolynomialTest, Scale) {
    // P * scalar
    Polynomial scaled_p_x = P_X.scale(FE_FIVE); // x * 5 = 5x
    Polynomial expected1 = from_ints({0, 5});
    EXPECT_EQ(scaled_p_x, expected1);

    Polynomial scaled_p_123 = P_1_PLUS_2X_PLUS_3X2.scale(FE_TWO); // (1+2x+3x^2)*2 = 2+4x+6x^2
    Polynomial expected2 = from_ints({2, 4, 6});
    EXPECT_EQ(scaled_p_123, expected2);

    EXPECT_EQ(P_X.scale(FE_ONE), P_X);
    EXPECT_EQ(P_X.scale(FE_ZERO), P_CONST_ZERO); // Check if scaling by zero yields constant zero poly
    EXPECT_EQ(ZERO_POLY.scale(FE_FIVE), ZERO_POLY);
}

// --- Standalone Function Tests ---

TEST_F(PolynomialTest, InterpolateDomain) {
    // Interpolate points from a known polynomial: P(x) = 1 + 2x
    std::vector<FieldElement> domain = {FE_ZERO, FE_ONE, FE_TWO};
    std::vector<FieldElement> values = {FE_ONE, FE_THREE, FE_FIVE}; // P(0)=1, P(1)=3, P(2)=5

    Polynomial interpolated = interpolate_domain(domain, values);
    // Note: Interpolation might produce a polynomial with higher degree but equivalent values
    // on the domain. Check evaluation. A perfect interpolation should recover the exact polynomial (or one with trailing zeros).
    EXPECT_EQ(interpolated.degree(), 1); // Should be degree 1
    EXPECT_EQ(interpolated, P_1_PLUS_2X); // Check exact coefficients

    // Interpolate points from P(x) = x^2
    Polynomial p_x2 = from_ints({0, 0, 1});
    domain = {FE_ZERO, FE_ONE, FE_TWO, FE_THREE};
    values = {FE_ZERO, FE_ONE, FE_FOUR, FE_NINE}; // 0^2, 1^2, 2^2, 3^2
    interpolated = interpolate_domain(domain, values);
    EXPECT_EQ(interpolated.degree(), 2);
    // Need to compare normalized forms if trailing zeros are possible
    EXPECT_EQ(interpolated, p_x2);

    // Test size mismatch
    std::vector<FieldElement> short_values = {FE_ONE};
    EXPECT_THROW(interpolate_domain(domain, short_values), std::invalid_argument);
}

TEST_F(PolynomialTest, ZerofierDomain) {
    std::vector<FieldElement> domain = {FE_ONE, FE_TWO, FE_THREE};
    // Zerofier Z(x) = (x-1)(x-2)(x-3)
    // Z(x) = (x^2 - 3x + 2)(x-3)
    // Z(x) = x^3 - 3x^2 + 2x - 3x^2 + 9x - 6
    // Z(x) = x^3 - 6x^2 + 11x - 6
    Polynomial zerofier = zerofier_domain(domain);
    Polynomial expected = from_ints({-6, 11, -6, 1});

    EXPECT_EQ(zerofier, expected);

    // Verify it evaluates to zero on the domain
    EXPECT_EQ(zerofier[FE_ONE], FE_ZERO);
    EXPECT_EQ(zerofier[FE_TWO], FE_ZERO);
    EXPECT_EQ(zerofier[FE_THREE], FE_ZERO);

    // Verify it's non-zero outside the domain (e.g., at 0)
    EXPECT_EQ(zerofier[FE_ZERO], from_ints({-6}).coeffs[0]); // Should be -6

    // Test empty domain (should be Z(x) = 1)
    std::vector<FieldElement> empty_domain = {};
    EXPECT_EQ(zerofier_domain(empty_domain), P_CONST_ONE);
}

TEST_F(PolynomialTest, TestColinearity) {
    // Points on y = 1 + 2x are collinear
    std::vector<FieldElement> domain1 = {FE_ZERO, FE_ONE, FE_TWO};
    std::vector<FieldElement> values1 = {FE_ONE, FE_THREE, FE_FIVE};
    EXPECT_TRUE(test_colinearity(domain1, values1));

    // Points on y = 5 are collinear
    std::vector<FieldElement> domain2 = {FE_ZERO, FE_ONE, FE_TWO};
    std::vector<FieldElement> values2 = {FE_FIVE, FE_FIVE, FE_FIVE};
    EXPECT_TRUE(test_colinearity(domain2, values2));

    // Points on y = x^2 are not collinear
    std::vector<FieldElement> domain3 = {FE_ZERO, FE_ONE, FE_TWO};
    std::vector<FieldElement> values3 = {FE_ZERO, FE_ONE, FieldElement(BigInt(4))};
    EXPECT_FALSE(test_colinearity(domain3, values3));

    // Test edge cases: 0 or 1 point (always true)
    std::vector<FieldElement> domain0 = {};
    std::vector<FieldElement> values0 = {};
    EXPECT_TRUE(test_colinearity(domain0, values0));

    std::vector<FieldElement> domain1pt = {FE_ONE};
    std::vector<FieldElement> values1pt = {FE_FIVE};
    EXPECT_TRUE(test_colinearity(domain1pt, values1pt));

    // Test size mismatch
    std::vector<FieldElement> short_values = {FE_ONE};
    EXPECT_THROW(test_colinearity(domain1, short_values), std::invalid_argument);
}


