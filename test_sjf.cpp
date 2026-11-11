#include "scheduler.h"
#include <iostream>
#include <iomanip>

using namespace std;

void printResults(const vector<Process>& result) {
    double totalWT = 0, totalTAT = 0;

    cout << left << setw(8) << "PID" << setw(10) << "Arrival" << setw(8) << "Burst"
         << setw(12) << "Completion" << setw(12) << "Turnaround" << setw(8) << "Waiting" << "\n";

    for (const auto& p : result) {
        cout << left << setw(8) << p.pid << setw(10) << p.arrivalTime << setw(8) << p.burstTime
             << setw(12) << p.completionTime << setw(12) << p.turnaroundTime
             << setw(8) << p.waitingTime << "\n";
        totalWT += p.waitingTime;
        totalTAT += p.turnaroundTime;
    }

    cout << fixed << setprecision(2);
    cout << "Ortalama Waiting Time    : " << totalWT / result.size() << "\n";
    cout << "Ortalama Turnaround Time : " << totalTAT / result.size() << "\n";

    // Gantt şeması (IDLE dahil)
    cout << "Gantt: ";
    int prevEnd = 0;
    for (const auto& p : result) {
        int start = p.completionTime - p.burstTime;
        if (start > prevEnd) cout << "| IDLE (" << prevEnd << "-" << start << ") ";
        cout << "| " << p.pid << " (" << start << "-" << p.completionTime << ") ";
        prevEnd = p.completionTime;
    }
    cout << "|\n\n";
}

int main() {
    // Test 1: Elle hesapladığımız örnek (beklenen sıra P1 P3 P2 P4, ort. WT 4.00, ort. TAT 8.00)
    vector<Process> test1 = {
        {"P1", 0, 7, 7}, {"P2", 2, 4, 4}, {"P3", 4, 1, 1}, {"P4", 5, 4, 4}
    };
    cout << "=== Test 1 ===\n";
    printResults(runSJF(test1));

    // Test 2: Başta CPU boşta kalıyor (P1 3. zamanda geliyor)
    vector<Process> test2 = {
        {"P1", 3, 2, 2}, {"P2", 4, 1, 1}
    };
    cout << "=== Test 2 (IDLE) ===\n";
    printResults(runSJF(test2));

    // Test 3: Aynı anda gelen, burst'leri eşit process'ler
    vector<Process> test3 = {
        {"P1", 0, 3, 3}, {"P2", 0, 3, 3}, {"P3", 0, 1, 1}
    };
    cout << "=== Test 3 (eşitlik) ===\n";
    printResults(runSJF(test3));

    return 0;
}