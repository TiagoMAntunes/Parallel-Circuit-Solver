import sys, glob
import random
if len(sys.argv) != 4:
    print("Usage: python3 test-generator.py <path to possible inputs> <number of lines> <filename to save>")
    sys.exit(0)

inputs = glob.glob(sys.argv[1] + "/random-*.txt")
lines = sys.argv[2]
outputs = []
lineCount = int(lines)
while lineCount > 0:
    lineCount -= 1
    outputs.append("run " + inputs[random.randint(0, len(inputs)-1)])

filename = sys.argv[3]
f = open(filename, 'w+')
for line in outputs:
    f.write(line + "\n")
f.close()

print("Done!")
