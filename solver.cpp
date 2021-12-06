const double INF_DOUBLE = 1e18;

/**
 * Reads an SCP instance in format `inputFormat` from `inputPath`
 * 
 * `string inputPath`: The path to the instance
 * `string inputFormat`: The format of the instance (either `rows` or `cols`, as specified for the OR-Library Data Sets)
 */
SCPinstance* readSCPinstance (string inputPath, string inputFormat) {
	freopen(inputPath.c_str(), "r", stdin);
	int N, M;
	cin >> N >> M;
	SCPinstance* res = new SCPinstance(N, M);
	if (inputFormat == "rows") {
		for (int &x: res->costs) cin >> x;
		for (int r = 0; r < N; r++) {
			int numCols;
			cin >> numCols;
			while (numCols--) {
				int c;
				cin >> c;
				c--;
				res->rows[r].push_back(c), res->cols[c].push_back(r);
			}
		}
	} else {
		for (int c = 0; c < M; c++) {
			cin >> res->costs[c];
			int numRows;
			cin >> numRows;
			while (numRows--) {
				int r;
				cin >> r;
				r--;
				res->rows[r].push_back(c), res->cols[c].push_back(r);
			}
		}
	}
	return res;
}

/**
 * Solves or approximates a SCP instance using an algorithm or heuristic, producing a SCP solution.
 * 
 * `SCPinstance* input`: The SCP instance to solve
 * `string algo`: The algorithm to use
 */
