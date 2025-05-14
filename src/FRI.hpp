#ifndef FRI_HPP
#define FRI_HPP

#include "Field.hpp"
#include "Polynomial.hpp"
#include "merklecpp.h"
namespace FRI {
    class FRI {

    };
    size_t expansion_factor = 4;
    size_t num_colinearity_checks = 1;

    size_t num_rounds(size_t domain_length) {
        size_t rounds = 0;
        while (domain_length > expansion_factor && 2 * num_colinearity_checks < domain_length) {
            domain_length = domain_length / 2;
            rounds++;
        }
        return rounds;
    }

    void prove(const vector<FieldElement> &codeword, 
        const FieldElement &_omega,
        const FieldElement &_offset,
        void (*commit)(void*), 
        void (*get_challenge)(void*), 
        void (*get_colinearity_challenge)(void*),
        void (*open_merkle)(void*)) {
            size_t domain_length = codeword.size();
            size_t rounds = num_rounds(domain_length);
            vector<FieldElement> cur_codeword = codeword;
            vector<FieldElement> folded_codeword;

            FieldElement omega = _omega;
            FieldElement offset = _offset;
            // Without use of merkle tree, temporary workaround
            for (size_t i = 0; i != rounds; i++) {
                commit((void*)&cur_codeword);
                FieldElement challenge;
                get_challenge((void*)&challenge);
                for (size_t j = 0; j != cur_codeword.size() / 2; j++) {
                    folded_codeword.push_back(
                        FieldElement(2).inv() * (
                            (FieldElement(1) + challenge / (offset * (omega^j))) * cur_codeword[j] + 
                            (FieldElement(1) - challenge / (offset * (omega^j))) * cur_codeword[j + domain_length / 2]
                        )
                    );
                }
                commit((void*)&folded_codeword);
                for (size_t j = 0; j != num_colinearity_checks; j++) {
                    size_t index;
                    get_colinearity_challenge((void*)&index);
                    assert(index != 0);
                    vector<FieldElement> colinearity_points_x;
                    vector<FieldElement> colinearity_points_y;
                    colinearity_points_x.push_back((offset * (omega^index)));
                    colinearity_points_y.push_back(cur_codeword[index]);

                    colinearity_points_x.push_back((offset * (omega^(index + domain_length / 2))));
                    colinearity_points_y.push_back(cur_codeword[index + domain_length / 2]);

                    colinearity_points_x.push_back((challenge));
                    colinearity_points_y.push_back(folded_codeword[index]);
                    
                    vector<FieldElement> open(6, FieldElement(0));
                    for (size_t k = 0; k != colinearity_points_x.size(); k++) {
                        open[k] = colinearity_points_x[k];
                        open[k + 3] = colinearity_points_y[k];
                    }
                    open_merkle((void*)&open); // Placeholder, now the verifier could check it themselves
                }

                cur_codeword = folded_codeword;
                folded_codeword.clear();
                domain_length = domain_length / 2;
                offset = (offset^2);
                omega = omega * omega;

                vector<FieldElement> new_domain;
                for (size_t j = 0; j != cur_codeword.size(); j++) {
                    new_domain.push_back(offset * omega^j);
                }
            }
        }
}

#endif