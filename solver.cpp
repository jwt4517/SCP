/**
 * @file solver.cpp
 * @brief This file contains functions for reading SCP input files, solving SCP
 * instances with various algorithms, and writing SCP solutions to output files.
 */
const double kDoubleInfinity = 1e18;

/**
 * @brief Reads an SCP instance from an input file and converts it to a
 * SCPinstance object
 * 
 * @param input_path The path to the instance
 * @param input_format The format of the instance (either `rows` or `columns`,
 * as specified for the OR-Library Data Sets)
 * @return ScpInstance*
 */
ScpInstance* readScpInstance (string input_path, string input_format) {
	freopen(input_path.c_str(), "r", stdin);
	int N, M;
	cin >> N >> M;
	ScpInstance* instance = new ScpInstance(N, M);
	if (input_format == "rows") {
		for (int &x: instance->costs) cin >> x;
		for (int r = 0; r < N; r++) {
			int column_count;
			cin >> column_count;
			while (column_count--) {
				int c;
				cin >> c;
				c--;
				instance->rows[r].push_back(c);
				instance->columns[c].push_back(r);
			}
		}
	} else {
		for (int c = 0; c < M; c++) {
			cin >> instance->costs[c];
			int row_count;
			cin >> row_count;
			while (row_count--) {
				int r;
				cin >> r;
				r--;
				instance->rows[r].push_back(c);
				instance->columns[c].push_back(r);
			}
		}
	}
	return instance;
}

/**
 * @brief Solves or approximates a SCP instance using an algorithm or heuristic
 * producing a SCP solution.
 * 
 * @param input The SCP instance to solve
 * @param algorithm The algorithm to use
 * @return ScpSolution* 
 */
