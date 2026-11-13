// Basit birim testleri (harici kutuphane gerektirmez).
// Calistirma: make test   (veya Visual Studio'da "SchedulerTests" projesi)
//
// Uc tur test var:
//  1) Bilinen sonuclar : elle hesaplanmis ortalama degerlerle karsilastirma
//  2) Edge case'ler    : idle, ayni arrival, tek process, quantum > burst ...
//  3) Degismezler      : her algoritma, her girdi icin matematiksel kurallara uymali
#include "scheduler.h"
#include "input.h"
#include "export.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

static int g_checks = 0, g_failed = 0;

#define CHECK(cond, msg) do { ++g_checks; if (!(cond)) { ++g_failed; \
    std::cout << "  [HATA] " << msg << "  (" #cond ")\n"; } } while (0)

static bool approx(double a, double b) { return std::fabs(a - b) < 0.006; }

static std::vector<Process> load(const std::string &name) {
    std::vector<Process> v;
    bool ok = readProcessesFromFile(resolveInputPath("data/" + name), v);
    CHECK(ok, "dosya okunamadi: data/" + name);
    return v;
}

static std::vector<SchedulingResult> allAlgos(const std::vector<Process> &p, int q = 4) {
    std::vector<SchedulingResult> r;
    r.push_back(runFCFS(p));
    r.push_back(runSJF(p));
    r.push_back(runSRTF(p));
    r.push_back(runRoundRobin(p, q));
    r.push_back(runPriority(p));
    r.push_back(runPriorityPreemptive(p));
    return r;
}

static void expectAvg(const SchedulingResult &r, double w, double t, double rt) {
    CHECK(approx(r.avgWaiting, w),    r.algorithm + " avg waiting " + std::to_string(r.avgWaiting) + " beklenen " + std::to_string(w));
    CHECK(approx(r.avgTurnaround, t), r.algorithm + " avg turnaround " + std::to_string(r.avgTurnaround) + " beklenen " + std::to_string(t));
    CHECK(approx(r.avgResponse, rt),  r.algorithm + " avg response " + std::to_string(r.avgResponse) + " beklenen " + std::to_string(rt));
}

// Her algoritmanin uymasi gereken kurallar
static void checkInvariants(const SchedulingResult &r, const std::vector<Process> &in, const std::string &ctx) {
    const std::string tag = ctx + " / " + r.algorithm + ": ";
    CHECK(r.processes.size() == in.size(), tag + "process sayisi degisti");

    long busy = 0, burstSum = 0;
    for (const auto &p : in) burstSum += p.burst;

    for (const auto &p : r.processes) {
        CHECK(p.turnaround == p.completion - p.arrival, tag + "turnaround formulu P" + std::to_string(p.pid));
        CHECK(p.waiting == p.turnaround - p.burst,       tag + "waiting formulu P" + std::to_string(p.pid));
        CHECK(p.response == p.start - p.arrival,         tag + "response formulu P" + std::to_string(p.pid));
        CHECK(p.waiting >= 0 && p.response >= 0,         tag + "negatif sure P" + std::to_string(p.pid));
        CHECK(p.start >= p.arrival,                      tag + "gelmeden once basladi P" + std::to_string(p.pid));
        CHECK(p.response <= p.waiting,                   tag + "response > waiting P" + std::to_string(p.pid));
        CHECK(p.completion >= p.arrival + p.burst,       tag + "cok erken bitti P" + std::to_string(p.pid));
    }

    // Gantt: bitisik, cakismasiz, bosluksuz; calisma suresi = toplam burst
    CHECK(!r.gantt.empty(), tag + "gantt bos");
    int prevEnd = 0;
    for (size_t i = 0; i < r.gantt.size(); ++i) {
        const GanttEntry &g = r.gantt[i];
        CHECK(g.end > g.start,  tag + "bos/ters gantt dilimi");
        CHECK(g.start == prevEnd, tag + "gantt'ta bosluk veya cakisma (t=" + std::to_string(g.start) + ")");
        prevEnd = g.end;
        if (g.pid != -1) busy += g.end - g.start;
    }
    CHECK(busy == burstSum, tag + "CPU'da gecen sure toplam burst'e esit degil");
    CHECK(r.totalTime == prevEnd, tag + "totalTime gantt'in sonuyla ayni degil");
    CHECK(r.cpuUtilization > 0 && r.cpuUtilization <= 100.0 + 1e-9, tag + "CPU kullanimi 0-100 disinda");

    // Hicbir process kendi arrival'indan once calismamali
    for (const auto &g : r.gantt) {
        if (g.pid == -1) continue;
        for (const auto &p : in) if (p.pid == g.pid) CHECK(g.start >= p.arrival, tag + "arrival'dan once calisti");
    }
}