SCPsolution* solveSCPinstance (SCPinstance* input, string algo) {
	int N = input->n, M = input->m;
	vector<int> costs = input->costs;
	vector<vector<int>> rows = input->rows, cols = input->cols;
	SCPsolution* res = new SCPsolution();

	// Determine if a solution exists (O(∑|S_i|))
	vector<bool> exists(N); // exists[i] holds whether i exists in any input set
	for (vector<int> col: cols) {
		for (int r: col) exists[r] = true;
	}
	for (int c = 0; c < N; c++) {
		if (!exists[c]) {
			cerr << "Error: No set contains element " << c + 1 << endl;
			return res;
		}
	}

	auto startTime = chrono::system_clock::now(); 

	// colSizes[i] holds the number of uncovered elements remaining in column i
	vector<int> colSizes; // Gets updated and used in the greedy algos
	for (vector<int> col: cols) colSizes.push_back(col.size());
	// selected is the subfamily of sets selected for the answer
	vector<bool> inUnion(N); // inUnion[r] holds whether r is in the union of all selected sets
	int unionSize = 0;
	vector<double> unitCosts(M, INF_DOUBLE);
	int bestC = 0; // Index of the column with the lowest unit cost

	if (algo == "NG") {
		/*
		Unoptimized, naive greedy

		On each iteration, run through all elements of all sets to recompute unit costs and select the one with least unit cost
		
		O(n∑|S|) time, where the summation iterates over all sets S in the problem description
		or
		O(n^2m) time

		O(nm) mem
		*/

		while (unionSize < N) {
			for (int c = 0; c < M; c++) {
				colSizes[c] = 0;
				for (int r: cols[c]) {
					if (!inUnion[r]) colSizes[c]++;
				}
				unitCosts[c] = colSizes[c] ? double(costs[c]) / colSizes[c] : INF_DOUBLE;
			}
			for (int c = 0; c < M; c++) {
				if (unitCosts[c] < unitCosts[bestC]) bestC = c;
			}
			res->selected.push_back(bestC);
			for (int r: cols[bestC]) {
				if (!inUnion[r]) {
					inUnion[r] = true;
					unionSize++;
				}
			}
			res->totalCost += costs[bestC];
		}
	} else if (algo == "OG") {
		/*
		Optimized greedy

		Optimize naive greedy using the observation that only the rows containing the new elements selected in each
		iteration must be updated.

		O(∑|S|) time, where the summation iterates over all sets S in the problem description
		or
		O(mn) time

		O(mn) mem
		*/
		for (int c = 0; c < M; c++) {
			if (colSizes[c]) unitCosts[c] = double(costs[c]) / colSizes[c];
			if (unitCosts[c] < unitCosts[bestC]) bestC = c;
		}
		while (unionSize < N) {
			for (int c = 0; c < M; c++) {
				if (unitCosts[c] < unitCosts[bestC]) bestC = c;
			}
			res->selected.push_back(bestC);
			res->totalCost += costs[bestC];
			for (int r: cols[bestC]) {
				if (!inUnion[r]) {
					for (int c: rows[r]) {
						colSizes[c]--;
						unitCosts[c] = colSizes[c] ? double(costs[c]) / colSizes[c] : INF_DOUBLE;
					}
					inUnion[r] = true;
					unionSize++;
				}
			}
		}
	} else if (algo == "2M") {
		/*
			2^m exact
			
			Brute-force all 2^m subfamilies of S to check if their union equals U.

			Requires 2^m <= LLONG_MAX, equivalent to m < 63

			O(2^m * mn) time
			O(mn) mem
		*/
		res->totalCost = LLONG_MAX;
		// x is a bitmask encoding the sets in the subfamily
		// The c-th bit from the end of x encodes S_c
		for (long long x = 0; x < (1LL << M); x++) {
			inUnion = vector<bool>(N);
			long long nxtTotal = 0;
			vector<int> nxtSelected;
			for (int c = 0; c < M; c++) {
				if ((x >> c) & 1) {
					for (int r: cols[c]) inUnion[r] = true;
					nxtTotal += costs[c];
					nxtSelected.push_back(c);
				}
			}
			bool ok = true;
			for (bool r: inUnion) ok &= r;
			if (ok && nxtTotal < res->totalCost) {
				res->totalCost = nxtTotal;
				res->selected = nxtSelected;
			}
		}
	} else if (algo == "2N") {
		/*
			2^n exact

			Use bitmask DP on the subsets of U to find the minimum total cost.

			Requires 2^n <= LLONG_MAX, equivalent to n < 63
		
			O(2^n * m) time
			O(2^n) mem
		*/
		vector<long long> dpTotal(1LL << N, 1e18); // Compute minimum total costs for each subset of U by DP
		dpTotal[0] = 0;
		vector<string> dpSubf(1LL << N); // Also store the corresponding subfamilies of S used, as bitmasks
		string emptySubf;
		for (int c = 0; c < M; c++) emptySubf += '0';
		for (string &x: dpSubf) x = emptySubf;
		// x is a bitmask encoding the elements in the subset of U
		// The r-th bit from the end of x encodes element r
		for (long long x = 0; x < (1LL << N); x++) {
			for (int c = 0; c < M; c++) {
				long long setDiff = x; // Set difference: (subset encoded by x) - S_c
				for (int r: cols[c]) setDiff &= ~(1 << r);
				long long nxtTotal = dpTotal[setDiff] + costs[c];
				if (nxtTotal < dpTotal[x]) {
					dpTotal[x] = nxtTotal;
					dpSubf[x] = dpSubf[setDiff];
					dpSubf[x][c] = '1';
				}
			}
		}
		long long lastIdx = (1LL << N) - 1;
		res->totalCost = dpTotal[lastIdx];
		for (int c = 0; c < M; c++) {
			if (dpSubf[lastIdx][c] == '1') res->selected.push_back(c);
		}
	} else {
		cerr << "Error: Unsupported algorithm \"" << algo << "\"" << endl;
		return res;
	}
	
	auto endTime = chrono::system_clock::now();
	chrono::duration<double> elapsed = endTime - startTime;
	res->runtime = elapsed.count();

	for (int &c: res->selected) c++;
	sort(res->selected.begin(), res->selected.end()); // Sort sets for presentation
	return res;
}

/**
 * Writes a SCP solution to an output file
 * 
 * `SCPsolution* output`: The SCP solution to write
 * `string outputPath`: The path to the output file
 */
void writeSCPsolution (SCPsolution* output, string outputPath) {
	freopen(outputPath.c_str(), "w", stdout);
	cout << "Number of sets: " << output->selected.size() << endl;
	cout << "Total cost: " << output->totalCost << endl;
	for (int c: output->selected) cout << c << ' ';
	cout << endl;
	cout << output->runtime << " s" << endl;
}