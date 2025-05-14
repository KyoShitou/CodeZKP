import sys

def determine_register(str):
    if str[0] != 'R':
        return False
    if int(str[1:]) < 0 or int(str[1:]) >= 5:
        raise ValueError("Invalid register number: " + str[1:])
    return True

def simulate(filename):
    R0 = 0
    R1 = 1
    R2 = 2
    R3 = 3
    R4 = 4
    OPCODE = 5
    ARG1 = 6
    ARG2 = 7
    immFLAG = 8
    ARG3 = 9
    PC = 10
    MEM = 11
    trace = [[0 for i in range(12)]]
    file = open(filename, "r")
    
    t = 0
    for line in file:
        line = line.replace(",", " ")
        line = line.split()
        trace.append([0 for i in range(12)])
        if (line[0] == "ADD"):
            trace[t][OPCODE] = 1
            if (determine_register(line[1])):
                trace[t][ARG1] = int(line[1][1:])
            else:
                raise ValueError("Invalid register number: " + line[1][1:])

            if (determine_register(line[2])):
                trace[t][ARG2] = int(line[2][1:])
            else:
                raise ValueError("Invalid register number: " + line[2][1:])
            
            if (determine_register(line[3])):
                trace[t][immFLAG] = 0
                trace[t][ARG3] = int(line[3][1:])
                trace[t + 1][int(line[1][1:])] = trace[t][int(line[2][1:])] + trace[t][int(line[3][1:])]
            else:
                trace[t][immFLAG] = 1
                trace[t][ARG3] = int(line[3])
                trace[t + 1][int(line[1][1:])] = trace[t][int(line[2][1:])] + int(line[3])

        elif (line[0] == "LD"):
            trace[t][OPCODE] = 2
            if (determine_register(line[1])):
                trace[t][ARG1] = int(line[1][1:])
            else:
                raise ValueError("Invalid register number: " + line[1][1:])
            if (len(line) == 2):
                trace[t][immFLAG] = 0
                trace[t + 1][ARG2] = trace[t][MEM]
            elif (len(line) == 3):
                trace[t][immFLAG] = 1
                trace[t][ARG3] = int(line[2])
                trace[t + 1][ARG2] = int(line[2])
            else:
                raise ValueError("Invalid number of arguments for LD instruction")
        
        elif (line[0] == "ST"):
            trace[t][OPCODE] = 3
            if (determine_register(line[1])):
                trace[t][ARG1] = int(line[1][1:])
                trace[t][immFLAG] = 0
                trace[t + 1][MEM] = trace[t][ARG1]
            else:
                trace[t][immFLAG] = 1
                trace[t][ARG3] = int(line[1])
                trace[t + 1][MEM] = int(line[1])

        else:
            raise ValueError("Invalid instruction: " + line[0])
        t += 1

    file.close()

    for l in trace:
        for i in l:
            print(i, end=" ")
        print()
    

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 interpreter.py <filename>")
        sys.exit(1)

    arg = sys.argv[1]
    simulate(arg)            
