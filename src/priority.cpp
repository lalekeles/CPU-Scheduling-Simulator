// Priority Scheduling (non-preemptive).
// Kucuk sayi = yuksek oncelik. Esit oncelikte once gelen, o da esitse PID kucuk olan.
#include "scheduler.h"
#include <algorithm>

SchedulingResult runPriority(std::vector<Process> procs) {
    SchedulingResult res;
    res.algorithm = "Priority";
    if (procs.empty()) return res;

    const int n = static_cast<int>(procs.size());
    std::vector<bool> done(n, false);
    int time = 0, finished = 0;

    while (finished < n) {
        // Su an gelmis ve bitmemis process'ler arasindan en yuksek oncelikliyi sec
        int best = -1;
        for (int i = 0; i < n; ++i) {
            if (done[i] || procs[i].arrival > time) continue;
            if (best == -1 ||
                procs[i].priority < procs[best].priority ||
                (procs[i].priority == procs[best].priority &&
                 (procs[i].arrival < procs[best].arrival ||
                  (procs[i].arrival == procs[best].arrival && procs[i].pid < procs[best].pid))))
                best = i;
        }

        if (best == -1) {
            // CPU bos: gelecek en erken process'e kadar idle
            int nextArrival = -1;
            for (int i = 0; i < n; ++i)
                if (!done[i] && (nextArrival == -1 || procs[i].arrival < nextArrival))
                    nextArrival = procs[i].arrival;
            res.gantt.push_back({-1, time, nextArrival});
            time = nextArrival;
            continue;
        }

        Process &p = procs[best];
        p.start      = time;
        p.completion = time + p.burst;
        p.turnaround = p.completion - p.arrival;
        p.waiting    = p.turnaround - p.burst;
        p.response   = p.start - p.arrival;

        res.gantt.push_back({p.pid, p.start, p.completion});
        time = p.completion;
        done[best] = true;
        ++finished;
    }

    res.processes = procs;
    computeMetrics(res);
    return res;
}