// ---------------------------------------------------------------- testler
static void testKnownResults() {
    std::cout << "[1] Bilinen sonuclar (elle hesaplanmis)\n";

    std::vector<Process> s = load("sample.txt");
    expectAvg(runFCFS(s),               8.20, 14.00, 8.20);
    expectAvg(runSJF(s),                7.40, 13.20, 7.40);
    expectAvg(runSRTF(s),               5.80, 11.60, 3.40);
    expectAvg(runRoundRobin(s, 4),     11.00, 16.80, 4.60);
    expectAvg(runPriority(s),           7.40, 13.20, 7.40);
    expectAvg(runPriorityPreemptive(s), 6.20, 12.00, 4.80);

    // Convoy effect: uzun is basta ise FCFS cok kotu
    std::vector<Process> c = load("convoy_effect.txt");
    expectAvg(runFCFS(c),            17.00, 27.00, 17.00);
    expectAvg(runSJF(c),              3.00, 13.00,  3.00);
    expectAvg(runRoundRobin(c, 4),    5.67, 15.67,  3.67);

    // SJF ders kitabi ornegi (Elif'in test senaryosu 1)
    std::vector<Process> k = load("sjf_classic.txt");
    SchedulingResult sj = runSJF(k);
    expectAvg(sj, 4.00, 8.00, 4.00);
    // SJF sirasi P1, P3, P2, P4 olmali
    CHECK(sj.gantt.size() == 4 && sj.gantt[0].pid == 1 && sj.gantt[1].pid == 3 &&
          sj.gantt[2].pid == 2 && sj.gantt[3].pid == 4, "SJF gantt sirasi P1,P3,P2,P4 degil");
    expectAvg(runSRTF(k), 3.00, 7.00, 0.50);
}

static void testEdgeCases() {
    std::cout << "[2] Edge case'ler\n";

    // (a) Baslangicta CPU bos: ilk process t=3'te geliyor
    std::vector<Process> idle = load("edge_idle_start.txt");
    for (const auto &r : allAlgos(idle)) {
        CHECK(!r.gantt.empty() && r.gantt[0].pid == -1 && r.gantt[0].start == 0 && r.gantt[0].end == 3,
              r.algorithm + ": ilk dilim IDLE(0-3) olmali");
        CHECK(r.idleTime == 3, r.algorithm + ": idle sure 3 olmali");
        expectAvg(r, 0.50, 2.00, 0.50);
    }

    // (b) Ortada bosluk: P1,P2 bitince CPU bekliyor, sonra P3,P4 geliyor
    std::vector<Process> gap = load("edge_idle_gap.txt");
    for (const auto &r : allAlgos(gap)) {
        bool foundMiddleIdle = false;
        for (size_t i = 1; i < r.gantt.size(); ++i) if (r.gantt[i].pid == -1) foundMiddleIdle = true;
        CHECK(foundMiddleIdle, r.algorithm + ": ortada IDLE dilimi olmali");
    }

    // (c) Ayni arrival + esitlikler: sonuc deterministik olmali (esitlikte kucuk PID)
    std::vector<Process> tie = load("edge_same_arrival.txt");
    SchedulingResult sj = runSJF(tie);
    CHECK(sj.gantt.size() == 3 && sj.gantt[0].pid == 3, "SJF: en kisa is (P3) once calismali");
    CHECK(sj.gantt.size() == 3 && sj.gantt[1].pid == 1, "SJF: esit burst'te kucuk PID (P1) once");
    SchedulingResult pr = runPriority(tie);
    CHECK(pr.gantt.size() == 3 && pr.gantt[0].pid == 2, "Priority: en kucuk oncelik sayisi (P2) once");
    expectAvg(sj, 1.67, 4.00, 1.67);

    // (d) Tek process: waiting = response = 0 (arrival=5 oldugu icin once IDLE)
    std::vector<Process> one = load("edge_single.txt");
    for (const auto &r : allAlgos(one)) {
        CHECK(r.processes.size() == 1 && r.processes[0].waiting == 0 && r.processes[0].response == 0,
              r.algorithm + ": tek process waiting/response 0 olmali");
        CHECK(r.processes[0].completion == 9, r.algorithm + ": tek process t=9'da bitmeli");
        CHECK(r.contextSwitches == 0, r.algorithm + ": tek process'te context switch 0 olmali");
    }

    // (e) Quantum >= en uzun burst ise Round Robin = FCFS
    std::vector<Process> s = load("sample.txt");
    SchedulingResult rrBig = runRoundRobin(s, 1000), fc = runFCFS(s);
    CHECK(approx(rrBig.avgWaiting, fc.avgWaiting) && approx(rrBig.avgTurnaround, fc.avgTurnaround),
          "RR (q buyuk) FCFS ile ayni sonucu vermeli");

    // (f) Quantum = 1: her dilim en fazla 1 birim
    SchedulingResult rr1 = runRoundRobin(s, 1);
    bool allUnit = true;
    for (const auto &g : rr1.gantt) if (g.pid != -1 && g.end - g.start < 1) allUnit = false;
    CHECK(allUnit, "RR q=1 gecerli dilimler uretmeli");

    // (g) Gecersiz girdiler
    CHECK(runRoundRobin(s, 0).processes.empty(), "RR quantum=0 bos sonuc dondurmeli");
    CHECK(runFCFS(std::vector<Process>()).processes.empty(), "Bos girdide cokmemeli");
    CHECK(runSJF(std::vector<Process>()).processes.empty(), "Bos girdide cokmemeli (SJF)");

    // (h) Dosya okuma hatalari
    std::vector<Process> tmp;
    CHECK(!readProcessesFromFile("bu_dosya_yok.txt", tmp), "Olmayan dosya false dondurmeli");
    std::string dir = defaultOutputDir() + "/_test";
    CHECK(ensureDirectory(dir), "test klasoru olusturulamadi");
    std::ofstream bad((dir + "/bad.txt").c_str());
    bad << "2\n0 5 1\n1 -3 1\n";                        // negatif burst
    bad.close();
    CHECK(!readProcessesFromFile(dir + "/bad.txt", tmp), "Negatif burst reddedilmeli");
    std::ofstream shortf((dir + "/short.txt").c_str());
    shortf << "3\n0 5 1\n";                              // eksik satirlar
    shortf.close();
    CHECK(!readProcessesFromFile(dir + "/short.txt", tmp), "Eksik satir reddedilmeli");
}

