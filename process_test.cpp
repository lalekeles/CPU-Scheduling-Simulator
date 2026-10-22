#include <iostream>
#include "process.h"

int main() {
    Process p1;

    p1.pid = "P1";
    p1.arrivalTime = 0;
    p1.burstTime = 5;
    p1.remainingTime = 5;
    p1.completionTime = 0;
    p1.waitingTime = 0;
    p1.turnaroundTime = 0;

    std::cout << "Process: " << p1.pid << std::endl;
    std::cout << "Arrival time: " << p1.arrivalTime << std::endl;
    std::cout << "Burst time: " << p1.burstTime << std::endl;
    std::cout << "Remaining time: " << p1.remainingTime << std::endl;

    return 0;
}
