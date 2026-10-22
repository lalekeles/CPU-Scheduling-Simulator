#include "scheduler.h"
#include <climits>

using namespace std;

// Non-preemptive SJF
// Dönen vector, process'lerin ÇALIŞMA SIRASINA göre dizilmiş halidir.
vector<Process> runSJF(vector<Process> processes) {
    int n = processes.size();
    vector<bool> done(n, false);
    vector<Process> result;
    int currentTime = 0;
    int completed = 0;

    while (completed < n) {
        int idx = -1;

        // Gelmiş ve bitmemiş process'ler arasından en kısa burst'lüyü seç
        for (int i = 0; i < n; i++) {
            if (done[i] || processes[i].arrivalTime > currentTime) continue;

            if (idx == -1) {
                idx = i;
            } else if (processes[i].burstTime < processes[idx].burstTime) {
                idx = i;
            } else if (processes[i].burstTime == processes[idx].burstTime) {
                // Eşitlikte: önce erken gelen, o da eşitse küçük pid
                if (processes[i].arrivalTime < processes[idx].arrivalTime) {
                    idx = i;
                } else if (processes[i].arrivalTime == processes[idx].arrivalTime &&
                           processes[i].pid < processes[idx].pid) {
                    idx = i;
                }
            }
        }

        // Hazır process yok: CPU IDLE, zamanı bir sonraki geliş anına ilerlet
        if (idx == -1) {
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (!done[i] && processes[i].arrivalTime < nextArrival) {
                    nextArrival = processes[i].arrivalTime;
                }
            }
            currentTime = nextArrival;
            continue;
        }

        // Seçilen process kesilmeden çalışır
        currentTime += processes[idx].burstTime;
        processes[idx].remainingTime = 0;
        processes[idx].completionTime = currentTime;
        processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
        processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;

        done[idx] = true;
        completed++;
        result.push_back(processes[idx]);
    }

    return result;
}