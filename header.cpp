/**
 * @file header.cpp
 * @brief This file imports libraries and defines helper functions and data
 * structures used in other files.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // Imports `sort()`
#include <chrono> // For timing algorithms
#include <cstdlib> // Imports `srand()` and `rand()`
#include <sstream> // For `doubleToString()`
#include <map> // For storing algorithm results

using namespace std;

/**
 * @brief Converts a `double` to a `string`.
 */
string doubleToString (double x) {
	ostringstream stream;
	stream << x;
	return stream.str();
}

/**
 * @brief Represents a solution to a (possibly weighted) SCP instance with the
 * runtime of the algorithm used.
 */
struct ScpInstance {
	int n, m; // nxm matrix (n rows/elements and m columns/sets)
	vector<int> costs; // Costs of each column (must be <= INT_MAX = 2^31 - 1)
	/**
	 * rows[i] holds the sets containing element i
	 * columns[i] holds the elements contained in set i
	 */
	vector<vector<int>> rows, columns;

	ScpInstance (int n, int m) {
		this->n = n, this->m = m;
		costs.resize(m);
		rows.resize(n), columns.resize(m);
	}
};

/**
 * @brief Represents a solution to a (possibly weighted) SCP instance with the
 * runtime of the algorithm used.
 */
struct ScpSolution {
	double runtime;
	long long total_cost = 0;
	vector<int> selected;
};

// The statistics recorded for each algorithm in `AlgorithmDataCollection`
// Minor note: These are named statistics to note they summarize samples of
// random instances as opposed to parameters for populations
// R = runtimes
// TC = total_costs
// AR = approx_ratios
const vector<string> kAlgorithmStats = {"R", "TC", "AR"};

/**
 * @brief Stores data collected for an algorithm paired with a data set setting
 * over an arbitary number of trials and provides some utililty functions.
 */
struct AlgorithmDataCollection {
	int trial_count = 0;
	map<string, vector<double>> data_lists;

	// Adds a solution to the record of data collected so far, possibly with an
	// exact total for computing approximation ratios.
	void recordTrial (ScpSolution* solution, long long exact_total) {
		trial_count++;
		data_lists["R"].push_back(solution->runtime);
		data_lists["TC"].push_back(solution->total_cost);
		double approx_ratio = exact_total > 0 ?
			(double)solution->total_cost / exact_total :
			-1;
		data_lists["AR"].push_back(approx_ratio);
	}
	// Computes the averages for each statistic.
	map<string, double> getAverages () {
		map<string, double> averages;
		for (pair<string, vector<double>> data_list: data_lists) {
			double sum = 0;
			for (double x: data_list.second) sum += x;
			averages[data_list.first] = sum / trial_count;
		}
		return averages;
	}
};