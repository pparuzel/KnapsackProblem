# KnapsackProblem
Knapsack Problem solved with a simple Genetic Algorithm

### Problem Formulation

_Given a number of items, with weights and their values, pack in as much value into the knapsack as possible so that the overall weight does not exceed the capacity of the bag. The largest sum of values is the solution._

**Required**: at least C++14

**Tested with**: GCC 9, Clang 9 and AppleClang 11

### Compiling
`g++ -std=c++14 knapsack_solver.cpp -o knapsack_solver`


### Fun facts
* Replaced all raw loops with `<algorithm>` functions if possible. Acquired 7x performance boost.
* Splitting the code into `.cpp` and `.hpp` caused 50% performance hit, probably due to the lack of inlining
* Without an optimization level (like `-O3`), the code is about 13 times slower
* The code fits into 80-character lines in width
* Some loops could have been replaced with std::for_each but that change made no sense overall
* Higher mutation rate slows down each iteration (insignificantly, but it does)
* Too high mutation rate ruins the convergence effectiveness to the solution (more errors)
* Too low mutation rate ruins the convergence accuracy to the solution (more errors)
* Evaluating fitness with Rouelette Wheel selection based on probability fails because summing up many floating point numbers escalates precision errors
* Even Kahan Summation does not help much in that case
* Roulette Wheel selection was, thus, done with integer values to avoid precision problems
