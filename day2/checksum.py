import numpy as np
from collections import Counter

inputs = np.loadtxt("input", dtype="object")

threes = 0
twos = 0

for string in inputs:
    c = Counter(str(string))
    if any(c[key] == 2 for key in c):
        twos += 1
    if any(c[key] == 3 for key in c):
        threes += 1

print(twos*threes)
