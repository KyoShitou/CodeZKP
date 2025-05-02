from algebra import *
from univariate import *
from multivariate import *
from rescue_prime import *
from fri import *
from ip import *
from stark import *

from enum import Enum

Opcodes = {
    "OP_BR": FieldElement(0, Field.main()),
    "OP_ADD": FieldElement(1, Field.main()),
}

Opcode_lst = [Opcodes["OP_BR"], Opcodes["OP_ADD"]]

def transition_constraint(omicrom):
    WIDTH = 7 # r0-r4 OPCODE PC
    field = Field.main()
    vars = MPolynomial.variables(WIDTH * 2 + 1, field)
    cycle = vars[0]
    R0, R1, R2, R3, R4, OPCODE, PC = vars[1:WIDTH + 1]
    R0_, R1_, R2_, R3_, R4_, OPCODE_, PC_ = vars[WIDTH + 1:WIDTH * 2 + 1]
    
    # Lagrange selector
    print(type(OPCODE))
    Z_s = (OPCODE - Opcodes["OP_BR"]) * (OPCODE - Opcodes["OP_ADD"])
    air = [Z_s]
    print("Z_s", Z_s.dictionary)
    print("OPCODE", OPCODE.dictionary)
    def Lagrange_selector(k):
        numerator = Polynomial([field.one()])
        denominator = Polynomial([field.one()])
        x = Polynomial([field.zero(), field.one()])
        for i in range(len(Opcode_lst)):
            if i == k:
                continue
            numerator = numerator * (x - Opcode_lst[i])
            denominator = denominator * (Opcode_lst[k] - Opcode_lst[i])
        selector = numerator / denominator
        return MPolynomial.lift(selector, 6)

    # Lagrange selector for opcode
    Lagrange_selectors = [Lagrange_selector(i) for i in range(len(Opcode_lst))]

    print("Lagrange_selectors", Lagrange_selectors)
    # AIR constraint for ADD (Now assuming ADD R0, R1, R2)
    AIR_ADD = Lagrange_selectors[Opcodes["OP_ADD"].value] * (R0_ - R1 - R2)
    air.append(AIR_ADD)
    for i in range(1, 5):
        air.append(Lagrange_selectors[Opcodes["OP_ADD"].value] * (vars[i + 1] - vars[i + WIDTH + 1]))
    
    return air

def boundary_constraints(): # Hard coded for now
    f = Field.main()
    boundary = []
    boundary += [(0, i, FieldElement(j, f)) for i, j in [(0, 0), (1, 1), (2, 2), (3, 0), (4, 0), (5, 1), (6, 0)]]
    boundary += [(2, i, FieldElement(j, f)) for i, j in [(0, 3), (1, 1), (2, 2), (3, 0), (4, 0), (5, 1), (6, 2)]]

    return boundary

def test():
    field = Field.main()
    expansion_factor = 4
    num_colinearity_checks = 2
    security_level = 2
    input_element = [FieldElement(i, field) for i in [0, 1, 2, 0, 0]] + [Opcodes["OP_ADD"]] + [FieldElement(0, field)]
    print("running trial with input:", input_element)
    num_cycles = 4
    state_width = 7

    stark = Stark(field, expansion_factor, num_colinearity_checks, security_level, state_width, num_cycles)

    # prove honestly
    print("honest proof generation ...")

    # prove
    trace = [[FieldElement(i, field) for i in [0, 1, 2, 0, 0]] + [Opcodes["OP_ADD"]] + [FieldElement(0, field)]] + \
            [[FieldElement(i, field) for i in [3, 1, 2, 0, 0]] + [Opcodes["OP_ADD"]] + [FieldElement(1, field)]] + \
            [[FieldElement(i, field) for i in [3, 1, 2, 0, 0]] + [Opcodes["OP_ADD"]] + [FieldElement(2, field)]] + \
            [[FieldElement(i, field) for i in [3, 1, 2, 0, 0]] + [Opcodes["OP_ADD"]] + [FieldElement(3, field)]]
    air = transition_constraint(stark.omicron)
    boundary = boundary_constraints()
    proof = stark.prove(trace, air, boundary)

    # verify
    verdict = stark.verify(proof, air, boundary)

    assert(verdict == True), "valid stark proof fails to verify"
    print("success \\o/")

    print("verifying false claim ...")
    # verify false claim
    boundary_ = boundary_constraints()
    boundary_[0] = (0, 0, FieldElement(1, field))
    verdict = stark.verify(proof, air, boundary_)

    assert(verdict == False), "invalid stark proof verifies"
    print("proof rejected! \\o/")

test()