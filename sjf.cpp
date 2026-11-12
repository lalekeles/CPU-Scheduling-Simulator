#include "scheduler.h"
#include <algorithm>
#include <vector>

// Non-preemptive SJF.
// Secim kurali: en kisa burst, esitse kucuk arrival, o da esitse kucuk PID.
SchedulingResult runSJF(std::vector<Process> procs) {
    SchedulingResult res;
    res.algorithm = "SJF";

    const int n = (int)procs.size();
    std::vector<bool> done(n, false);
    int completed = 0;
    int time = 0;

    while (completed < n) {
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (done[i] || procs[i].arrival > time) continue;
            if (idx == -1 ||
                procs[i].burst < procs[idx].burst ||
                (procs[i].burst == procs[idx].burst &&
                 (procs[i].arrival < procs[idx].arrival ||
                  (procs[i].arrival == procs[idx].arrival &&
                   procs[i].pid < procs[idx].pid)))) {
                idx = i;
            }
        }

        // Hazir process yok: CPU bosta, sonraki gelis ani kadar idle dilimi
        if (idx == -1) {
            int nextArrival = 1 << 30;
            for (int i = 0; i < n; i++)
                if (!done[i]) nextArrival = std::min(nextArrival, procs[i].arrival);
            res.gantt.push_back({-1, time, nextArrival});
            time = nextArrival;
            continue;
        }

        Process &p = procs[idx];
        p.start      = time;
        p.response   = p.start - p.arrival;
        time        += p.burst;
        p.completion = time;
        p.turnaround = p.completion - p.arrival;
        p.waiting    = p.turnaround - p.burst;

        res.gantt.push_back({p.pid, p.start, p.completion});
        res.processes.push_back(p);

        done[idx] = true;
        completed++;
    }

    computeMetrics(res);
    return res;
}