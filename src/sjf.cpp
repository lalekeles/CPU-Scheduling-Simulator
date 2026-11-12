// SJF (Shortest Job First) - Non-preemptive.
// Mantik arkadasin yazdigi surumden alinmis, ortak SchedulingResult arayuzune uyarlanmistir.
// Esitlikte: once erken gelen, o da esitse kucuk PID.
#include "scheduler.h"
#include <climits>

SchedulingResult runSJF(std::vector<Process> procs) {
    SchedulingResult res;
    res.algorithm = "SJF";
    if (procs.empty()) return res;

    const int n = static_cast<int>(procs.size());
    std::vector<bool> done(n, false);
    int time = 0, completed = 0;

    while (completed < n) {
        // Gelmis ve bitmemis process'ler arasindan en kisa burst'lu olani sec
        int idx = -1;
        for (int i = 0; i < n; ++i) {
            if (done[i] || procs[i].arrival > time) continue;
            if (idx == -1 ||
                procs[i].burst < procs[idx].burst ||
                (procs[i].burst == procs[idx].burst &&
                 (procs[i].arrival < procs[idx].arrival ||
                  (procs[i].arrival == procs[idx].arrival && procs[i].pid < procs[idx].pid))))
                idx = i;
        }

        // Hazir process yok: CPU bos, bir sonraki gelise atla
        if (idx == -1) {
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; ++i)

                if (!done[i] && procs[i].arrival < nextArrival) nextArrival = procs[i].arrival;
            res.gantt.push_back({-1, time, nextArrival});
            time = nextArrival;
            continue;
        }

        // Secilen process kesilmeden calisir
        Process &p = procs[idx];
        p.start      = time;
        p.completion = time + p.burst;
        p.turnaround = p.completion - p.arrival;
        p.waiting    = p.turnaround - p.burst;
        p.response   = p.start - p.arrival;

        res.gantt.push_back({p.pid, p.start, p.completion});
        time = p.completion;
        done[idx] = true;
        ++completed;
    }

    res.processes = procs;
    computeMetrics(res);
    return res;
}