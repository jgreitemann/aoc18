import numpy as np
from itertools import combinations

inputs = np.loadtxt("input", dtype="string")

combs = combinations(inputs, 2)

for a, b in combs:
    if sum(1 for i, j in zip(a, b) if i != j) == 1:
        print(''.join([i for i,j in zip(a, b) if i==j]))

