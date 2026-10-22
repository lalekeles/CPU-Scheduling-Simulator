#pragma once
#include <string>
#include <vector>

struct Process {
    std::string pid;
    int arrivalTime = 0;
    int burstTime = 0;
    int remainingTime = 0;
    int completionTime = 0;
    int waitingTime = 0;
    int turnaroundTime = 0;
};

std::vector<Process> runSJF(std::vector<Process> processes);