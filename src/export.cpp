#include "export.h"
#include "scheduler.h"
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <iomanip>

#ifdef _WIN32
  #include <direct.h>
  #define MAKE_DIR(p) _mkdir(p)
#else
  #include <sys/stat.h>
  #include <sys/types.h>
  #define MAKE_DIR(p) mkdir(p, 0755)
#endif

bool fileExists(const std::string &path) {
    std::ifstream f(path.c_str());
    return f.good();
}

std::string findProjectRoot() {
    std::string prefix;
    for (int up = 0; up <= 6; ++up) {
        if (fileExists(prefix + "data/sample.txt") && fileExists(prefix + "include/process.h"))
            return prefix;
        prefix += "../";
    }
    return "";
}

std::string resolveInputPath(const std::string &p) {
    if (fileExists(p)) return p;
    std::string alt = findProjectRoot() + p;
    if (fileExists(alt)) return alt;
    return p;
}

std::string defaultOutputDir() {
    return findProjectRoot() + "output";
}

bool ensureDirectory(const std::string &path) {
    // Her "/" veya "\" noktasinda o ana kadarki klasoru olustur (zaten varsa hata onemsiz)
    std::string cur;
    for (size_t i = 0; i < path.size(); ++i) {
        char c = path[i];
        if ((c == '/' || c == '\\') && !cur.empty() && cur != "." && cur != "..")
            MAKE_DIR(cur.c_str());
        cur += c;
    }
    if (!cur.empty() && cur != "." && cur != "..") MAKE_DIR(cur.c_str());
    // Yazilabildigini dogrula
    std::ofstream probe((path + "/.write_test").c_str());
    bool ok = probe.good();
    probe.close();
    if (ok) std::remove((path + "/.write_test").c_str());
    return ok;
}

static std::string label(int pid) {
    return pid == -1 ? "IDLE" : "P" + std::to_string(pid);
}

bool exportResults(const std::vector<SchedulingResult> &results,
                   const std::string &dir, std::string &error) {
    if (results.empty()) { error = "Disa aktarilacak sonuc yok."; return false; }
    if (!ensureDirectory(dir)) { error = "Klasor olusturulamadi / yazilamiyor: " + dir; return false; }

    const std::string base = dir + "/";

    // --- results.csv ---
    std::ofstream rf((base + "results.csv").c_str());
    if (!rf) { error = "results.csv yazilamadi."; return false; }
    rf << "Algorithm,PID,Arrival,Burst,Priority,Start,Completion,WaitingTime,TurnaroundTime,ResponseTime\n";
    for (const auto &r : results) {
        std::vector<Process> ps = r.processes;
        std::sort(ps.begin(), ps.end(),
                  [](const Process &a, const Process &b) { return a.pid < b.pid; });
        for (const auto &p : ps)
            rf << r.algorithm << ',' << label(p.pid) << ',' << p.arrival << ',' << p.burst
               << ',' << p.priority << ',' << p.start << ',' << p.completion << ','
               << p.waiting << ',' << p.turnaround << ',' << p.response << '\n';
    }

    // --- gantt.csv ---
    std::ofstream gf((base + "gantt.csv").c_str());
    if (!gf) { error = "gantt.csv yazilamadi."; return false; }
    gf << "Algorithm,PID,Start,End\n";
    for (const auto &r : results)
        for (const auto &g : r.gantt)
            gf << r.algorithm << ',' << label(g.pid) << ',' << g.start << ',' << g.end << '\n';

    // --- summary.csv ---
    std::ofstream sf((base + "summary.csv").c_str());
    if (!sf) { error = "summary.csv yazilamadi."; return false; }
    sf << "Algorithm,AvgWaitingTime,AvgTurnaroundTime,AvgResponseTime,CPUUtilization,ContextSwitches,TotalTime,Throughput\n";
    sf << std::fixed << std::setprecision(2);
    for (const auto &r : results)
        sf << r.algorithm << ',' << r.avgWaiting << ',' << r.avgTurnaround << ','
           << r.avgResponse << ',' << r.cpuUtilization << ',' << r.contextSwitches << ','
           << r.totalTime << ',' << std::setprecision(4) << r.throughput << std::setprecision(2) << '\n';

    // --- report.txt ---
    std::ofstream tf((base + "report.txt").c_str());
    if (!tf) { error = "report.txt yazilamadi."; return false; }
    tf << "CPU Scheduling Simulator - Rapor\n";
    for (const auto &r : results) printResult(r, tf);
    printComparison(results, tf);

    return true;
}
