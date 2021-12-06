#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime> // srand seeding and timing
#include <cstdlib> // srand, rand
#include <sstream> // convert double to string

using namespace std;

string doubleToStr (double x) {
	ostringstream stream;
	stream << x;
	return stream.str();
}

struct SCPinstance {
	int n, m; // nxm matrix (n rows/elements and m columns/sets)
	vector<int> costs; // Costs of each column (must be <= INT_MAX = 2^31 - 1)
	/**
	 * rows[i] holds the sets containing element i
	 * cols[i] holds the elements contained in set i
	 */
	vector<vector<int>> rows, cols;

	// SCPinstance () {}

	SCPinstance (int n, int m) {
		this->n = n, this->m = m;
		costs.resize(m);
		rows.resize(n), cols.resize(m);
	}
};

struct SCPsolution {
	double runtime;
	long long totalCost = 0;
	vector<int> selected;
};