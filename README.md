# SCP Data Collector
This repository contains 5 functions:

|Function name|File|Description|
|-|-|-|
|`generateSCPinstance`|`generator.cpp`|Generates random SCP instances (input data sets)|
|`writeSCPinstance`|`generator.cpp`|Writes a SCP instance to a file|
|`readSCPinstance`|`solver.cpp`|Reads and parses a SCP instance from a file|
|`solveSCPinstance`|`solver.cpp`|Solves or approximates a SCP instance using an algorithm, producing a SCP solution|
|`writeSCPsolution`|`solver.cpp`|Writes a SCP solution to a file|

# Description and Info for the OR-Library Data Sets
http://people.brunel.ac.uk/~mastjjb/jeb/orlib/scpinfo.html

**Note:** Lists of rows in each column or columns in each row are not guaranteed to be in order

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
```
Problem set        Files
4                  scp41, ..., scp410
5                  scp51, ..., scp510
6                  scp61, ..., scp65
A                  scpa1, ..., scpa5
B                  scpb1, ..., scpb5
C                  scpc1, ..., scpc5
D                  scpd1, ..., scpd5
E                  scpe1, ..., scpe5
```
### Subgroup 1B (20 files)
```
Problem set        Files
E                  scpnre1, ..., scpnre5
F                  scpnrf1, ..., scpnrf5
G                  scpnrg1, ..., scpnrg5
H                  scpnrh1, ..., scpnrh5
```
### Subgroup 1C (10 files)
```
scpcyc06, ..., scpcyc11
scpclr10, ..., scpclr13
```

## Group 2 (7 files)
### Format: cols
```
# of rows/elements (n), # of columns/sets (m)
For each column 1 ≤ j ≤ m ((1)-(3) are all on the same line):
(1) cost of column j
(2) # of rows that j covers
(3) a list of the rows j covers
```
### Subgroup 2A (7 files)
```
rail507 with 507 rows and 63,009 columns
rail516 with 516 rows and 47,311 columns
rail582 with 582 rows and 55,515 columns
rail2536 with 2536 rows and 1,081,841 columns
rail2586 with 2586 rows and 920,683 columns
rail4284 with 4284 rows and 1,092,610 columns
rail4872 with 4872 rows and 968,672 columns
```