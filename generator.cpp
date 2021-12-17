/**
 * @file generator.cpp
 * @brief This file contains functions for generating random SCP instances with
 * various settings and writing them to input files.
 */

/**
 * @brief Generates a random SCP instance (input data set).
 * 
 * @param n The number of rows/elements
 * @param m The number of columns/sets
 * @param max_cost The maximum cost assigned to a set (all costs are integers
 * selected uniformly at random from `[1, max_cost]`)
 * @param density The approximate density of cells in the matrix, or the
 * probability that any given cell will be filled (each row is guaranteed to
 * have at least one cell.)
 * @param seed The seed used for calibrating randomization
 * @return ScpInstance* 
 */
unique_ptr<ScpInstance> generateScpInstance (
	int n, int m, int max_cost, double density, int seed
) {
	srand(seed);

	unique_ptr<ScpInstance> instance(new ScpInstance(n, m));

	for (int &x: instance->costs) x = rand() % max_cost + 1;

	for (int r = 0; r < n; r++) {
		for (int c = 0; c < m; c++) {
			if (rand() < density * RAND_MAX) {
				instance->rows[r].push_back(c);
				instance->columns[c].push_back(r);
			}
		}
		if (instance->rows[r].empty()) {
			// Ensure the row is non-empty by selecting a random column
			int random_c = rand() % m;
			instance->rows[r].push_back(random_c);
			instance->columns[random_c].push_back(r);
		}
	}

	return instance;
}

/**
 * @brief Writes a SCP instance to an input file
 * 
 * @param input The instance to write
 * @param input_format The format of the instance (either `rows` or `columns`,
 * as specified for the OR-Library Data Sets)
 * @param input_path The path to write the instance to
 */
void writeScpInstance (
	unique_ptr<ScpInstance> &input, string input_format, string input_path
) {
	ofstream fout;
	fout.open(input_path);
	fout << input->n << ' ' << input->m << '\n';
	// Generate with OR-Library "rows" setting such that all rows are covered
	if (input_format == "rows") {
		// costs
		for (int x: input->costs) fout << x << ' ';
		fout << '\n';
		// N * ([# of columns in row] column_1 ... column_last)
		for (vector<int> row: input->rows) {
			fout << row.size() << '\n';
			for (int c: row) fout << c + 1 << ' ';
			fout << '\n';
		}
	// Generate with OR-Library "columns" setting such that all rows are covered
	} else if (input_format == "columns") {
		// M * ([column cost] [# rows in column] row_1 ... row_last)
		for (int c = 0; c < input->m; c++) {
			fout << input->costs[c] << ' ' << input->columns[c].size();
			for (int r: input->columns[c]) fout << ' ' << r + 1;
			fout << '\n';
		}
	} else {
		fout << "Error: Unsupported input format \"" << input_format << "\"\n";
		fout.close();
		return;
	}
	fout.close();
}