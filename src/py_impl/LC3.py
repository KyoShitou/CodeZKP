from algebra import *
from univariate import *
from multivariate import *
from rescue_prime import *
from fri import *
from ip import *
from stark import *

from enum import Enum

Opcodes = {
    "NOP": FieldElement(0, Field.main()),
    "OP_ADD": FieldElement(1, Field.main()),
    "OP_HALT" : FieldElement(2, Field.main()),
    "OP_LD": FieldElement(3, Field.main()),
    "OP_ST": FieldElement(4, Field.main()),
}

# ADD R0, R1, R2
# ADD R0, R1, imm

def transition_constraint(omicrom):
    WIDTH = 12 # r0-r4 OPCODE RD SR1 immFlg Imm PC
    field = Field.main()
    vars = MPolynomial.variables(WIDTH * 2 + 1, field)
    cycle = vars[0]

    R0, R1, R2, R3, R4, OPCODE, RD, SR1, immFlg, Imm, MEM, PC = vars[1:WIDTH + 1]
    R0_, R1_, R2_, R3_, R4_, OPCODE_, RD_, SR1_, immFlg_, Imm_, MEM_, PC_ = vars[WIDTH + 1:WIDTH * 2 + 1]

    this_cycle = [R0, R1, R2, R3, R4, OPCODE, RD, SR1, immFlg, Imm, PC]
    next_cycle = [R0_, R1_, R2_, R3_, R4_, OPCODE_, RD_, SR1_, immFlg_, Imm_, PC_]
    
    # Lagrange selector
    def Lagrange_selector_REG(k):
        numerator = Polynomial([field.one()])
        denominator = Polynomial([field.one()])
        x = Polynomial([field.zero(), field.one()])
        for i in range(5):
            if i == k:
                continue
            numerator = numerator * (x - FieldElement(i, field))
            denominator = denominator * FieldElement(k - i, field)
        selector = numerator / denominator
        return selector
    
    def Lagrange_selector_OP(k):
        numerator = Polynomial([field.one()])
        denominator = Polynomial([field.one()])
        x = Polynomial([field.zero(), field.one()])
        for i in range(5):
            if i == k:
                continue
            numerator = numerator * (x - FieldElement(i, field))
            denominator = denominator * FieldElement(k - i, field)
        selector = numerator / denominator
        return selector
    
    selector_reg = [Lagrange_selector_REG(i) for i in range(5)]
    selector_op = [Lagrange_selector_OP(i) for i in range(5)]
    # AIR constraint for ADD

    air = []
    
    dst_reg = MPolynomial({})
    for i in range(5):
        dst_reg = dst_reg + (MPolynomial.lift(selector_reg[i], 7) * next_cycle[i])
    src_reg1 = MPolynomial({})
    for i in range(5):
        src_reg1 = src_reg1 + (MPolynomial.lift(selector_reg[i], 8) * this_cycle[i])

    src_reg2 = MPolynomial({})
    for i in range(5):
        src_reg2 = src_reg2 + (MPolynomial.lift(selector_reg[i], 10) * this_cycle[i])


# Semantics for ADD
    one     = MPolynomial({(0,0): field.one()})
    sel_ADD = MPolynomial.lift(selector_op[1], 6)
    print(selector_op[1].evaluate(field.zero()))
    air.append(sel_ADD * immFlg * (dst_reg - src_reg1 - Imm))
    air.append(sel_ADD * (one - immFlg) * (dst_reg - src_reg1 - src_reg2) )

# Semantics for LD
    sel_LD = MPolynomial.lift(selector_op[3], 6)
    air.append(sel_LD * (dst_reg - MEM))

# Semantics for ST
    sel_ST = MPolynomial.lift(selector_op[4], 6)
    air.append(sel_ST * (MEM_ - dst_reg))


    # air.append( immFlg * (one - immFlg))
    zerofier_opcodes = Polynomial.zerofier_domain([FieldElement(i, field) for i in range(5)])
    air.append( MPolynomial.lift(zerofier_opcodes, 6))

    return air

# def boundary_constraints(): # Hard coded for now
#     f = Field.main()
#     boundary = []
#     boundary += [(0, i, FieldElement(j, f)) for i, j in [(0, 0), (1, 1), (2, 2), (3, 0), (4, 0), (5, 1), (6, 0)]]
#     boundary += [(3, i, FieldElement(j, f)) for i, j in [(0, 3), (1, 1), (2, 2), (3, 0), (4, 0), (5, 1), (6, 3)]]

#     return boundary

def new_boundary_constraints(int_trace):
    f = Field.main()
    boundary_constraints = []
    boundary_constraints += [(0, i, FieldElement(j, f)) for i, j in enumerate(int_trace[0])]
    boundary_constraints += [(len(int_trace) - 1, i, FieldElement(j, f)) for i, j in enumerate(int_trace[-1])]
    return boundary_constraints


def test():
    field = Field.main()
    expansion_factor = 4
    num_colinearity_checks = 2
    security_level = 2
    num_cycles = 4
    state_width = 12

    stark = Stark(field, expansion_factor, num_colinearity_checks, security_level, state_width, num_cycles, transition_constraints_degree=12)

    # prove honestly
    print("honest proof generation ...")

    int_trace = [
        [0, 0, 0, 0, 0] + [1, 0, 0, 1, 10] + [0] + [0], # ADD R0, R1, 10
        [10, 0, 0, 0, 0] + [4, 0, 0, 0, 0] + [0] + [1], # ST R0
        [10, 0, 0, 0, 0] +  [3, 1, 0, 0, 0] + [10] + [2], # LD R1
        [10, 10, 0, 0, 0] +  [0, 0, 0, 0, 0] + [10] + [3], # HALT
    ]

    # prove
    # trace = [[FieldElement(i, field) for i in [0, 1, 2, 0, 0]] + [Opcodes["OP_ADD"]] + [FieldElement(0, field)]] + \
    #         [[FieldElement(i, field) for i in [3, 1, 2, 0, 0]] + [Opcodes["OP_ADD"]] + [FieldElement(1, field)]] + \
    #         [[FieldElement(i, field) for i in [3, 1, 2, 0, 0]] + [Opcodes["OP_ADD"]] + [FieldElement(2, field)]] + \
    #         [[FieldElement(i, field) for i in [3, 1, 2, 0, 0]] + [Opcodes["OP_ADD"]] + [FieldElement(3, field)]]

    trace = [[FieldElement(i, field) for i in row] for row in int_trace]
    air = transition_constraint(stark.omicron)
    boundary = new_boundary_constraints(int_trace)
    proof = stark.prove(trace, air, boundary)

    # verify
    verdict = stark.verify(proof, air, boundary)

    assert(verdict == True), "valid stark proof fails to verify"
    print("success \\o/")

    print("verifying false claim ...")
    # verify false claim
    boundary_ = new_boundary_constraints(int_trace)
    boundary_[0] = (0, 0, FieldElement(1, field))
    verdict = stark.verify(proof, air, boundary_)

    assert(verdict == False), "invalid stark proof verifies"
    print("proof rejected! \\o/")

test()