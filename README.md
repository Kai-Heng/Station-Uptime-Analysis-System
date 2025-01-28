# README

This project implements a **station uptime calculator** in C++. It reads an input file describing stations, their chargers, and per-charger availability intervals, then calculates and prints each station’s uptime percentage.

## 1. Files and Directories

- **`main.cpp`**: The main source file for the station uptime calculator.
- **`test_main.cpp`**: A simple unit test driver for key utility functions (optional).
- **`functions.h`**: Shared function declarations and inline implementations (optional).
- **`Makefile`**: Used to build the project and run tests.
- **`test_input_n.txt`**: Example input files for integration testing.

## 2. Building

Ensure you have a modern C++ compiler (e.g., g++ with support for C++17) installed, then run:

```bash
make
```

This will produce two executables:
1. **`station_uptime`**: The main program  
2. **`test_main`**: The test driver (unit tests)

## 3. Running the Program

```bash
./station_uptime <path/to/input_file.txt>
```

- The program will parse the file and either print:
  - `ERROR` if the file is invalid, or
  - One line per station in the format `<station_id> <uptime_percentage>`.

## 4. Testing

### 4.1. Integration Tests

Several example input files (`test_input_n.txt`) are provided. For instance:

```bash
./station_uptime <test_input_3.txt>
```

Compare the output with the expected results provided (usually in the `input_3_expected_stdout.txt` or in the documentation).

### 4.2. Unit Tests

Run the following:

```bash
make test
```

Which will:
1. Ensure `test_main` is built.
2. Execute `./test_main`, running internal unit tests for the interval-merging logic and other helpers.  
3. Print success messages or assertions if a test fails.

## 5. Cleaning Up

To remove the executables and object files:

```bash
make clean
```

This deletes the generated files (`station_uptime`, `test_main`, and all `.o` files).

---
