/**
 * Generates a random SCP instance (input data set)
 * 
 * `int n`: The number of rows/elements
 * `int m`: The number of columns/sets
 * `int mx_cost`: The maximum cost assigned to a set (all costs are integers selected uniformly at random from `[1, mx_cost]`)
 * `double density`: The approximately density of cells in the matrix, or the probability that any given cell will be filled (each row is guaranteed to have at least one cell.)
 * `int seed`: The seed used for calibrating randomization
 */
SCPinstance* generateSCPinstance (int n, int m, int mx_cost, double density, int seed) {
	srand(seed);

	SCPinstance* res = new SCPinstance(n, m);

	for (int &x: res->costs) x = rand() % mx_cost + 1;

	for (int r = 0; r < n; r++) {
		for (int c = 0; c < m; c++) {
			if (rand() < density * RAND_MAX) res->rows[r].push_back(c), res->cols[c].push_back(r);
		}
		if (res->rows[r].empty()) {
			int randC = rand() % m; // Ensure the row is non-empty by selecting a random column
			res->rows[r].push_back(randC), res->cols[randC].push_back(r);
		}
	}

	return res;
}

/**
 * Writes a SCP instance to an input file
 * 
 * `SCPinstance* input`: The instance to write
 * `string inputFormat`: The format of the instance (either `rows` or `cols`, as specified for the OR-Library Data Sets)
 * `string inputPath`: The path to write the instance to
 */
void writeSCPinstance (SCPinstance* input, string inputFormat, string inputPath) {
	freopen(inputPath.c_str(), "w", stdout);
	cout << input->n << ' ' << input->m << endl;
	// Generate following the OR-lib "rows" setting such that all rows are covered
	if (inputFormat == "rows") {
		// costs
		for (int x: input->costs) cout << x << ' ';
		cout << endl;
		// N * ([# cols in row] col_1 ... col_last)
		for (vector<int> row: input->rows) {
			cout << row.size() << endl;
			for (int c: row) cout << c + 1 << ' ';
			cout << endl;
		}
	// Generate following the OR-lib "cols" setting
	} else if (inputFormat == "cols") {
		// M * ([column cost] [# rows in col] row_1 ... row_last)
		for (int c = 0; c < input->m; c++) {
			cout << input->costs[c] << ' ' << input->cols[c].size();
			for (int r: input->cols[c]) cout << ' ' << r + 1;
			cout << endl;
		}
	} else {
		cerr << "Error: Unsupported input format \"" << inputFormat << "\"" << endl;
		return;
	}
}