static unsigned g_seed = 12345;
static int rnd(int lo, int hi) {                          // sabit tohumlu LCG: sonuclar her makinede ayni
    g_seed = g_seed * 1103515245u + 12345u;
    return lo + static_cast<int>((g_seed >> 16) % static_cast<unsigned>(hi - lo + 1));
}

static void testInvariants() {
    std::cout << "[3] Degismezler (tum veri dosyalari + 300 rastgele senaryo)\n";

    const char *files[] = { "sample.txt", "convoy_effect.txt", "sjf_classic.txt", "edge_idle_start.txt",
                            "edge_idle_gap.txt", "edge_same_arrival.txt", "edge_single.txt", "large_10.txt" };
    for (const char *f : files) {
        std::vector<Process> p = load(f);
        for (int q = 1; q <= 5; q += 2)
            for (const auto &r : allAlgos(p, q)) checkInvariants(r, p, f);
    }

    for (int t = 0; t < 300; ++t) {
        int n = rnd(1, 12);
        std::vector<Process> p;
        for (int i = 0; i < n; ++i) p.push_back(Process(i + 1, rnd(0, 25), rnd(1, 12), rnd(0, 5)));
        int q = rnd(1, 8);
        std::vector<SchedulingResult> rs = allAlgos(p, q);
        for (const auto &r : rs) checkInvariants(r, p, "rastgele#" + std::to_string(t));

        // SRTF, ortalama waiting time'i en kucuk yapan algoritmadir (kanitli sonuc)
        for (const auto &r : rs)
            CHECK(rs[2].avgWaiting <= r.avgWaiting + 1e-9,
                  "rastgele#" + std::to_string(t) + ": SRTF en dusuk avg waiting'e sahip olmali (" + r.algorithm + ")");
    }
}

static void testExport() {
    std::cout << "[4] CSV / rapor disa aktarma\n";
    std::vector<Process> s = load("sample.txt");
    std::vector<SchedulingResult> rs = allAlgos(s);
    std::string dir = defaultOutputDir() + "/_test", err;
    CHECK(exportResults(rs, dir, err), "exportResults basarisiz: " + err);

    int lines = 0; std::string line, header;
    std::ifstream sf((dir + "/summary.csv").c_str());
    while (std::getline(sf, line)) { if (lines == 0) header = line; ++lines; }
    CHECK(lines == 7, "summary.csv 7 satir olmali (baslik + 6 algoritma), bulunan: " + std::to_string(lines));
    CHECK(header.find("AvgWaitingTime") != std::string::npos, "summary.csv basligi hatali");

    lines = 0;
    std::ifstream rf((dir + "/results.csv").c_str());
    while (std::getline(rf, line)) ++lines;
    CHECK(lines == 1 + 6 * 5, "results.csv 31 satir olmali (baslik + 6x5 process), bulunan: " + std::to_string(lines));
    CHECK(fileExists(dir + "/gantt.csv") && fileExists(dir + "/report.txt"), "gantt.csv / report.txt eksik");
}

int main() {
    std::cout << "=== CPU Scheduling Simulator - Testler ===\n";
    testKnownResults();
    testEdgeCases();
    testInvariants();
    testExport();
    std::cout << "\n" << g_checks << " kontrol, " << g_failed << " basarisiz.\n"
              << (g_failed == 0 ? "TUM TESTLER GECTI\n" : "BAZI TESTLER BASARISIZ\n");
    return g_failed == 0 ? 0 : 1;
}
