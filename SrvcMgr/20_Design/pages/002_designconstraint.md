@page design_id 2. Design Constraints
* The module shall be compatible with Autosar 20-11
* The programming language shall be C
* The module shall follow at least the subset of MISRA 2012 rules specified by GM (ADVISORY, MANDATORY and REQUIRED)
* The module shall be compliant with the ETAS Static Analysis metrics for embedded code (2020) :

| Metric | Description | Range | QAC |
|--------|:-----------:|:-----:|:---:|
| PATH | Number of possible paths | 1 .. 1000 | STPTH |
| GOTO | Number of possible GOTOs | 0 | STGTO |
| v(G) | Number of decisions | 1 .. 20 | STCYC |
| PARAM | Number of parameters | 0 .. 12 | STPAR |
| STMT | Number of statements per function | 0 .. 100 | STST3 |
| LEVEL | Number of nesting control structures | 0 .. 6| STMIF |