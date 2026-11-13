// Preemptive algoritmalar: SRTF (Shortest Remaining Time First) ve
// Preemptive Priority. Ikisi de ayni simulasyon dongusunu kullanir,
// yalnizca "anahtar" (neye gore secim yapilacagi) farklidir.
//
// Kural: Calisan process, ancak anahtari KESINLIKLE daha iyi olan bir process
// gelirse kesilir. Esit anahtarda calisan devam eder (gereksiz context switch yok).
// Birden fazla aday ayni anahtara sahipse: once erken gelen, sonra kucuk PID.
#include "scheduler.h"
#include <climits>

namespace {

template <typename KeyFn>
SchedulingResult simulatePreemptive(std::vector<Process> procs,
                                    const std::string &name, KeyFn key) {
    SchedulingResult res;
    res.algorithm = name;
    if (procs.empty()) return res;

    const int n = static_cast<int>(procs.size());
    std::vector<int> remaining(n);
    for (int i = 0; i < n; ++i) remaining[i] = procs[i].burst;

    int time = 0, done = 0, cur = -1;

    while (done < n) {
        // Gelmis process'ler arasindan en iyi adayi bul
        int best = -1;
        for (int i = 0; i < n; ++i) {
            if (remaining[i] <= 0 || procs[i].arrival > time) continue;
            if (best == -1) { best = i; continue; }
            int ki = key(procs[i], remaining[i]);
            int kb = key(procs[best], remaining[best]);
            if (ki < kb ||
                (ki == kb && (procs[i].arrival < procs[best].arrival ||
                              (procs[i].arrival == procs[best].arrival &&
                               procs[i].pid < procs[best].pid))))
                best = i;
        }

        // Hazir process yok: CPU bos, bir sonraki gelise atla
        if (best == -1) {
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; ++i)
                if (remaining[i] > 0 && procs[i].arrival < nextArrival)
                    nextArrival = procs[i].arrival;
            res.gantt.push_back({-1, time, nextArrival});
            time = nextArrival;
            cur = -1;
            continue;
        }

        // Calisan process, sadece kesinlikle daha iyi aday varsa kesilir
        if (cur != -1 && remaining[cur] > 0 &&
            !(key(procs[best], remaining[best]) < key(procs[cur], remaining[cur])))
            best = cur;
        cur = best;

        Process &p = procs[cur];
        if (p.start == -1) p.start = time;        // ilk CPU alisi (response icin)

        // 1 zaman birimi calistir; ardisik ayni process dilimlerini birlestir
        if (!res.gantt.empty() && res.gantt.back().pid == p.pid &&
            res.gantt.back().end == time)
            res.gantt.back().end = time + 1;
        else
            res.gantt.push_back({p.pid, time, time + 1});

        ++time;
        --remaining[cur];

        if (remaining[cur] == 0) {
            p.completion = time;
            p.turnaround = p.completion - p.arrival;
            p.waiting    = p.turnaround - p.burst;
            p.response   = p.start - p.arrival;
            ++done;
            cur = -1;
        }
    }

    res.processes = procs;
    computeMetrics(res);
    return res;
}

} // namespace

// SRTF: kalan burst suresi en kisa olan process calisir.
SchedulingResult runSRTF(std::vector<Process> procs) {
    return simulatePreemptive(procs, "SRTF",
        [](const Process &, int remaining) { return remaining; });
}

// Preemptive Priority: kucuk sayi = yuksek oncelik.
SchedulingResult runPriorityPreemptive(std::vector<Process> procs) {
    return simulatePreemptive(procs, "Priority (Preemptive)",
        [](const Process &p, int) { return p.priority; });
}
