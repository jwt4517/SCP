#include "header.cpp"
#include "generator.cpp"
#include "solver.cpp"

const string INPUT_DIRECTORY = "input/";
const string OUTPUT_DIRECTORY = "output/";

/**
 * rows: A list of column costs, then each row's columns (OR-Library Group 1's format)
 * cols: Each column's cost and rows (OR-Library Group 2's format)
 */
const string INPUT_FORMAT = "cols";

/**
 * Algorithms list
 * +------+------------------+
 * | Code | Name             |
 * +------+------------------+
 * | NG   | Naive greedy     |
 * | OG   | Optimized greedy |
 * | 2M   | 2^M exact        |
 * | 2N   | 2^N exact        |
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
 * ∑w_i <= LL_MAX
 * 
 * 2^m exact:
 * 		2^M <= INT_MAX
 * 2^n exact:
 * 		2^N <= INT_MAX
 * 
 * Lastly, ensure that time and memory complexities are feasible.
 */

// Note: Zero-indexing used internally, 1-indexing used for I/O

int main () {
	const vector<pair<int, int>> SIZES = {
		{20, 1000},
		{1000, 20},
		{1000, 1000}
	};
	const vector<double> DENSITIES = {
		0.01,
		0.1,
		0.2,
		0.5
	};
	const int MX_COST = 100;
	const int TRIALS_PER_LEVEL = 1; // Trials per level
	const int TOTAL_TRIALS = SIZES.size() * DENSITIES.size() * TRIALS_PER_LEVEL;
	const string INPUT_FORMAT = "cols";
	const bool WRITE_INPUT = true;

	cerr << "Running SCP" << endl;
	cerr << endl;
	cerr << "Sizes (" << SIZES.size() << "): ";
	for (pair<int, int> size: SIZES) cerr << size.first << 'x' << size.second << ' ';
	cerr << endl;
	cerr << "Densities (" << DENSITIES.size() << "): ";
	for (double density: DENSITIES) cerr << density << ' ';
	cerr << endl;
	cerr << "Max column cost: " << MX_COST << endl;
	cerr << "Trials per level: " << TRIALS_PER_LEVEL << endl;
	cerr << endl;
	cerr << "Input format: " << INPUT_FORMAT << endl;
	cerr << "Input directory: " << INPUT_DIRECTORY << endl;
	cerr << "Output directory: " << OUTPUT_DIRECTORY << endl;
	cerr << "Input writing is " << (WRITE_INPUT ? "on" : "off") << endl;
	cerr << endl;

	auto startTime = chrono::system_clock::now();
	int trialCount = 0;
	for (pair<int, int> size: SIZES) {
		int n = size.first, m = size.second;
		for (double density: DENSITIES) {
			for (int trial = 1; trial <= TRIALS_PER_LEVEL; trial++, trialCount++) {
				cerr << '(' << trialCount + 1 << '/' << TOTAL_TRIALS << ")";
				cerr << ' ' << n << 'x' << m;
				cerr << ", " << density << " density";
				cerr << ", trial #" << trial << '/' << TRIALS_PER_LEVEL << endl;
				string dataSetName = "rand-" + INPUT_FORMAT + '-' +
					to_string(n) + 'x' + to_string(m) +
					"-MC" + to_string(MX_COST) +
					"-D" + doubleToStr(density) +
					"-S" + to_string(trial);
				cerr << "\tGenerating SCP instance " << dataSetName << "..." << endl;
				SCPinstance* input = generateSCPinstance(
					n,
					m,
					MX_COST,
					density,
					trial
				);
				if (WRITE_INPUT) {
					cerr << "\tWriting input file..." << endl;
					writeSCPinstance(input, INPUT_FORMAT, INPUT_DIRECTORY + dataSetName + ".txt");
				}
				vector<string> algorithms = {
					"NG",
					"OG"
				};
				if (m <= 20) algorithms.push_back("2M");
				if (n <= 20) algorithms.push_back("2N");
				for (string algorithm: algorithms) {
					cerr << "\tRunning " << algorithm << "...";
					SCPsolution* output = solveSCPinstance(input, algorithm);
					writeSCPsolution(output, OUTPUT_DIRECTORY + algorithm + '-' + dataSetName + ".txt");
					cerr << " (" << output->runtime << " s)" << endl;
				}
			}
		}
	}
	auto endTime = chrono::system_clock::now();
	chrono::duration<double> elapsed = endTime - startTime;
	double totalRuntime = elapsed.count();
	int minutes = totalRuntime / 60;
	double seconds = totalRuntime - 60 * minutes;
	cerr << "Completed " << TOTAL_TRIALS << " trials in " << minutes << ':' << seconds << endl;
}