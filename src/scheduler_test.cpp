#include <iostream>
#include <vector>
#include "scheduler.h"

int main() {
   std::vector<Process> processes = {
    {"P1", 0, 5, 5, 0, 0, 0},
    {"P2", 1, 3, 3, 0, 0, 0},
    {"P3", 5, 2, 2, 0, 0, 0}
};

int quantum = 2;
    std::vector<Process> result =
        runRoundRobin(processes, quantum);

    std::cout << "Number of processes: "
              << result.size() << std::endl;

    return 0;
}
