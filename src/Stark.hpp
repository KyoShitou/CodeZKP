#ifndef STARK_HPP
#define STARK_HPP

#include <vector>
#include <tuple>
#include <cmath>
#include "../src/MPolynomial.hpp"
#include "../src/Polynomial.hpp"
#include "../src/Field.hpp"
#include "../src/FRI.hpp"
#include "../src/merklecpp.h"

using std::tuple;
using std::log2;

#include <iostream>
using std::cout;
using std::endl;

namespace STARK {
    
    vector<uint8_t> serialize_boundary_commitment(vector<vector<FieldElement> > &boundary_quotient_codewords) {
        vector<uint8_t> serialized_boundary_commitment;
        for (auto &codeword : boundary_quotient_codewords) {
            merkle::Tree tree;
            for (const auto &code : codeword) {
                tree.insert(hash_from_FieldElement(code));
            }
            merkle::Hash root = tree.root();
            vector<uint8_t> serialized_root(root.bytes, root.bytes + sizeof(root));
            serialized_boundary_commitment.insert(serialized_boundary_commitment.end(), serialized_root.begin(), serialized_root.end());
        }
        return serialized_boundary_commitment;
    }   

    void prove(
        vector<vector<FieldElement> > &trace_matrix,
        vector<MPolynomial> &transition_constraints,
        vector<tuple<size_t, size_t, FieldElement> > &boundary_constraints,
        void (*commit)(void*),
        void (*get_challenge)(void*),
        vector<void(*)(void*)> &fri_fns,
        size_t transition_constraints_degree=2,
        size_t expansion_factor=4,
        size_t num_randomizors=2
    ) {
        size_t trace_length = trace_matrix.size() + num_randomizors;
        size_t register_count = trace_matrix[0].size();
        size_t omicron_domain_length = 1 << ((size_t)log2(trace_length * transition_constraints_degree));
        size_t fri_domain_length = omicron_domain_length * expansion_factor;


        FieldElement g = generator();
        FieldElement omega = primitive_nth_root(static_cast<ttmath::ulint>(fri_domain_length));
        FieldElement omicron = primitive_nth_root(static_cast<ttmath::ulint>(omicron_domain_length));

        for (size_t i = 0; i != num_randomizors; i++) {
            trace_matrix.push_back(vector<FieldElement>(register_count, FieldElement((i + 1) * 20)));
        }

        cout << "Trace matrix: " << endl;
        for (size_t i = 0; i != trace_matrix.size(); i++) {
            cout << "Row " << i << ": ";
            for (size_t j = 0; j != trace_matrix[i].size(); j++) {
                cout << trace_matrix[i][j] << " ";
            }
            cout << endl;
        }

        vector<FieldElement> fri_domain(fri_domain_length);
        for (size_t i = 0; i != fri_domain_length; i++) {
            fri_domain[i] = (g * (omega^i));
        }

        vector<FieldElement> trace_domain(trace_length);
        for (size_t i = 0; i != trace_length; i++) {
            trace_domain[i] = omicron^(i);
        }

        vector<Polynomial> trace_polynomials;
        for (size_t i = 0; i != register_count; i++) {
            vector<FieldElement> single_reg_trace(trace_length);
            for (size_t j = 0; j != trace_length; j++) single_reg_trace[j] = trace_matrix[j][i];
            cout << "single_reg_trace: ";
            for (auto &point : single_reg_trace) {
                cout << point << " ";
            }
            cout << endl;
            cout << "single_reg_trace size: " << single_reg_trace.size() << endl;
            cout << "trace_domain size: " << trace_domain.size() << endl;
            trace_polynomials.push_back(interpolate_domain(trace_domain, single_reg_trace));
            cout << "Trace polynomial " << i << ": " << trace_polynomials[i] << std::endl;
        }

        vector<Polynomial> boundary_quotients(register_count);

        // cout << "Boundary constraints: ";
        // for (auto &point : boundary_constraints) {
        //     cout << "(" << std::get<0>(point) << ", " << std::get<1>(point) << ", " << std::get<2>(point) << ") ";
        // }
        for (size_t i = 0; i != register_count; i++) {
            vector<tuple<size_t, FieldElement> > single_reg_boundary_constraints;
            
            for (auto &point : boundary_constraints) {
                if (std::get<1>(point) == i) {
                    // cout << "Found boundary constraint for register " << i << ": (" << std::get<0>(point) << ", " << std::get<1>(point) << ", " << std::get<2>(point) << ")" << endl;
                    single_reg_boundary_constraints.push_back(std::make_tuple(std::get<0>(point), std::get<2>(point)));
                }
            }
            
            if (single_reg_boundary_constraints.size() == 0) {
                continue;
            }
            vector<FieldElement> single_reg_boundary_domain;
            vector<FieldElement> single_reg_boundary_values;
            for (auto &point : single_reg_boundary_constraints) {
                single_reg_boundary_domain.push_back(omicron^(std::get<0>(point)));
                single_reg_boundary_values.push_back(std::get<1>(point));
            }

            cout << "single_reg_boundary_domain: ";
            for (auto &point : single_reg_boundary_domain) {
                cout << point << " ";
            }
            cout << endl;
            cout << "single_reg_boundary_values: ";
            for (auto &point : single_reg_boundary_values) {
                cout << point << " ";
            }
            cout << endl;

            Polynomial zerofier = zerofier_domain(single_reg_boundary_domain);
            Polynomial boundary_constraints_interpolant = interpolate_domain(single_reg_boundary_domain, single_reg_boundary_values);
            boundary_quotients[i] = (trace_polynomials[i] - boundary_constraints_interpolant) / zerofier;
        }

        cout << "FRI domain: ";
        for (auto &point : fri_domain) {
            cout << point << " ";
        }
        cout << endl;

        vector<vector<FieldElement> > boundary_quotient_codewords(register_count);
        for (size_t i = 0; i != register_count; i++) {
            boundary_quotient_codewords[i] = boundary_quotients[i].evaluate_domain(fri_domain);
        }


        vector<Polynomial> transition_arguments(2 * register_count + 1);
        transition_arguments[0] = Polynomial(vector<FieldElement>{FieldElement(0), FieldElement(1)});
        for (size_t i = 0; i != register_count; i++) {
            transition_arguments[i + 1] = trace_polynomials[i];
            transition_arguments[register_count + 1 + i] = trace_polynomials[i].scale(omicron);
        }

        cout << "Transition arguments: ";
        for (auto &point : transition_arguments) {
            cout << point << endl;
        }
        cout << endl;

        vector<Polynomial> transition_quotient(transition_constraints.size());
        vector<FieldElement> temp; // Because the last cycle is not subject to the constraint
        for (size_t i = 0; i != num_randomizors + 1; i++) {
            temp.push_back(trace_domain.back());
            trace_domain.pop_back();
        }
        Polynomial transition_constraint_zerofier = zerofier_domain(trace_domain);
        for (size_t i = 0; i != num_randomizors + 1; i++) {
            trace_domain.push_back(temp[temp.size() - 1 - i]);
        }
        cout << transition_constraints.size() << endl;
        for (size_t i = 0; i != transition_constraints.size(); i++) {
            Polynomial transition_ploynomial = transition_constraints[i].evaluate_symbolic(transition_arguments);
            cout << "Transition constraint " << i << ": " << transition_ploynomial << endl;
            transition_quotient[i] = transition_ploynomial / transition_constraint_zerofier;
        }

        // vector<uint8_t> boundary_committment = serialize_boundary_commitment(boundary_quotient_codewords);
        commit((void*)&boundary_quotient_codewords);
        
        vector<FieldElement> challenge(transition_quotient.size() + boundary_quotients.size());
        get_challenge((void*)&challenge);

        Polynomial combined_constraints = Polynomial(vector<FieldElement>{FieldElement(0)});
        for (size_t i = 0; i != transition_quotient.size(); i++) {
            cout << "Transition quotient " << i << ": " << transition_quotient[i] << endl;
            cout << "Challenge " << i << ": " << challenge[i] << endl;
            combined_constraints = combined_constraints + (transition_quotient[i] * Polynomial(vector<FieldElement>({challenge[i]})));
            cout << "Combined constraints: " << combined_constraints << endl;
        }

        std::cout << "Combined constraints: ";
        std::cout << combined_constraints << std::endl;

        std::cout << "{";
        for (size_t i = 0; i != combined_constraints.coeffs.size(); i++) {
            std::cout << "\"" << (string)combined_constraints.coeffs[i] << "\", ";
        }
        std::cout << "}" << std::endl;

        FRI::prove(
            combined_constraints.evaluate_domain(fri_domain),
            omega,
            g,
            fri_fns[0],
            fri_fns[1],
            fri_fns[2],
            fri_fns[3]
        );
    }
}

#endif