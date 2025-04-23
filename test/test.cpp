#include "../src/Field.hpp"

using namespace std;
void test_inverse(){
    FieldElement one(1);
    FieldElement neg1(P - 1);
    FieldElement val1("123456789012345678901234567890123456789");
    FieldElement val2("987654321098765432109876543210987654321");

    cout << one.inv() << endl;
    cout << neg1.inv() << endl;
    cout << val1.inv() * val1 << endl;
    cout << val2.inv() * val2 << endl;

    cout << val1.inv().inv() << endl;
}

void test_exp() {
    cout << "Test Exponentiation" << endl;

    FieldElement one(1);
    FieldElement neg1(P - 1);
    FieldElement val1("123456789012345678901234567890123456789");
    FieldElement val2("987654321098765432109876543210987654321");

    // cout << (val1^0) << endl; // 1
    // cout << (FieldElement(0)^0) << endl; // 1
    // cout << (val1^1) << endl; // val1
    // cout << (FieldElement(0)^1) << endl; // 0
    // cout << (one^1) << endl; // 1

    FieldElement two(2);

    // cout << (two^2) << endl; // 4
    // cout << (two^3) << endl; // 8
    // cout << (FieldElement(3)^2) << endl; // 9

    // cout << ((two^2)^3) << ", " << (two^(2*3)) << endl; // 64, 64

    FieldElement three(3);

    // cout << ((three^2)^2) << ", " << (three^(2*2)) << endl; // 81, 81

    // cout << ((two^2) * (two^3)) << ", " << (two^(2+3)) << endl; 

    FieldElement p_minus_1_fe(P - 1);
    // cout << p_minus_1_fe << endl; // P - 1
    cout << (two^p_minus_1_fe) << endl; // 1
    // cout << (three^p_minus_1_fe) << endl; // 1
    // cout << (val1^p_minus_1_fe) << endl; // 1
}


void test_primitive_root() {
    cout << "Test Primitive Root" << endl;

    BigInt order_pow2 = BigInt(1) << 119; // 2^119

    FieldElement root_pow2 = primitive_nth_root(order_pow2);
    cout << (root_pow2^FieldElement(order_pow2)) << endl; // 1

    BigInt order_halp_pow2 = order_pow2 >> 1; // 2^118
    cout << (root_pow2^FieldElement(order_halp_pow2)) << endl; // 1

    BigInt order_pow2_minus_1 = BigInt(1) << 118;
    FieldElement root_pow2_minus_1 = primitive_nth_root(order_pow2_minus_1);

    cout << (root_pow2_minus_1^FieldElement(order_pow2_minus_1)) << endl; // 1
}

int main() {
    test_primitive_root();
    return 0;
}