#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <cstdlib>
#include "process.h"
#include "scheduler.h"
#include "input.h"
#include "export.h"

static void printMenu(int quantum) {
    std::cout << "\n===== CPU Scheduling Simulator =====\n"
              << "1)  Process'leri klavyeden gir\n"
              << "2)  Process'leri dosyadan oku\n"
              << "3)  FCFS calistir\n"
              << "4)  SJF (non-preemptive) calistir\n"
              << "5)  Round Robin calistir (mevcut quantum = " << quantum << ")\n"
              << "6)  Priority (non-preemptive) calistir\n"
              << "7)  Hepsini calistir ve karsilastir\n"
              << "8)  SRTF (preemptive SJF) calistir\n"
              << "9)  Priority (preemptive) calistir\n"
              << "10) Hepsini calistir ve sonuclari dosyaya yaz (CSV + rapor)\n"
              << "11) Yuklu process'leri goster\n"
              << "0)  Cikis\n"
              << "Secim: ";
}

static void printUsage(const char *prog) {
    std::cout <<
        "Kullanim:\n"
        "  " << prog << "                          menulu mod\n"
        "  " << prog << " <dosya>                  dosyayi yukle, menulu mod\n"
        "  " << prog << " <dosya> [secenekler]     menusuz (toplu) mod\n\n"
        "Secenekler:\n"
        "  --algo <ad>     fcfs | sjf | srtf | rr | priority | ppriority | all\n"
        "  --all           --algo all ile ayni (hepsi + karsilastirma)\n"
        "  --quantum <n>   Round Robin time quantum (varsayilan 4)\n"
        "  --export        sonuclari CSV + rapor olarak dosyaya yaz\n"
        "  --out <klasor>  --export icin hedef klasor (varsayilan <proje>/output)\n"
        "  --help          bu yardim\n\n"
        "Ornek:\n"
        "  " << prog << " data/sample.txt --all --export\n";
}

static void printProcesses(const std::vector<Process> &procs) {
    std::cout << "\nYuklu process'ler:\n"
              << std::left << std::setw(6) << "PID" << std::setw(9) << "Arrival"
              << std::setw(7) << "Burst" << std::setw(9) << "Priority" << "\n";
    for (const auto &p : procs)
        std::cout << std::left << std::setw(6) << ("P" + std::to_string(p.pid))
                  << std::setw(9) << p.arrival << std::setw(7) << p.burst
                  << std::setw(9) << p.priority << "\n";
}

// Henuz yazilmamis algoritma (bos sonuc) ise uyari verir.
static bool show(const SchedulingResult &r) {
    if (r.processes.empty()) {
        std::cout << "\n[" << r.algorithm << "] henuz implement edilmedi.\n";
        return false;
    }
    printResult(r);
    return true;
}

static std::vector<SchedulingResult> runAll(const std::vector<Process> &procs, int quantum) {
    std::vector<SchedulingResult> all;
    all.push_back(runFCFS(procs));
    all.push_back(runSJF(procs));
    all.push_back(runSRTF(procs));
    all.push_back(runRoundRobin(procs, quantum));
    all.push_back(runPriority(procs));
    all.push_back(runPriorityPreemptive(procs));
    return all;
}

static void doExport(const std::vector<SchedulingResult> &all, const std::string &dirArg) {
    std::string dir = dirArg.empty() ? defaultOutputDir() : dirArg;
    std::string err;
    if (exportResults(all, dir, err))
        std::cout << "\nSonuclar yazildi -> " << dir << "/ (results.csv, gantt.csv, summary.csv, report.txt)\n"
                  << "Grafikler icin: python cpu_project/visualizer.py\n";
    else
        std::cout << "\nDisa aktarma basarisiz: " << err << "\n";
}

// Menusuz mod. Basarili ise 0 dondurur.
static int runBatch(const std::vector<Process> &procs, const std::string &algo,
                    int quantum, bool doExp, const std::string &outDir) {
    std::vector<SchedulingResult> results;
    if (algo == "all" || algo.empty()) {
        results = runAll(procs, quantum);
    } else if (algo == "fcfs")           results.push_back(runFCFS(procs));
    else if (algo == "sjf")              results.push_back(runSJF(procs));
    else if (algo == "srtf")             results.push_back(runSRTF(procs));
    else if (algo == "rr")               results.push_back(runRoundRobin(procs, quantum));
    else if (algo == "priority")         results.push_back(runPriority(procs));
    else if (algo == "ppriority")        results.push_back(runPriorityPreemptive(procs));
    else {
        std::cout << "Bilinmeyen algoritma: " << algo << "\n";
        printUsage("scheduler");
        return 2;
    }

    for (const auto &r : results) show(r);
    if (results.size() > 1) printComparison(results);
    if (doExp) doExport(results, outDir);
    return 0;
}

