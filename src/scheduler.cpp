
#include "scheduler.h"

#include <iostream>
#include <queue>
#include <algorithm>

std::vector<Process> runRoundRobin(
    std::vector<Process> processes,
    int quantum
) {
     if (quantum <= 0) {
        std::cout << "Error: quantum must be positive.\n";
        return {};
    }

    for (const Process& p : processes) {
        if (p.arrivalTime < 0) {
            std::cout << "Error: arrival time cannot be negative.\n";
            return {};
        }

        if (p.burstTime <= 0) {
            std::cout << "Error: burst time must be positive.\n";
            return {};
        }
    }

    // Surecleri varis zamanina gore sirala.
    std::sort(
        processes.begin(),
        processes.end(),
        [](const Process& a, const Process& b) {
            return a.arrivalTime < b.arrivalTime;
        }
    );

    std::queue<int> readyQueue;

    int time = 0;
    int nextProcess = 0;
    int n = static_cast<int>(processes.size());

    // Her surecin kalan calisma suresini baslat.
    for (Process& p : processes) {
        p.remainingTime = p.burstTime;
    }

    while (nextProcess < n || !readyQueue.empty()) {

        // Henuz surec gelmediyse CPU bos bekler.
        if (readyQueue.empty()) {
            if (time < processes[nextProcess].arrivalTime) {
                time = processes[nextProcess].arrivalTime;
            }
        }

        // O ana kadar gelen surecleri kuyruga ekle.
        while (
            nextProcess < n &&
            processes[nextProcess].arrivalTime <= time
        ) {
            readyQueue.push(nextProcess);
            nextProcess++;
        }

        if (readyQueue.empty()) {
            continue;
        }

        int index = readyQueue.front();
        readyQueue.pop();

        Process& current = processes[index];

        int runTime = std::min(quantum, current.remainingTime);

        std::cout << current.pid
                  << " runs from " << time
                  << " to " << time + runTime << '\n';

        time += runTime;
        current.remainingTime -= runTime;

        // Calisma sirasinda gelen surecleri once kuyruga ekle.
        while (
            nextProcess < n &&
            processes[nextProcess].arrivalTime <= time
        ) {
            readyQueue.push(nextProcess);
            nextProcess++;
        }

        if (current.remainingTime > 0) {
            readyQueue.push(index);
        } else {
            current.completionTime = time;
            current.turnaroundTime =
                current.completionTime - current.arrivalTime;
            current.waitingTime =
                current.turnaroundTime - current.burstTime;

            std::cout << current.pid << " completed.\n";
        }
    }
    
    std::cout << "\n===== Round Robin Results =====\n";

    std::cout << "PID\tAT\tBT\tCT\tTAT\tWT\n";

    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;

    for (const Process& p : processes) {
        std::cout << p.pid << '\t'
                  << p.arrivalTime << '\t'
                  << p.burstTime << '\t'
                  << p.completionTime << '\t'
                  << p.turnaroundTime << '\t'
                  << p.waitingTime << '\n';

        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
    }

    if (!processes.empty()) {
        double averageWaitingTime =
            totalWaitingTime / processes.size();

        double averageTurnaroundTime =
            totalTurnaroundTime / processes.size();

        std::cout << "\nAverage Waiting Time: "
                  << averageWaitingTime << '\n';

        std::cout << "Average Turnaround Time: "
                  << averageTurnaroundTime << '\n';
    }

    std::cout << "================================\n";

    return processes;
}
