#include "scheduler.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

// Ortalama waiting / turnaround / response ve Gantt chart'tan turetilen ek olcumler.
void computeMetrics(SchedulingResult &res) {
    if (res.processes.empty()) return;
    double w = 0, t = 0, r = 0;
    for (const auto &p : res.processes) {
        w += p.waiting;
        t += p.turnaround;
        r += p.response;
    }
    double n = static_cast<double>(res.processes.size());
    res.avgWaiting    = w / n;
    res.avgTurnaround = t / n;
    res.avgResponse   = r / n;

    // Gantt chart'tan: toplam sure, idle sure, context switch, CPU kullanimi
    int last = 0, busy = 0, idle = 0, dispatches = 0;
    for (const auto &g : res.gantt) {
        if (g.end > last) last = g.end;
        if (g.pid == -1) idle += g.end - g.start;
        else { busy += g.end - g.start; ++dispatches; }
    }
    res.totalTime       = last;
    res.idleTime        = idle;
    res.contextSwitches = dispatches > 0 ? dispatches - 1 : 0;
    res.cpuUtilization  = last > 0 ? 100.0 * busy / last : 0.0;
    res.throughput      = last > 0 ? n / last : 0.0;
}

static std::string label(int pid) {
    return pid == -1 ? "IDLE" : "P" + std::to_string(pid);
}

void printResult(const SchedulingResult &res, std::ostream &os) {
    os << "\n===== " << res.algorithm << " =====\n";

    std::vector<Process> sorted = res.processes;
    std::sort(sorted.begin(), sorted.end(),
              [](const Process &a, const Process &b) { return a.pid < b.pid; });

    os << std::left << std::setw(6) << "PID" << std::setw(9) << "Arrival"
       << std::setw(7) << "Burst" << std::setw(11) << "Completion"
       << std::setw(9) << "Waiting" << std::setw(12) << "Turnaround"
       << std::setw(9) << "Response" << "\n";
    for (const auto &p : sorted) {
        os << std::left << std::setw(6) << label(p.pid)
           << std::setw(9) << p.arrival << std::setw(7) << p.burst
           << std::setw(11) << p.completion << std::setw(9) << p.waiting
           << std::setw(12) << p.turnaround << std::setw(9) << p.response << "\n";
    }

    // Gantt chart: ust satir process'ler, alt satir zaman damgalari
    if (!res.gantt.empty()) {
        std::string top = "|", bottom;
        std::string t0 = std::to_string(res.gantt.front().start);
        bottom = t0;
        size_t pos = t0.size();
        for (const auto &g : res.gantt) {
            std::string cell = " " + label(g.pid) + " |";
            top += cell;
            std::string e = std::to_string(g.end);
            size_t target = top.size();
            // bitis zamanini hucrenin sag kenarina hizala
            if (target > pos) bottom += std::string(target - pos, ' ');
            bottom += e;
            pos = target + e.size();
        }
        os << "\nGantt Chart:\n" << top << "\n" << bottom << "\n";
    }

    std::ostringstream tmp;   // os'un bicim ayarlarini (fixed/precision) bozmamak icin
    tmp << std::fixed << std::setprecision(2)
        << "\nAvg Waiting Time    : " << res.avgWaiting
        << "\nAvg Turnaround Time : " << res.avgTurnaround
        << "\nAvg Response Time   : " << res.avgResponse
        << "\nCPU Utilization     : " << res.cpuUtilization << " %"
        << "\nThroughput          : " << res.throughput << " process/zaman birimi"
        << "\nContext Switches    : " << res.contextSwitches
        << "\nToplam Sure         : " << res.totalTime << "\n";
    os << tmp.str();
}

// Secilen olcute gore en kucuk degere sahip algoritma(lar)in adlarini dondurur.
template <typename Getter>
static std::string bestOf(const std::vector<SchedulingResult> &rs, Getter get) {
    double best = get(rs.front());
    for (const auto &r : rs) best = std::min(best, get(r));
    std::string names;
    for (const auto &r : rs) {
        if (get(r) - best < 1e-9) {
            if (!names.empty()) names += ", ";
            names += r.algorithm;
        }
    }
    return names;
}

void printComparison(const std::vector<SchedulingResult> &results, std::ostream &os) {
    if (results.empty()) return;
    os << "\n===== KARSILASTIRMA =====\n";
    os << std::left << std::setw(24) << "Algoritma" << std::setw(14) << "Avg Waiting"
       << std::setw(16) << "Avg Turnaround" << std::setw(14) << "Avg Response"
       << std::setw(10) << "CPU %" << std::setw(10) << "CtxSw" << "\n";
    for (const auto &r : results) {
        os << std::left << std::setw(24) << r.algorithm << std::fixed
           << std::setprecision(2) << std::setw(14) << r.avgWaiting
           << std::setw(16) << r.avgTurnaround << std::setw(14) << r.avgResponse
           << std::setw(10) << r.cpuUtilization << std::setw(10) << r.contextSwitches << "\n";
    }

    os << "\nEn iyi (en dusuk) degerler:\n"
       << "  Waiting Time    : " << bestOf(results, [](const SchedulingResult &r) { return r.avgWaiting; }) << "\n"
       << "  Turnaround Time : " << bestOf(results, [](const SchedulingResult &r) { return r.avgTurnaround; }) << "\n"
       << "  Response Time   : " << bestOf(results, [](const SchedulingResult &r) { return r.avgResponse; }) << "\n"
       << "  Context Switch  : " << bestOf(results, [](const SchedulingResult &r) { return static_cast<double>(r.contextSwitches); }) << "\n";
}
