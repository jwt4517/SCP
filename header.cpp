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
#include <ctime> // Imports `time_t`, `time`, and `ctime`
#include <memory> // Imports `unique_ptr`
#include <regex> // Imports `regex` and `regex_replace`

using std::filesystem::create_directory;
using std::ifstream, std::ofstream;
using std::cout, std::endl;
using std::string, std::ostringstream, std::to_string;
using std::replace;
using std::vector, std::map, std::pair;
using std::chrono::system_clock, std::chrono::duration;
using std::unique_ptr;
using std::regex, std::regex_replace;

/**
 * @brief Converts a `double` to a `string`.
 */
string doubleToString (double x) {
	ostringstream stream;
	stream << x;
	return stream.str();
}

/**
 * @brief Converts a `double` to a `string` representing a component of a time
 * string. For example, the time components in "02:35:24.61" are "02", "35", and
 * "24.61".
 * 
 * @return string
 */
string doubleToTimeComponent (double x) {
	string result;
	if (x < 10) result += '0';
	result += doubleToString(x);
	return result;
}

string getCurrentTimeString () {
	time_t timer;
	time(&timer); // Stores current calendar time in `timer`
	string result = ctime(&timer);
	return result.substr(0, result.size() - 1);
}

/**
 * @brief Creates a left margin containing the current date and time, as well as
 * a border, to start a line logged to the console, such as
 * "Sun Dec 12 14:06:39 2021 | ".
 * 
 * @return string 
 */
string currentTimeMargin () {
	return getCurrentTimeString() + " | ";
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
	long long total_cost = 0;
	vector<int> selected;
	double runtime;
};

const vector<string> kAlgorithmIds = {"NG", "OG", "2ME", "2NE"};
const map<string, string> kAlgorithms = {
	{"NG", "Naive-greedy"},
	{"OG", "Optimized-greedy"},
	{"2ME", "2ᵐ-exact"},
	{"2NE", "2ⁿ-exact"}
};

// The statistics recorded for each algorithm in `AlgorithmDataCollection`
// Minor note: These are named statistics to note they summarize samples of
// random instances as opposed to parameters for populations
// ID's serve as codes/abbreviations for full names
const vector<string> kAlgorithmStatsIds = {"R", "TC", "AR"};
const map<string, string> kAlgorithmStats = {
	{"R", "Runtime"},
	{"TC", "Total cost"},
	{"AR", "Approximation ratio"}
};

/**
 * @brief Stores data collected for an algorithm paired with a data set setting
 * over an arbitary number of trials and provides some utililty functions.
 */
struct AlgorithmDataCollection {
	int trial_count = 0;
	map<string, vector<double>> data_lists;

	// Adds a solution to the record of data collected so far, possibly with an
	// exact total for computing approximation ratios.
	void recordTrial (
		unique_ptr<ScpSolution> &solution, long long exact_total
	) {
		trial_count++;
		data_lists["R"].push_back(solution->runtime);
		data_lists["TC"].push_back(solution->total_cost);
		double approx_ratio = exact_total > 0 ?
			(double)solution->total_cost / exact_total : -1;
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