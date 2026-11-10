# 5G Uplink Multi-User Scheduler

A C++17 heuristic for allocating frequency resources to multiple users in a 5G uplink scheduling problem. Developed for **TECHARENA Challenge MSU 2024**, where the team placed **12th out of 54 entries**.

## Problem

The scheduler allocates resource blocks (RBs) to users with different demand and beam identifiers. It aims to maximize the total allocation credited to users, capped by each user's requested number of RBs.

A valid schedule must:

- Avoid reserved resource blocks and use no more than `J` non-overlapping intervals.
- Assign at most `L` users to each interval, with distinct beam identifiers.
- Give each user a contiguous frequency allocation, even when it spans adjacent intervals.

The complete task, constraints and examples are in the [competition statement](statements-240400.pdf).

## Approach

The implementation combines greedy allocation with local refinement:

1. **Group users by beam and demand.** Keep candidates from different beams to respect the interference constraint.
2. **Identify available intervals.** Split the bandwidth around reserved blocks and estimate the gain from assigning users to each free interval.
3. **Choose promising allocations.** Repeatedly select the candidate with the highest estimated gain, assign compatible users and consider interval splits. Update remaining demand and candidate scores after each allocation.
4. **Refine interval boundaries.** Shift boundaries between adjacent intervals when this increases the credited allocation, then discard empty intervals.

This is a competition heuristic; it does not guarantee a globally optimal schedule. The main implementation is in [`solver.cpp`](solver.cpp).

## Build and run

### Requirements

- A C++17 compiler, such as Clang or GCC.
- A POSIX-compatible shell for the commands below.
- CMake 3.22.3 or newer if using the optional CMake build.

### Quick start

Run these commands from the repository root:

```sh
mkdir -p build/run
c++ -std=c++17 -O2 main.cpp RBsCounter.cpp solver.cpp -o build/MSU
cp open.txt build/open.txt
(cd build/run && ../MSU)
tail -n 2 build/output.txt
```

The current runner reads `../open.txt` and writes `../output.txt` relative to its **working directory**. Running it from `build/run` keeps generated output inside `build/` and preserves the repository's original `output.txt`.

The C++17 build and execution were checked against the bundled **1,000-case** input. The final output lines were:

```text
Total RBs: 978758
Total RB need: 1388367
```

These values report credited allocations and requested demand across the bundled open dataset. The official competition evaluation also used hidden datasets.

### CMake alternative

The repository also defines an `MSU` executable through CMake:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
mkdir -p build/run
cp open.txt build/open.txt
(cd build/run && ../MSU)
```

These commands assume a single-configuration generator such as Unix Makefiles or Ninja. The reference run above used the direct compiler command.

## Input and output

`open.txt` starts with the number of test cases. Each case contains:

```text
N M K J L
reserved_start reserved_end   # repeated K times
required_RBs beam_id          # repeated N times
```

The comments above explain the format and are not part of the input file. `N` is the user count, `M` the bandwidth in RBs, `K` the reserved-interval count, `J` the maximum schedule-interval count, and `L` the maximum users per interval. Interval endpoints are half-open: `[start, end)`.

The competition entry point is `Solver(...)`, which returns intervals and their assigned user IDs. The local executable evaluates multiple cases, prints diagnostics for selected cases, and finishes with aggregate totals.

## Repository contents

| File | Purpose |
| --- | --- |
| `solver.cpp` | Allocation heuristic and boundary refinement |
| `solver.h` | Public data structures and solver declarations |
| `main.cpp` | Local batch runner and diagnostic output |
| `RBsCounter.cpp` | Allocation scoring and diagnostic helpers |
| `open.txt` | Bundled open test cases |
| `output.txt` | Existing local diagnostic output |
| `CMakeLists.txt` | CMake configuration for the C++17 executable |
| `statements-240400.pdf` | Official task description and submission interface |

## Authors

- **Daria Nelepova** - [d-da-das-dash-dasha](https://github.com/d-da-das-dash-dasha)
- **Alexey Antoshkin** - [AntoshkinAlexey](https://github.com/AntoshkinAlexey)