int main(int argc, char *argv[]) {
    std::vector<Process> procs;
    std::string file, algo, outDir;
    int quantum = 4;
    bool doExp = false, batch = false;

    // ---- Komut satiri argumanlari ----
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--help" || a == "-h") { printUsage(argv[0]); return 0; }
        else if (a == "--all")    { algo = "all"; batch = true; }
        else if (a == "--algo" && i + 1 < argc)    { algo = argv[++i]; batch = true; }
        else if (a == "--quantum" && i + 1 < argc) {
            quantum = std::atoi(argv[++i]);
            if (quantum < 1) { std::cout << "Quantum >= 1 olmali.\n"; return 2; }
        }
        else if (a == "--export") { doExp = true; batch = true; }
        else if (a == "--out" && i + 1 < argc)     { outDir = argv[++i]; }
        else if (!a.empty() && a[0] != '-' && file.empty()) file = a;
        else { std::cout << "Taninmayan arguman: " << a << "\n\n"; printUsage(argv[0]); return 2; }
    }

    // ---- Dosya verilirse oku: ./scheduler data/sample.txt ----
    if (!file.empty()) {
        std::string path = resolveInputPath(file);
        if (readProcessesFromFile(path, procs)) {
            std::cout << procs.size() << " process dosyadan okundu (" << path << ").\n";
            if (batch) printProcesses(procs);
        } else {
            std::cout << "Dosya okunamadi veya format hatali: " << file << "\n";
            if (batch) return 1;
        }
    }

    if (batch) {
        if (procs.empty()) { std::cout << "Menusuz mod icin gecerli bir girdi dosyasi gerekli.\n"; return 1; }
        return runBatch(procs, algo, quantum, doExp, outDir);
    }

    // ---- Menulu mod ----
    int choice = -1;
    do {
        printMenu(quantum);
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) break;                      // girdi bitti
            std::cin.clear();                                // harf vs. girildi: temizle, tekrar sor
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            std::cout << "Gecersiz secim.\n";
            choice = -1;
            continue;
        }

        if (choice == 1) {
            procs = readProcessesFromUser();
        } else if (choice == 2) {
            std::string f;
            std::cout << "Dosya adi: ";
            std::cin >> f;
            if (!readProcessesFromFile(resolveInputPath(f), procs))
                std::cout << "Dosya okunamadi veya format hatali.\n";
            else
                std::cout << procs.size() << " process okundu.\n";
        } else if (choice >= 3 && choice <= 11) {
            if (procs.empty()) {
                std::cout << "Once process girmelisin (1 veya 2).\n";
                continue;
            }
            if (choice == 3) show(runFCFS(procs));
            else if (choice == 4) show(runSJF(procs));
            else if (choice == 5) {
                int q;
                std::cout << "Time quantum: ";
                if (!(std::cin >> q) || q < 1) {
                    std::cin.clear();
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                    std::cout << "Quantum >= 1 olmali.\n";
                    continue;                                // eskiden programi kapatiyordu
                }
                quantum = q;
                show(runRoundRobin(procs, quantum));
            } else if (choice == 6) show(runPriority(procs));
            else if (choice == 7) {
                std::vector<SchedulingResult> all = runAll(procs, quantum);
                for (const auto &r : all) show(r);
                printComparison(all);
            } else if (choice == 8) show(runSRTF(procs));
            else if (choice == 9) show(runPriorityPreemptive(procs));
            else if (choice == 10) {
                std::vector<SchedulingResult> all = runAll(procs, quantum);
                printComparison(all);
                doExport(all, "");
            } else if (choice == 11) printProcesses(procs);
        } else if (choice != 0) {
            std::cout << "Gecersiz secim.\n";
        }
    } while (choice != 0);

    std::cout << "Cikiliyor...\n";
    return 0;
}
