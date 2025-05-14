#include "../src/Stark.hpp"
#include <string>
#include <openssl/sha.h>

using std::string;

namespace WITNESS {
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

    string transcript;
    size_t fri_commit_round = 0;
    size_t fri_length = 0;
    bool fri_pass = true;

    size_t sha256_to_size_t(const std::string& data) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((const unsigned char*)data.c_str(), data.size(), hash);
    
        size_t result;
        static_assert(sizeof(size_t) <= SHA256_DIGEST_LENGTH, "size_t too large");
    
        // Use first sizeof(size_t) bytes from the hash
        std::memcpy(&result, hash, sizeof(size_t));
        
        return result;
    }
    

    std::string sha256_decimal(const std::string& data) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((const unsigned char*)data.c_str(), data.size(), hash);
    
        // Convert to decimal string: treat hash as big-endian integer
        std::vector<unsigned char> bytes(hash, hash + SHA256_DIGEST_LENGTH);
        
        // Big integer arithmetic using std::string
        std::string result = "0";
        for (unsigned char byte : bytes) {
            // Multiply current result by 256 and add byte
            int carry = byte;
            for (int i = result.size() - 1; i >= 0; --i) {
                int digit = (result[i] - '0') * 256 + carry;
                result[i] = '0' + (digit % 10);
                carry = digit / 10;
            }
            while (carry > 0) {
                result.insert(result.begin(), '0' + (carry % 10));
                carry /= 10;
            }
        }
    
        // Strip leading zeros
        result.erase(0, result.find_first_not_of('0'));
        return result.empty() ? "0" : result;
    }
    

    void stark_challenge(void *data) {
        vector<FieldElement> *challenge = static_cast<vector<FieldElement> *>(data);
        transcript += "Stark ask for " + std::to_string(challenge->size()) + " field elements\n";
        for (size_t i = 0; i < challenge->size(); i++) {
            string hash = sha256_decimal(transcript);
            challenge->at(i) = FieldElement(hash);
            transcript += "Challenge " + std::to_string(i) + ": " + (string)challenge->at(i) + "\n";
        }
    }

    void stark_commit(void *data) {
        vector<vector<FieldElement> > boundary_quotient_codewords = *static_cast<vector<vector<FieldElement> > *>(data);
        transcript += "commit boundary quotient codewords: ";
        for (const auto &codeword : boundary_quotient_codewords) {
            transcript += "[ ";
            for (const auto &code : codeword) {
                transcript += (string)code + " ";
            }
            transcript += "] ";
        }
        transcript += "\n";
    }

    void fri_commit(void *data) {
        vector<FieldElement> *codewords = static_cast<vector<FieldElement> *>(data);
        fri_length = 0;
        transcript += "FRI commit round " + std::to_string(fri_commit_round) + ": ";
        for (const auto &codeword : *codewords) {
            transcript += (string)codeword + " ";
            fri_length++;
        }
        transcript += "\n";
        fri_commit_round++;
    }

    void fri_getchallenge(void *data) {
        FieldElement *challenge = static_cast<FieldElement *>(data);
        string hash = sha256_decimal(transcript);
        transcript += "FRI get challenge, response: " + hash + "\n";
        *challenge = FieldElement(hash);
    }

    void fri_getcolinearity_challenge(void *data) {
        size_t *index = static_cast<size_t *>(data);
        string hash = sha256_decimal(transcript);
        size_t challenge = sha256_to_size_t(hash);
        challenge = challenge % (fri_length / 2);
        if (challenge == 0) {
            challenge = 1;
        }
        *index = challenge;
        transcript += "FRI get colinearity challenge, response: " + std::to_string(challenge) + "\n";
        return;
    }

    void fri_open_merkle(void *data) {
        vector<FieldElement> codewords = *static_cast<vector<FieldElement> *>(data);
        transcript += "FRI open merkle: ";
        vector<FieldElement> open_points_x = {codewords[0], codewords[1], codewords[2]};
        vector<FieldElement> open_points_y = {codewords[3], codewords[4], codewords[5]};

        transcript += "open points: ";
        for (size_t i = 0; i != open_points_x.size(); i++) {
            transcript += "(" + (string)open_points_x[i] + ", " + (string)open_points_y[i] + ") ";
        }
        transcript += "\n";
        transcript += "Doing colinearity test: ";
        if (test_colinearity(open_points_x, open_points_y)) {
            transcript += "accept\n";
        } else {
            transcript += "reject\n";
            fri_pass = false;
        }
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

    vector<MPolynomial> AIR_transition_constraints() {
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
    
    vector<tuple<size_t, size_t, FieldElement> > create_boundary_constraints(vector<vector<FieldElement> > &trace_matrix) 
    // (cycle, register, value)
    {
        vector<tuple<size_t, size_t, FieldElement> > results;
        for (size_t i = 0; i != trace_matrix[0].size(); i++) {
            results.push_back(std::make_tuple(0, i, trace_matrix[0][i]));
        }
        for (size_t i = 0; i != trace_matrix[0].size(); i++) {
            results.push_back(std::make_tuple(trace_matrix.size() - 1, i, trace_matrix[trace_matrix.size() - 1][i]));
        }
    
        return results;
    }

    string witness(
        vector<vector<FieldElement> > &trace_matrix,
        size_t transition_constraints_degree=2,
        size_t expansion_factor=4,
        size_t num_randomizors=2
    ) {
        transcript = "STARK witness started\n";
        vector<MPolynomial> transition_constraints = AIR_transition_constraints();
        vector<tuple<size_t, size_t, FieldElement> > boundary_constraints = create_boundary_constraints(trace_matrix);
        fri_commit_round = 0;
        fri_length = 0;
        fri_pass = true;
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
        if (fri_pass) {
            transcript += "STARK witness passed\n";
        } else {
            transcript += "STARK witness failed\n";
        }
        return transcript;
    }
}