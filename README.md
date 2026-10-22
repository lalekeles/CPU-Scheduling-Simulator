# CPU Scheduling Simulator

## Project Description

This project simulates CPU scheduling algorithms as part of an Operating Systems course project.

The goal is to implement and compare different CPU scheduling algorithms, including:

- First Come, First Served (FCFS)
- Shortest Job First (SJF)
- Round Robin (RR)

## Current Progress

- Round Robin scheduling algorithm implemented in C++.
- Process arrival times and burst times are supported.
- Completion time, turnaround time, and waiting time are calculated.
- Average waiting time and average turnaround time are displayed.
- Basic input validation is included.

## Requirements

- A C++ compiler supporting C++17 (e.g., g++)
- Visual Studio Code (optional)

## How to Compile and Run

Open a terminal in the project directory and run:

```bash
g++ src/scheduler_test.cpp src/scheduler.cpp -o scheduler_test
```

Then run the program:

**Windows PowerShell:**

```powershell
.\scheduler_test.exe
```

**Linux / macOS:**

```bash
./scheduler_test
```

## Team Project

This repository is being developed as a group project for the Operating Systems course.

More scheduling algorithms and Python-based performance analysis will be added as development continues.
