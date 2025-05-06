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

void test() {
    Polynomial p = from_ints({16, 6, 2, 0, 1, 11, 15});
    Polynomial z = zerofier_domain({FieldElement(1), FieldElement(13), FieldElement(16)});
    Polynomial quotient = p / z;
    cout << "Polynomial: " << p << std::endl;
    cout << "Zerofier: " << z << std::endl;
    cout << "Quotient: " << quotient << std::endl;

    FieldElement omega(3);
    vector<FieldElement> domain(16, FieldElement(0));
    for (size_t i = 0; i != domain.size(); i++) {
        domain[i] = omega ^ static_cast<ttmath::ulint>(i);
        cout << domain[i] << " ";
    }
    cout << std::endl;

    cout << "Prover commits evaluation of quotient polynomial at domain points" << std::endl;
    for (size_t i = 0; i != domain.size(); i++) {
        FieldElement eval = quotient[domain[i]];
        cout << eval << " ";
    }
    cout << std::endl;

    FieldElement challenge = FieldElement(5);
    cout << "Verifier challenges prover with a random point" << challenge << std::endl;
    cout << "Prover folds the polynomial according to the challenge" << std::endl;

    vector<FieldElement> folded_domain;
    for (size_t i = 0; i != domain.size() / 2; i++) {
        folded_domain.push_back(omega^(2 * i));
    }
    cout << "Folded domain: ";
    for (size_t i = 0; i != folded_domain.size(); i++) {
        cout << folded_domain[i] << " ";
    }
    cout << std::endl;

    vector<FieldElement> folded_evals;
    cout << "Folded evaluations: ";
    for (size_t i = 0; i != folded_domain.size(); i++) {
        FieldElement two = FieldElement(2);
        FieldElement even = quotient[(omega^i)] + quotient[ -(omega^i)];
        even = even * (two.inv());

        FieldElement odd = quotient[(omega^i)] - quotient[ -(omega^i) ];
        odd = odd * (two.inv());
        odd = odd * ((omega^i).inv());
        odd = odd * (challenge);

        folded_evals.push_back(odd + even);
        cout << folded_evals[i] << " ";
    }
    cout << std::endl;

    cout << "Verifier samples a random point from the folded domain i = " << FieldElement(3) << std::endl;
    cout << "calculate point A(omega^i, f(omega^i)): ( " << (omega^3) << ", " << quotient[(omega^3)] << " )" << std::endl;
    cout << "calculate point B(omega^{N / 2 + i}, f(omega^{N / 2 + i})): ( " << (omega^(8 + 3)) << ", " << quotient[(omega^(8 + 3))] << " )" << std::endl;
    cout << "calculate point C(\\alpha, f^*(omega^{2i})): ( " << challenge << ", " << folded_evals[3] << " )" << std::endl;

    cout << "Verifier performs a collinearity test: " << (test_colinearity({FieldElement(10), FieldElement(7), FieldElement(5)}, {FieldElement(10), FieldElement(8), FieldElement(1)}) == true ? "accept" : "reject") << std::endl;

    cout << "Both prover and verifier sets omega_new = omega^2 = " << (omega^2) << std::endl;
    cout << "Round 2 domain: ";
    omega = omega^2;
    domain = folded_domain;
    vector<FieldElement> new_eval = folded_evals;
    for (size_t i = 0; i != domain.size(); i++) {
        cout << domain[i] << " ";
    }
    cout << std::endl;
    cout << "Round 2 evaluations: ";
    for (size_t i = 0; i != new_eval.size(); i++) {
        cout << new_eval[i] << " ";
    }
    cout << std::endl;

    challenge = FieldElement(4);
    cout << "Verifier challenges prover with a random point " << challenge << std::endl;
    cout << "Prover folds the polynomial according to the challenge" << std::endl;
    folded_domain.clear();
    for (size_t i = 0; i != domain.size() / 2; i++) {
        folded_domain.push_back(omega^(2 * i));
    }
    cout << "Folded domain: ";
    for (size_t i = 0; i != folded_domain.size(); i++) {
        cout << folded_domain[i] << " ";
    }
    cout << std::endl;
    folded_evals.clear();
    cout << "Folded evaluations: ";
    for (size_t i = 0; i != folded_domain.size(); i++) {
        FieldElement two = FieldElement(2);
        FieldElement even = new_eval[i] + new_eval[ i + domain.size()/2 ];
        even = even * (two.inv());

        FieldElement odd = new_eval[i] - new_eval[ i + domain.size()/2 ];
        odd = odd * (two.inv());
        odd = odd * ((omega^i).inv());
        odd = odd * (challenge);

        folded_evals.push_back(odd + even);
        cout << folded_evals[i] << " ";
    }
    cout << std::endl;
        
}

int main(){
    test();
    return 0;
}