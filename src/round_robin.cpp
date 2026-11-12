// Round Robin (preemptive) - ortak SchedulingResult arayuzune uyarlanmistir.
#include "scheduler.h"
#include <algorithm>
#include <queue>

SchedulingResult runRoundRobin(std::vector<Process> procs, int quantum) {
    SchedulingResult res;
    res.algorithm = "Round Robin (q=" + std::to_string(quantum) + ")";
    if (quantum <= 0 || procs.empty()) return res;

    // Varis zamanina gore sirala; esitlikte PID kucuk olan once
    std::stable_sort(procs.begin(), procs.end(),
        [](const Process &a, const Process &b) {
            if (a.arrival != b.arrival) return a.arrival < b.arrival;
            return a.pid < b.pid;
        });

    const int n = static_cast<int>(procs.size());
    std::vector<int> remaining(n);
    for (int i = 0; i < n; ++i) remaining[i] = procs[i].burst;

    std::queue<int> ready;
    int time = 0, next = 0, done = 0;

    auto admit = [&]() {
        while (next < n && procs[next].arrival <= time) ready.push(next++);
    };

    while (done < n) {
        admit();
        if (ready.empty()) {                       // CPU bos: sonraki varisa atla
            res.gantt.push_back({-1, time, procs[next].arrival});

            time = procs[next].arrival;
            continue;
        }

        int i = ready.front(); ready.pop();
        Process &p = procs[i];
        if (p.start == -1) p.start = time;         // ilk CPU alisi (response icin)

        int run = std::min(quantum, remaining[i]);
        // Ardisik ayni process dilimlerini Gantt'ta birlestir
        if (!res.gantt.empty() && res.gantt.back().pid == p.pid &&
            res.gantt.back().end == time)
            res.gantt.back().end = time + run;
        else
            res.gantt.push_back({p.pid, time, time + run});

        time += run;
        remaining[i] -= run;
        admit();                                   // calisirken gelenler once kuyruga

        if (remaining[i] > 0) {
            ready.push(i);
        } else {
            p.completion = time;
            p.turnaround = p.completion - p.arrival;
            p.waiting    = p.turnaround - p.burst;
            p.response   = p.start - p.arrival;
            ++done;
        }
    }

    res.processes = procs;

    computeMetrics(res);
    return res;
}