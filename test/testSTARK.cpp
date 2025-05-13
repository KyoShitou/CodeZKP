#include "../src/Stark.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
using std::vector;
using std::cout;
using std::endl;
using std::cin;
using std::find;
using std::transform;

enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_OPCODE,
    R_RD,
    R_SR1,
    R_IMMFLG,
    R_IMM,
    R_PC,
    MEM,
    R_COUNT
};

enum 
{
    NOP,
    ADD,
    LD,
    ST,
    HALT,
    OP_COUNT
};

void stark_commit(void *data) {
    vector<vector<FieldElement> > boundary_quotient_codewords = *static_cast<vector<vector<FieldElement> > *>(data);
    cout << "Stark commit" << endl;
    cout << "Boundary quotient codewords: ";
    // for (const auto &codeword : boundary_quotient_codewords) {
    //     cout << "[ ";
    //     for (const auto &code : codeword) {
    //         cout << code << " ";
    //     }
    //     cout << "] ";
    // }
    cout << endl;
}

void stark_challenge(void *data) {
    vector<FieldElement> *challenge = static_cast<vector<FieldElement> *>(data);
    cout << "Stark challenge, need " << challenge->size() << " field elements" << endl;
    for (size_t i = 0; i < challenge->size(); i++) {
        // int c;
        // cin >> c;
        (*challenge)[i] = FieldElement(i);
    }
    return;
}

void fri_commit(void *data) {
    vector<FieldElement> *codewords = static_cast<vector<FieldElement> *>(data);
    cout << "FRI commit" << endl;
    cout << "Codewords: ";
    for (const auto &codeword : *codewords) {
        cout << codeword << " ";
    }
    cout << endl;
}

void fri_getchallenge(void *data) {
    FieldElement *challenge = static_cast<FieldElement *>(data);
    cout << "FRI get challenge: " << endl;
    int c;
    cin >> c;
    *challenge = FieldElement(c);
    return;
}

void fri_getcolinearity_challenge(void *data) {
    size_t *index = static_cast<size_t *>(data);
    cout << "FRI get colinearity challenge: " << endl;
    cin >> *index;
    return;
}

void fri_open_merkle(void *data) {
    vector<FieldElement> codewords = *static_cast<vector<FieldElement> *>(data);
    cout << "FRI open merkle" << endl;
    vector<FieldElement> open_points_x = {codewords[0], codewords[1], codewords[2]};
    vector<FieldElement> open_points_y = {codewords[3], codewords[4], codewords[5]};

    cout << "Doing colinearity test: " << (test_colinearity(open_points_x, open_points_y)? "Accept" : "Reject") << std::endl;
    return;
}

Polynomial lagrange_selector(const FieldElement &k, vector<FieldElement> &space) {
    if (find(space.begin(), space.end(), k) == space.end()) {
        throw std::invalid_argument("k is not in the space");
    }

    Polynomial numerator(vector<FieldElement>({FieldElement(1)}));
    Polynomial denominator(vector<FieldElement>({FieldElement(1)}));
    Polynomial x(vector<FieldElement>({FieldElement(0), FieldElement(1)}));

    for (auto &point : space) {
        if (point == k) continue;
        numerator = numerator * (x - point);
        denominator = denominator * (k - point);
    }
    Polynomial result = numerator / denominator;
    return result;
}

