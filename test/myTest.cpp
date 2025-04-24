#include "../src/Field.hpp"
#include "../src/Polynomial.hpp"
#include <iostream>
using namespace std;

Polynomial from_ints(const std::vector<int64_t>& ints) {
    std::vector<FieldElement> fe_coeffs;
    for (int64_t i : ints) {
        fe_coeffs.push_back(FieldElement(BigInt(i)));
    }
    return Polynomial(fe_coeffs);
}

void testEq() {
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


    cout << ((ZERO_POLY == Polynomial()) ? "true" : "false") << endl;
    cout << ((ZERO_POLY == P_CONST_ZERO) ? "true" : "false") << endl; // Different representations
    cout << ((P_CONST_ZERO == P_CONST_ONE) ? "true" : "false") << endl;
    cout << ((P_X == Polynomial(std::vector<FieldElement>{FE_ZERO, FE_ONE})) ? "true" : "false") << endl;
    cout << ((P_X == P_1_PLUS_2X) ? "true" : "false") << endl;

    Polynomial p1{std::vector<FieldElement>{FE_ONE, FE_TWO}}; // 1 + 2x
    Polynomial p2{std::vector<FieldElement>{FE_ONE, FE_TWO, FE_ZERO}}; // 1 + 2x + 0x^2

    cout << ((p1 == p2) ? "true" : "false") << endl; // Based on current implementation comparing vectors directly

    Polynomial p3{std::vector<FieldElement>{FE_ONE, FE_THREE}}; // 1 + 3x

    cout << ((p1 == p3) ? "true" : "false") << endl;
}

void testMul() {
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

    cout << (P_1_PLUS_2X * ZERO_POLY == ZERO_POLY ? "true" : "false") << endl;
    cout << (ZERO_POLY * P_1_PLUS_2X == ZERO_POLY ? "true" : "false") << endl;
}

void testDiv() {
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

    Polynomial quotient, remainder;

    P_1_PLUS_2X_PLUS_3X2.divided_by(P_CONST_ONE, quotient, remainder);

    cout << (quotient == P_1_PLUS_2X_PLUS_3X2 ? "true" : "false") << endl;
    cout << (remainder == ZERO_POLY ? "true" : "false") << endl;

    P_1_PLUS_2X_PLUS_3X2.divided_by(P_1_PLUS_2X_PLUS_3X2, quotient, remainder);
    cout << (quotient == P_CONST_ONE ? "true" : "false") << endl;
    cout << (remainder == ZERO_POLY ? "true" : "false") << endl;

    try
    {
        P_X.divided_by(ZERO_POLY, quotient, remainder);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    try
    {
        P_X / ZERO_POLY;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    try
    {
        P_X % ZERO_POLY;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    Polynomial p_x_plus_2x2 = from_ints({0, 1, 2});
    Polynomial expected_q1 = from_ints({1, 2});
    p_x_plus_2x2.divided_by(P_X, quotient, remainder);

    cout << "Polynomial: " << (string)p_x_plus_2x2 << endl;
    cout << "Divisor: " << (string)P_X << endl;
    cout << "Quotient: " << (string)quotient << endl;
    cout << "Remainder: " << (string)remainder << endl;

    cout << (quotient == expected_q1 ? "true" : "false") << endl;
    cout << (remainder == ZERO_POLY ? "true" : "false") << endl;
    cout << (p_x_plus_2x2 / P_X == expected_q1 ? "true" : "false") << endl;
    cout << (p_x_plus_2x2 % P_X == ZERO_POLY ? "true" : "false") << endl;

    Polynomial expected_q2 = from_ints({2, 3});
    Polynomial expected_r2 = from_ints({1});
    P_1_PLUS_2X_PLUS_3X2.divided_by(P_X, quotient, remainder);
    // 1 + 2x + 3x^2 = (2 + 3x)(x) + 1

    cout << "Polynomial: " << (string)P_1_PLUS_2X_PLUS_3X2 << endl;
    cout << "Divisor: " << (string)P_X << endl;
    cout << "Quotient: " << (string)quotient << endl;
    cout << "Remainder: " << (string)remainder << endl;

    cout << (quotient == expected_q2 ? "true" : "false") << endl;
    cout << (remainder == expected_r2 ? "true" : "false") << endl;
    cout << (string)remainder << endl;
    cout << (P_1_PLUS_2X_PLUS_3X2 % P_X == expected_r2 ? "true" : "false") << endl;
    try {
        P_1_PLUS_2X_PLUS_3X2 / P_X;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void testExp(){
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

    cout << ((P_1_PLUS_2X ^ BigInt(0)) == P_CONST_ONE ? "true" : "false") << endl;
    cout << ((ZERO_POLY ^ BigInt(0)) == P_CONST_ONE ? "true" : "false") << endl;
    cout << ((P_1_PLUS_2X ^ BigInt(1)) == P_1_PLUS_2X ? "true" : "false") << endl;
    cout << ((ZERO_POLY ^ BigInt(1)) == ZERO_POLY ? "true" : "false") << endl;


}

int main(){
    testExp();
    return 0;
}