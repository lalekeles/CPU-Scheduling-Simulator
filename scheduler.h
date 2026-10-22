#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "process.h"

std::vector<Process> runRoundRobin(
    std::vector<Process> processes,
    int quantum
);

#endif
