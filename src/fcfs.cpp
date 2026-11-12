#include "scheduler.h"
#include <algorithm>

// First-Come First-Served (non-preemptive):
// Process'ler varis zamanina gore siralanir ve sirayla calistirilir.
SchedulingResult runFCFS(std::vector<Process> procs) {
    SchedulingResult res;
    res.algorithm = "FCFS";

    // Varis zamanina gore sirala; esitlikte PID kucuk olan once
    std::stable_sort(procs.begin(), procs.end(),
        [](const Process &a, const Process &b) {
            if (a.arrival != b.arrival) return a.arrival < b.arrival;
            return a.pid < b.pid;
        });

    int time = 0;
    for (auto &p : procs) {
        // CPU bossa ve process henuz gelmediyse idle dilimi ekle
        if (time < p.arrival) {
            res.gantt.push_back({-1, time, p.arrival});
            time = p.arrival;
        }
        p.start      = time;
        p.completion = time + p.burst;
        p.turnaround = p.completion - p.arrival;
        p.waiting    = p.turnaround - p.burst;
        p.response   = p.start - p.arrival;

        res.gantt.push_back({p.pid, p.start, p.completion});
        time = p.completion;
    }

    res.processes = procs;
    computeMetrics(res);
    return res;
}
