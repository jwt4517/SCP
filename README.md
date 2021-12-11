**<h1 style="text-align: center;">SCP Data Collector</h1>**

# About
I wrote this code, chiefly comprising C++ algorithms, to collect my own data for my Senior Research Project (SRP) on the [Set Cover Problem (SCP)](https://en.wikipedia.org/wiki/Set_cover_problem).

# Structure
The repository contains 5 C++ functions to help collect data on heuristics for the set cover problem (SCP):

|Function name|File|Description|
|-|-|-|
|`generateSCPinstance`|`generator.cpp`|Generates random SCP instances (input data sets)|
|`writeSCPinstance`|`generator.cpp`|Writes a SCP instance to a file|
|`readSCPinstance`|`solver.cpp`|Reads and parses a SCP instance from a file|
|`solveSCPinstance`|`solver.cpp`|Solves or approximates a SCP instance using an algorithm, producing a SCP solution|
|`writeSCPsolution`|`solver.cpp`|Writes a SCP solution to a file|

# Usage
To manipulate how the functions are run, modify `main.cpp`.

To run the project, compile and run the entry point `main.cpp`. Example: `g++ -std=gnu++17 main.cpp -o main && ./main`.

**Note for contributors:** This code is written in alignment with the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

# Description and Info for the OR-Library Data Sets
The [OR-Library SCP section](http://people.brunel.ac.uk/~mastjjb/jeb/orlib/scpinfo.html) contains many real-world SCP data sets that can be used for testing.

**Note:** Lists of rows in each column and lists of columns in each row are not guaranteed to be in order. However, the algorithms in `solveSCPinstance` are insensitive to unordered lists (they work normally).

## Group 1 (80 files)
### Format: rows
```
# of rows/elements (n), # of columns/sets (m)
The cost c_j of each column 1 ≤ j ≤ m
For each row 1 ≤ i ≤ n ((1) and (2) are on separate lines):
(1) # of columns covering row i
(2) a list of columns covering row i, on one line
```
### Subgroup 1A (50 files)
|Problem set|File count|Files|
|-|-|-|
|4|10|scp41, ..., scp410|
|5|10|scp51, ..., scp510|
|6|5|scp61, ..., scp65|
|A|5|scpa1, ..., scpa5|
|B|5|scpb1, ..., scpb5|
|C|5|scpc1, ..., scpc5|
|D|5|scpd1, ..., scpd5|
|E|5|scpe1, ..., scpe5|
### Subgroup 1B (20 files)
|Problem set|File count|Files|
|-|-|-|
|E|5|scpnre1, ..., scpnre5|
|F|5|scpnrf1, ..., scpnrf5|
|G|5|scpnrg1, ..., scpnrg5|
|H|5|scpnrh1, ..., scpnrh5|
### Subgroup 1C (10 files)
|Problem set|File count|Files|
|-|-|-|
|Corresponds to CYC set (number of edges required to hit every 4-cycle in a hypercube)|6|scpcyc06, ..., scpcyc11|
|Corresponds to the CLR set of problems (number of 4-tuples forming the smallest non-bi-chromatic hypergraph)|4|scpclr10, ..., scpclr13|

## Group 2 (7 files)
### Format: columns
```
# of rows/elements (n), # of columns/sets (m)
For each column 1 ≤ j ≤ m ((1)-(3) are all on the same line):
(1) cost of column j
(2) # of rows that j covers
(3) a list of the rows j covers
```
### Subgroup 2A (7 files)
|File|Row count (n)|Column count (m)|
|-|-|-|
|rail507|507|63009|
|rail516|516|47311|
|rail582|582|55515|
|rail2536|2536|1081841|
|rail2586|2586|920683|
|rail4284|4284|1092610|
|rail4872|4872|968672|