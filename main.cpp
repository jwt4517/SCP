/**
 * @file main.cpp
 * @brief This is the entry file for the project. Compiling and running it
 * collects data at a variety of settings.
 */
#include "header.cpp"
#include "generator.cpp"
#include "solver.cpp"

const string kInputDirectory = "input/";
const string kOutputDirectory = "output/";

/**
 * rows: A list of column costs, then each row's columns (OR-Library Group 1's
 * format)
 * columns: Each column's cost and rows (OR-Library Group 2's format)
 */
const string kInputFormat = "columns";

/**
 * Algorithms list
 * +------+------------------+
 * | Code | Name             |
 * +------+------------------+
 * | NG   | Naive greedy     |
 * | OG   | Optimized greedy |
 * | 2ME  | 2^M exact        |
 * | 2NE  | 2^N exact        |
 * +------+------------------+
 */

/**
 * Bounds
 * 
 * N = |S_i|
 * M = |S|
 * INT_MAX = 2^31 - 1
 * LL_MAX = 2^63 - 1
 * 
 * 0 <= N, M <= INT_MAX
 * 0 <= w_i <= INT_MAX
 * ∑w_i << LL_MAX ("<<" for "much less than")
 * 
 * 2^m exact:
 * 		2^M <= INT_MAX
 * 2^n exact:
 * 		2^N <= INT_MAX
 * 
 * Lastly, time and memory complexities must be feasible.
 */

// Note: Zero-indexing used internally, 1-indexing used for I/O
const vector<pair<int, int>> kSizes = {
	{20, 1000},
	{1000, 20},
	{10000, 10000}
};
const vector<double> kDensities = {
	0.01,
	0.05,
	0.1,
	0.2,
	0.5,
	0.9
};
const int kMaxCost = 1000;
const int kTrialsPerLevel = 10; // Trials per level
const int kTotalTrials = kSizes.size() * kDensities.size() * kTrialsPerLevel;
const bool kWriteInput = false;

int main () {
	cerr << "Running SCP" << endl;
	cerr << endl;
	cerr << "Sizes (" << kSizes.size() << "): ";
	for (pair<int, int> size: kSizes)
		cerr << size.first << 'x' << size.second << ' ';
	cerr << endl;
	cerr << "Densities (" << kDensities.size() << "): ";
	for (double density: kDensities) cerr << density << ' ';
	cerr << endl;
	cerr << "Max column cost: " << kMaxCost << endl;
	cerr << "Trials per level: " << kTrialsPerLevel << endl;
	cerr << endl;
	cerr << "Input format: " << kInputFormat << endl;
	cerr << "Input directory: " << kInputDirectory << endl;
	cerr << "Output directory: " << kOutputDirectory << endl;
	cerr << "Input writing is " << (kWriteInput ? "on" : "off") << endl;
	cerr << endl;

	// Runs and times all trials
	auto start_time = chrono::system_clock::now();
	int trial_count = 0;
	for (int i = 0; i < kSizes.size(); i++) {
		pair<int, int> size = kSizes[i];
		int n = size.first, m = size.second;
		const string kSizeString = to_string(n) + 'x' + to_string(m);
		cerr << '[' << i + 1 << '/' << kSizes.size() << "] " << kSizeString <<
			endl;
		// Finds the algorithms that are feasible to run on this matrix size
		vector<string> algorithms = {
			"NG",
			"OG"
		};
		if (m <= 20) algorithms.push_back("2ME");
		if (n <= 20) algorithms.push_back("2NE");
		// Reverses order to enable computation of approximation ratios
		reverse(algorithms.begin(), algorithms.end());
		// `size_averages` stores all average stats for this size
		map<string, vector<map<string, double>>> size_averages;
		for (string algorithm: algorithms) size_averages[algorithm] = {};
		for (double density: kDensities) {
			map<string, AlgorithmDataCollection*> data_collections;
			for (string algorithm: algorithms)
				data_collections[algorithm] = new AlgorithmDataCollection();
			for (int trial = 1; trial <= kTrialsPerLevel;
				trial++, trial_count++) {
				cerr << "\t[" << trial_count + 1 << '/' << kTotalTrials << "]";
				cerr << ' ' << kSizeString;
				cerr << ", " << density << " density";
				cerr << ", repetition #" << trial << '/' << kTrialsPerLevel <<
					endl;
				string kDataSetName = "rand-" + kInputFormat + '-' +
					kSizeString +
					"-MC" + to_string(kMaxCost) +
					"-D" + doubleToString(density) +
					"-S" + to_string(trial);
				cerr << "\t\tGenerating SCP instance " << kDataSetName <<
					"..." << endl;
				ScpInstance* input = generateScpInstance(
					n,
					m,
					kMaxCost,
					density,
					trial
				);
				if (kWriteInput) {
					cerr << "\t\tWriting input file..." << endl;
					writeScpInstance(input, kInputFormat, kInputDirectory +
						kDataSetName + ".txt");
				}
				// -1 if there is no feasible exact algorithm
				long long exact_total = -1;
				for (string algorithm: algorithms) {
					cerr << "\t\tRunning " << algorithm << "...";
					ScpSolution* solution = solveScpInstance(input, algorithm);
					if (algorithm.back() == 'E')
						exact_total = solution->total_cost;
					data_collections[algorithm]->
						recordTrial(solution, exact_total);
					writeScpSolution(solution, kOutputDirectory +
						kDataSetName + '-' + algorithm + ".txt");
					cerr << " (" << solution->runtime << " s)" << endl;
				}
			}
			for (string algorithm: algorithms) {
				size_averages[algorithm].push_back(
					data_collections[algorithm]->getAverages()
				);
			}
		}
		for (string stat: kAlgorithmStats) {
			if (stat == "AR" && algorithms.front().back() != 'E') continue;
			string output_file_name = kSizeString + "-MC" +
				to_string(kMaxCost) + "-D";
			for (double density: kDensities)
				output_file_name += doubleToString(density) + '-';
			output_file_name += "T" + to_string(kTrialsPerLevel);
			for (string algorithm: algorithms)
				output_file_name += '-' + algorithm;
			output_file_name += '-' + stat + ".txt";
			// make a file for each stat for each size
			cerr << "\tWriting statistics file " << output_file_name <<
				"..." << endl;
			freopen((kOutputDirectory + output_file_name).c_str(), "w", stdout);
			cout << kTrialsPerLevel << "-trial averages" << endl;
			vector<vector<string>> table(
				algorithms.size() + 1, vector<string>(kDensities.size() + 1)
			);
			for (int i = 0; i < kDensities.size(); i++)
				table[0][i + 1] = kDensities[i];
			for (int r = 0; r < algorithms.size() + 1; r++) {
				for (int c = 0; c < kDensities.size() + 1; c++) {
					if (r == 0) {
						if (c == 0) cout << "Algorithm";
						else cout << kDensities[c - 1];
					} else {
						string algorithm = algorithms[r - 1];
						if (c == 0) cout << algorithm;
						else cout << size_averages[algorithm][c - 1][stat];
					}
					cout << '\t';
				}
				cout << endl;
			}
		}
	}
	auto end_time = chrono::system_clock::now();
	chrono::duration<double> elapsed = end_time - start_time;
	double total_runtime = elapsed.count();
	int minutes = total_runtime / 60;
	double seconds = total_runtime - 60 * minutes;
	cerr << "Completed " << kTotalTrials << " trials in " << minutes << ':' <<
		seconds << endl;
}