ScpSolution* solveScpInstance (ScpInstance* input, string algorithm) {
	int N = input->n, M = input->m;
	vector<int> costs = input->costs;
	vector<vector<int>> rows = input->rows, columns = input->columns;
	ScpSolution* solution = new ScpSolution();

	// Checks that a solution exists in O(∑|S_i|)
	vector<bool> exists(N); // exists[i] holds whether i exists in any column
	for (vector<int> column: columns) {
		for (int r: column) exists[r] = true;
	}
	for (int c = 0; c < N; c++) {
		if (!exists[c]) {
			cerr << "Error: No set contains element " << c + 1 << endl;
			return solution;
		}
	}

	auto start_time = chrono::system_clock::now(); 

	/*
	 * column_sizes[i] holds the number of uncovered elements remaining in
	 * column i
	 */
	vector<int> column_sizes; // Gets updated and used in the greedy algos
	for (vector<int> column: columns) column_sizes.push_back(column.size());
	// in_union[r] holds whether r is in the union of all selected sets
	vector<bool> in_union(N);
	int union_size = 0;
	vector<double> unit_costs(M, kDoubleInfinity);
	int best_c = 0; // Index of the best column (lowest unit cost)

	if (algorithm == "NG") {
		/**
		 * @brief Unoptimized, naive greedy
		 * 
		 * On each iteration, the algorithm runs through all elements of all
		 * sets to recompute unit costs and select the one with least unit cost.
		 * 
		 * O(n∑|S|) time, where the summation iterates over all sets S in the
		 * problem description
		 * or
		 * O(mn^2) time
		 * 
		 * O(mn) memory
		 */
		while (union_size < N) {
			for (int c = 0; c < M; c++) {
				column_sizes[c] = 0;
				for (int r: columns[c]) {
					if (!in_union[r]) column_sizes[c]++;
				}
				unit_costs[c] = column_sizes[c] ?
					double(costs[c]) / column_sizes[c] : kDoubleInfinity;
			}
			for (int c = 0; c < M; c++) {
				if (unit_costs[c] < unit_costs[best_c]) best_c = c;
			}
			solution->selected.push_back(best_c);
			for (int r: columns[best_c]) {
				if (!in_union[r]) {
					in_union[r] = true;
					union_size++;
				}
			}
			solution->total_cost += costs[best_c];
		}
	} else if (algorithm == "OG") {
		/**
		 * @brief Optimized greedy
		 * 
		 * Optimizes naive greedy using the observation that only the rows
		 * containing the new elements selected in each iteration must be
		 * updated.
		 * 
		 * O(∑|S_i|) time, where the summation iterates over all sets S in the
		 * problem description
		 * or
		 * O(mn) time
		 * 
		 * O(mn) memory
		 */
		for (int c = 0; c < M; c++) {
			if (column_sizes[c])
				unit_costs[c] = double(costs[c]) / column_sizes[c];
			if (unit_costs[c] < unit_costs[best_c]) best_c = c;
		}
		while (union_size < N) {
			for (int c = 0; c < M; c++) {
				if (unit_costs[c] < unit_costs[best_c]) best_c = c;
			}
			solution->selected.push_back(best_c);
			solution->total_cost += costs[best_c];
			for (int r: columns[best_c]) {
				// Performs updates on all newly-covered elements
				if (in_union[r]) continue;
				for (int c: rows[r]) {
					column_sizes[c]--;
					unit_costs[c] = column_sizes[c] ?
						double(costs[c]) / column_sizes[c] : kDoubleInfinity;
				}
				in_union[r] = true;
				union_size++;
			}
		}
	} else if (algorithm == "2ME") {
		/**
		 * @brief 2^m exact
		 * 
		 * Brute-forces all 2^m subfamilies of S to check if their union is U.
		 * 
		 * Requires 2^m <= LLONG_MAX, equivalent to m < 63
		 * 
		 * O(2^m * mn) time
		 * O(mn) memory
		 */
		solution->total_cost = LLONG_MAX;
		// x is a bitmask encoding the sets in the subfamily
		// The c-th bit from the end of x encodes S_c
		for (long long x = 0; x < (1LL << M); x++) {
			in_union = vector<bool>(N);
			long long next_total = 0;
			vector<int> next_selected;
			for (int c = 0; c < M; c++) {
				if ((x >> c) & 1) {
					for (int r: columns[c]) in_union[r] = true;
					next_total += costs[c];
					next_selected.push_back(c);
				}
			}
			bool ok = true;
			for (bool r: in_union) ok &= r;
			if (ok && next_total < solution->total_cost) {
				solution->total_cost = next_total;
				solution->selected = next_selected;
			}
		}
	} else if (algorithm == "2NE") {
		/**
		 * @brief 2^n exact
		 * 
		 * Use bitmask DP on the subsets of U to find the minimum total cost.
		 * 
		 * Requires 2^n <= LLONG_MAX, equivalent to n < 63
		 * 
		 * O(2^n * m) time
		 * O(2^n) mem
		 */
		// Stores minimum total costs for each subset of U by DP
		vector<long long> dp_totals(1LL << N, 1e18);
		dp_totals[0] = 0;
		// Stores the corresponding subfamilies of S used, as bitmask strings
		vector<string> dp_subfamilies(1LL << N);
		string empty_subfamily;
		for (int c = 0; c < M; c++) empty_subfamily += '0';
		for (string &x: dp_subfamilies) x = empty_subfamily;
		// x is a bitmask encoding the elements in the subset of U
		// The r-th bit from the end of x encodes element r
		for (long long x = 0; x < (1LL << N); x++) {
			for (int c = 0; c < M; c++) {
				// Set difference: (subset encoded by x) - S_c
				long long set_difference = x;
				for (int r: columns[c]) set_difference &= ~(1 << r);
				long long next_total = dp_totals[set_difference] + costs[c];
				if (next_total < dp_totals[x]) {
					dp_totals[x] = next_total;
					dp_subfamilies[x] = dp_subfamilies[set_difference];
					dp_subfamilies[x][c] = '1';
				}
			}
		}
		solution->total_cost = dp_totals.back();
		for (int c = 0; c < M; c++) {
			if (dp_subfamilies.back()[c] == '1')
				solution->selected.push_back(c);
		}
	} else {
		cerr << "Error: Unsupported algorithm \"" << algorithm << "\"" << endl;
		return solution;
	}
	
	auto end_time = chrono::system_clock::now();
	chrono::duration<double> elapsed = end_time - start_time;
	solution->runtime = elapsed.count();

	for (int &c: solution->selected) c++;
	// Sort sets for presentation
	sort(solution->selected.begin(), solution->selected.end());
	return solution;
}

/**
 * @brief Writes a SCP solution to an output file
 * 
 * @param solution The SCP solution to write
 * @param output_path The path to the output file
 */
void writeScpSolution (ScpSolution* solution, string output_path) {
	freopen(output_path.c_str(), "w", stdout);
	cout << "Number of sets: " << solution->selected.size() << endl;
	cout << "Total cost: " << solution->total_cost << endl;
	cout << "Sets selected: " << endl;
	for (int c: solution->selected) cout << c << ' ';
	cout << endl;
	cout << "Runtime: " << solution->runtime << " s" << endl;
}