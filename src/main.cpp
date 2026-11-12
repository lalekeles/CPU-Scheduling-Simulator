#include <iostream>
#include <string>
#include <vector>
#include "process.h"
#include "scheduler.h"
#include "input.h"

static void printMenu() {
    std::cout << "\n===== CPU Scheduling Simulator =====\n"
              << "1) Process'leri klavyeden gir\n"
              << "2) Process'leri dosyadan oku\n"
              << "3) FCFS calistir\n"
              << "4) SJF calistir\n"
              << "5) Round Robin calistir\n"
              << "6) Priority calistir\n"
              << "7) Hepsini calistir ve karsilastir\n"
              << "0) Cikis\n"
              << "Secim: ";
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

int main(int argc, char *argv[]) {
    std::vector<Process> procs;

    // Dosya verilirse otomatik oku: ./scheduler data/sample.txt
    if (argc > 1) {
        if (readProcessesFromFile(argv[1], procs))
            std::cout << procs.size() << " process dosyadan okundu.\n";
        else
            std::cout << "Dosya okunamadi: " << argv[1] << "\n";
    }

    int choice;
    do {
        printMenu();
        if (!(std::cin >> choice)) break;

        if (choice == 1) {
            procs = readProcessesFromUser();
        } else if (choice == 2) {
            std::string f;
            std::cout << "Dosya adi: ";
            std::cin >> f;
            if (!readProcessesFromFile(f, procs))
                std::cout << "Dosya okunamadi veya format hatali.\n";
            else
                std::cout << procs.size() << " process okundu.\n";
        } else if (choice >= 3 && choice <= 7) {
            if (procs.empty()) {
                std::cout << "Once process girmelisin (1 veya 2).\n";
                continue;
            }
            if (choice == 3) show(runFCFS(procs));
            else if (choice == 4) show(runSJF(procs));
            else if (choice == 5) {
                int q;
                std::cout << "Time quantum: ";
                if (!(std::cin >> q) || q < 1) { std::cout << "Quantum >= 1 olmali.\n"; break; }
                show(runRoundRobin(procs, q));
            } else if (choice == 6) show(runPriority(procs));
            else {
                std::vector<SchedulingResult> all;
                const int quantum = 4;
                SchedulingResult rs[] = { runFCFS(procs), runSJF(procs),
                                          runRoundRobin(procs, quantum), runPriority(procs) };
                for (auto &r : rs) if (show(r)) all.push_back(r);
                printComparison(all);
            }
        } else if (choice != 0) {
            std::cout << "Gecersiz secim.\n";
        }
    } while (choice != 0);

    std::cout << "Cikiliyor...\n";
    return 0;
}