vector<MPolynomial> transition_constraints() {
    size_t trace_width = 12;
    size_t reg_count = 5;
    vector<MPolynomial> constraints;
    vector<MPolynomial> regs = identity(2 * trace_width + 1);

    MPolynomial cycle = regs[0];
    auto [R0, R1, R2, R3, R4, OPCODE, RD, SR1, immFlg, Imm, PC, MEM] = std::tuple{regs[1], regs[2], regs[3], regs[4], regs[5], regs[6], regs[7], regs[8], regs[9], regs[10], regs[11], regs[12]};
    auto [R0n, R1n, R2n, R3n, R4n, OPCODEn, RDn, SR1n, immFlg_n, Imm_n, PCn, MEMn] = std::tuple{regs[13], regs[14], regs[15], regs[16], regs[17], regs[18], regs[19], regs[20], regs[21], regs[22], regs[23], regs[24]};

    vector<MPolynomial> this_cycle = {R0, R1, R2, R3, R4, OPCODE, RD, SR1, immFlg, Imm, PC, MEM};
    vector<MPolynomial> next_cycle = {R0n, R1n, R2n, R3n, R4n, OPCODEn, RDn, SR1n, immFlg_n, Imm_n, PCn, MEMn};

    vector<FieldElement> reg_space(reg_count);
    for (size_t i = 0; i != reg_count; i++) reg_space[i] = FieldElement(i);

    vector<FieldElement> instr_space(OP_COUNT); // 0 - HALT, 1 - ADD
    for (size_t i = 0; i != instr_space.size(); i++) instr_space[i] = FieldElement(i);

    vector<Polynomial> reg_selector(reg_count);
    for (size_t i = 0; i != reg_count; i++) reg_selector[i] = lagrange_selector(i, reg_space);

    vector<Polynomial> instr_selector(instr_space.size());
    for (size_t i = 0; i != instr_space.size(); i++) instr_selector[i] = lagrange_selector(i, instr_space);

    MPolynomial dst_reg = MPolynomial();
    MPolynomial src_reg = MPolynomial();
    MPolynomial src_reg1 = MPolynomial();
    MPolynomial src_reg2 = MPolynomial();

    for (size_t i = 0; i != reg_count; i++) {
        dst_reg = dst_reg + (MPolynomial(reg_selector[i], R_RD) * next_cycle[i]);
    }

    for (size_t i = 0; i != reg_count; i++) {
        src_reg = src_reg + (MPolynomial(reg_selector[i], R_RD) * this_cycle[i]);
    }

    for (size_t i = 0; i != reg_count; i++) {
        src_reg1 = src_reg1 + (MPolynomial(reg_selector[i], R_SR1) * this_cycle[i]);
    }

    for (size_t i = 0; i != reg_count; i++) {
        src_reg2 = src_reg2 + (MPolynomial(reg_selector[i], R_IMM) * this_cycle[i]);
    }

    MPolynomial sel_ADD = MPolynomial(instr_selector[1], R_OPCODE) * this_cycle[R_OPCODE];
    MPolynomial one = MPolynomial(1);

    // ADD semantics
    constraints.push_back(sel_ADD * immFlg * (dst_reg - src_reg1 - Imm));
    constraints.push_back(sel_ADD * (one - immFlg) * (dst_reg - src_reg1 - src_reg2));

    // LD semantics
    MPolynomial sel_LD = MPolynomial(instr_selector[2], R_OPCODE) * this_cycle[R_OPCODE];
    constraints.push_back(sel_LD * immFlg * (dst_reg - Imm));
    constraints.push_back(sel_LD * (one - immFlg) * (dst_reg - MEM));

    // ST semantics
    MPolynomial sel_ST = MPolynomial(instr_selector[3], R_OPCODE) * this_cycle[R_OPCODE];
    constraints.push_back(sel_ST * immFlg * (MEMn - Imm));
    constraints.push_back(sel_ST * (one - immFlg) * (MEMn - src_reg));

    return constraints;
}

vector<tuple<size_t, size_t, FieldElement> > boundary_constraints(vector<vector<int> > &int_trace_matrix) 
// (cycle, register, value)
{
    vector<tuple<size_t, size_t, FieldElement> > results;
    for (size_t i = 0; i != int_trace_matrix[0].size(); i++) {
        results.push_back(std::make_tuple(0, i, FieldElement(int_trace_matrix[0][i])));
    }
    for (size_t i = 0; i != int_trace_matrix[0].size(); i++) {
        results.push_back(std::make_tuple(int_trace_matrix.size() - 1, i, FieldElement(int_trace_matrix[int_trace_matrix.size() - 1][i])));
    }

    return results;
}

int main() {
    // For clear understanding, represent the trace matrix as a matrix of integers
    vector<vector<int> > int_trace_matrix = {
        {0, 0, 0, 0, 0, 1, 0, 0, 1, 10, 0, 0},
        {10, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1},
        {10, 0, 0, 0, 0, 2, 1, 0, 0, 0, 10, 2},
        {10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 10, 3}
    };

    vector<vector<FieldElement> > trace_matrix(int_trace_matrix.size(), vector<FieldElement>(int_trace_matrix[0].size()));
    for (size_t i = 0; i != int_trace_matrix.size(); i++) {
        for (size_t j = 0; j != int_trace_matrix[0].size(); j++) {
            trace_matrix[i][j] = FieldElement(int_trace_matrix[i][j]);
        }
    }

    vector<MPolynomial> transition_constraints = ::transition_constraints();
    BigInt degree = 0;
    for (auto &c : transition_constraints) {
        for (auto &p : c.dict) {
            BigInt total_deg = 0;
            for (auto &v : p.first) {
                total_deg += v;
            }
            if (total_deg > degree) {
                degree = total_deg;
            }
        }
    }
    cout << "Degree of transition constraints: " << degree << endl;
    vector<tuple<size_t, size_t, FieldElement> > boundary_constraints = ::boundary_constraints(int_trace_matrix);
    vector<void (*) (void*)> fri_fns = {fri_commit, fri_getchallenge, fri_getcolinearity_challenge, fri_open_merkle};
    STARK::prove(
        trace_matrix,
        transition_constraints,
        boundary_constraints,
        stark_commit,
        stark_challenge,
        fri_fns,
        11,
        4,
        1
    );
}