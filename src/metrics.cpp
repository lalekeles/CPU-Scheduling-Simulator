#include "scheduler.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

void computeMetrics(SchedulingResult &res) {
    if (res.processes.empty()) return;
    double w = 0, t = 0, r = 0;
    for (const auto &p : res.processes) {
        w += p.waiting;
        t += p.turnaround;
        r += p.response;
    }
    double n = res.processes.size();
    res.avgWaiting    = w / n;
    res.avgTurnaround = t / n;
    res.avgResponse   = r / n;
}

static std::string label(int pid) {
    return pid == -1 ? "IDLE" : "P" + std::to_string(pid);
}

void printResult(const SchedulingResult &res) {
    std::cout << "\n===== " << res.algorithm << " =====\n";

    std::vector<Process> sorted = res.processes;
    std::sort(sorted.begin(), sorted.end(),
              [](const Process &a, const Process &b) { return a.pid < b.pid; });

    std::cout << std::left << std::setw(6) << "PID" << std::setw(9) << "Arrival"
              << std::setw(7) << "Burst" << std::setw(11) << "Completion"
              << std::setw(9) << "Waiting" << std::setw(12) << "Turnaround"
              << std::setw(9) << "Response" << "\n";
    for (const auto &p : sorted) {
        std::cout << std::left << std::setw(6) << label(p.pid)
                  << std::setw(9) << p.arrival << std::setw(7) << p.burst
                  << std::setw(11) << p.completion << std::setw(9) << p.waiting
                  << std::setw(12) << p.turnaround << std::setw(9) << p.response << "\n";
    }

    // Gantt chart: ust satir process'ler, alt satir zaman damgalari
    std::string top = "|", bottom;
    std::string t0 = std::to_string(res.gantt.front().start);
    bottom = t0;
    int pos = t0.size();
    for (const auto &g : res.gantt) {
        std::string cell = " " + label(g.pid) + " |";
        top += cell;
        std::string e = std::to_string(g.end);
        int target = top.size();
        // bitis zamanini hucrenin sag kenarina hizala
        if (target > pos) bottom += std::string(target - pos - 0, ' ');
        bottom += e;
        pos = target + e.size();
    }
    std::cout << "\nGantt Chart:\n" << top << "\n" << bottom << "\n";

    std::cout << std::fixed << std::setprecision(2)
              << "\nAvg Waiting Time    : " << res.avgWaiting
              << "\nAvg Turnaround Time : " << res.avgTurnaround
              << "\nAvg Response Time   : " << res.avgResponse << "\n";
}

void printComparison(const std::vector<SchedulingResult> &results) {
    std::cout << "\n===== KARSILASTIRMA =====\n";
    std::cout << std::left << std::setw(20) << "Algoritma" << std::setw(14) << "Avg Waiting"
              << std::setw(16) << "Avg Turnaround" << std::setw(14) << "Avg Response" << "\n";
    for (const auto &r : results) {
        std::cout << std::left << std::setw(20) << r.algorithm << std::fixed
                  << std::setprecision(2) << std::setw(14) << r.avgWaiting
                  << std::setw(16) << r.avgTurnaround << std::setw(14) << r.avgResponse << "\n";
    }
}
