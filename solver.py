"""
The original solver program was written in Python.
I switched to C++ for efficiency and to take advantage of the stdlib algorithms
and data structures.
"""

# Program settings
FILE_NAME = "scp41.txt"
INPUT_DIRECTORY = "input/"
OUTPUT_DIRECTORY = "output/"
# rows: A list of column costs, then each row's columns (Group 1's format)
# cols: Each column's cost and rows (Group 2's format)
INPUT_FORMAT = "rows"
ALGORITHM = "greedy-naive" # greedy-naive, greedy-optimized

# Reads input file and converts it into a stream to sequentially pull ints from
with open(INPUT_DIRECTORY + FILE_NAME, "r") as f:
	intStream = [int(s) for line in f.read().split("\n") for s in line.strip().split(" ")]
	streamIndex = -1
	def nextInt ():
		global streamIndex
		streamIndex += 1
		return intStream[streamIndex]

N, M = nextInt(), nextInt() # NxM matrix (N rows/elements and M columns/sets)
costs = [] # Costs of each column
# rows[i] holds the sets containing element i
# cols[i] holds the elements contained in set i
rows, cols = [[] for r in range(N)], [[] for c in range(M)]
if INPUT_FORMAT == "rows":
	for c in range(M):
		costs.append(nextInt())
	for r in range(N):
		numContaining = nextInt()
		for i in range(numContaining):
			c = nextInt() - 1
			rows[r].append(c), cols[c].append(r)
else:
	for c in range(M):
		costs.append(nextInt())
		numContained = nextInt()
		for i in range(numContained):
			r = nextInt() - 1
			rows[r].append(c), cols[c].append(r)

exists = [False] * N # exists[i] holds whether i exists in any input set
for col in cols:
	for r in col:
		exists[r] = True
for i in range(N):
	if not exists[i]:
		print(f"No set contains element {i + 1}")
		exit()

selected = [False] * M # selected[i] holds whether set i has been selected
union = [False] * N # union[i] holds whether i is in the union of all selected sets
unionSize = 0
INF = float("inf")
unitCosts = [INF] * M
bestC = 0 # Index of the column with the lowest unit cost
totalCost = 0
if ALGORITHM == "greedy-naive":
	"""
	Unoptimized, naive greedy

	O(n∑|S|), where the summation iterates over all sets S in the problem description
	or
	O(n^2m)
	"""
	while unionSize < N:
		for c in range(M):
			numUncovered = 0
			for r in cols[c]:
				if not union[r]:
					numUncovered += 1
			unitCosts[c] = costs[c] / numUncovered if numUncovered else INF
		for c in range(M):
			if unitCosts[c] < unitCosts[bestC]:
				bestC = c
		selected[bestC] = True
		for r in cols[bestC]:
			if not union[r]:
				union[r] = True
				unionSize += 1
		totalCost += costs[bestC]
elif ALGORITHM == "greedy-optimized":
	"""
	Optimized greedy
	This algorithm uses the observation that only the new rows selected in each iteration must be updated.

	O(∑|S|), where the summation iterates over all sets S in the problem description
	or
	O(nm)
	"""
	colSizes = [len(col) for col in cols] # colSizes[i] holds the number of uncovered elements remaining in column i
	for c in range(M):
		if colSizes[c]:
			unitCosts[c] = costs[c] / colSizes[c]
		if unitCosts[c] < unitCosts[bestC]:
			bestC = c
	while unionSize < N:
		for c in range(M):
			if unitCosts[c] < unitCosts[bestC]:
				bestC = c
		selected[bestC] = True
		totalCost += costs[bestC]
		for r in cols[bestC]:
			if not union[r]:
				for c in rows[r]:
					colSizes[c] -= 1
					unitCosts[c] = costs[c] / colSizes[c] if colSizes[c] else INF
				union[r] = True
				unionSize += 1

with open(OUTPUT_DIRECTORY + "python-" + ALGORITHM + '-' + FILE_NAME, "w") as f:
	result = []
	for i in range(M):
		if selected[i]:
			result.append(str(i + 1))
	f.write(str(len(result)) + "\n")
	f.write(" ".join(result))
	print(f"Selected {len(result)} sets")
	print(f"Total cost: {totalCost}")