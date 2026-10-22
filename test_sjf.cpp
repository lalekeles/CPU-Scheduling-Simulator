#include "scheduler.h"
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace std;

void printResults(const vector<Process>& result) {
    if (result.empty()) {
        cout << "Sonuc yok\n\n";
        return;
    }

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
    // Test 1: normal ornek
    vector<Process> test1 = {
        {"P1", 0, 7, 7}, {"P2", 2, 4, 4}, {"P3", 4, 1, 1}, {"P4", 5, 4, 4}
    };
    cout << "=== Test 1 ===\n";
    auto r1 = runSJF(test1);
    printResults(r1);
    assert(r1.size() == 4);
    assert(r1[0].pid == "P1" && r1[1].pid == "P3" &&
           r1[2].pid == "P2" && r1[3].pid == "P4");
    assert(r1[0].completionTime == 7);
    assert(r1[1].completionTime == 8);
    assert(r1[2].completionTime == 12);
    assert(r1[3].completionTime == 16);

    // Test 2: basta CPU bosta
    vector<Process> test2 = {
        {"P1", 3, 2, 2}, {"P2", 4, 1, 1}
    };
    cout << "=== Test 2 (basta IDLE) ===\n";
    auto r2 = runSJF(test2);
    printResults(r2);
    assert(r2[0].pid == "P1" && r2[0].completionTime == 5);
    assert(r2[1].pid == "P2" && r2[1].completionTime == 6);

    // Test 3: esit burst
    vector<Process> test3 = {
        {"P1", 0, 3, 3}, {"P2", 0, 3, 3}, {"P3", 0, 1, 1}
    };
    cout << "=== Test 3 (esitlik) ===\n";
    auto r3 = runSJF(test3);
    printResults(r3);
    assert(r3[0].pid == "P3" && r3[1].pid == "P1" && r3[2].pid == "P2");

    // Test 4: ortada bosluk
    vector<Process> test4 = {
        {"P1", 0, 2, 2}, {"P2", 10, 3, 3}
    };
    cout << "=== Test 4 (ortada IDLE) ===\n";
    auto r4 = runSJF(test4);
    printResults(r4);
    assert(r4[0].completionTime == 2);
    assert(r4[1].completionTime == 13);
    assert(r4[1].waitingTime == 0);

    // Test 5: tek process
    vector<Process> test5 = { {"P1", 0, 5, 5} };
    cout << "=== Test 5 (tek process) ===\n";
    auto r5 = runSJF(test5);
    printResults(r5);
    assert(r5.size() == 1);
    assert(r5[0].completionTime == 5);
    assert(r5[0].waitingTime == 0);

    // Test 6: bos vektor
    cout << "=== Test 6 (bos) ===\n";
    vector<Process> test6;
    auto r6 = runSJF(test6);
    printResults(r6);
    assert(r6.empty());

    // Test 7: preemption yok (uzun process kesilmez)
    vector<Process> test7 = {
        {"P1", 0, 10, 10}, {"P2", 1, 1, 1}
    };
    cout << "=== Test 7 (preemption yok) ===\n";
    auto r7 = runSJF(test7);
    printResults(r7);
    assert(r7[0].pid == "P1" && r7[1].pid == "P2");
    assert(r7[1].completionTime == 11);
    assert(r7[1].waitingTime == 9);

    cout << "TUM TESTLER GECTI\n";
    return 0;
}
