#include "input.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <cstdlib>

// Gecerli bir tam sayi (minValue'dan buyuk veya esit) okuyana kadar tekrar sorar.
static int readInt(const std::string &prompt, int minValue) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minValue) return value;
        if (std::cin.eof()) { std::cout << "\nGirdi sonlandi.\n"; std::exit(1); }  // Ctrl+Z / Ctrl+D ile sonsuz donguyu onle
        std::cout << "  Gecersiz deger! En az " << minValue << " olmali.\n";
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }
}

std::vector<Process> readProcessesFromUser() {
    std::vector<Process> procs;
    int n = readInt("Process sayisi: ", 1);

    for (int i = 0; i < n; i++) {
        std::cout << "\n--- Process P" << i + 1 << " ---\n";
        int arrival  = readInt("  Arrival time : ", 0);
        int burst    = readInt("  Burst time   : ", 1);
        int priority = readInt("  Priority (kucuk = oncelikli): ", 0);
        procs.push_back(Process(i + 1, arrival, burst, priority));
    }
    return procs;
}

// Dosya formati: ilk satir process sayisi, sonra her satir "arrival burst priority"
bool readProcessesFromFile(const std::string &filename, std::vector<Process> &out) {
    std::ifstream in(filename);
    if (!in) return false;

    int n;
    if (!(in >> n) || n <= 0) return false;

    std::vector<Process> tmp;
    for (int i = 0; i < n; i++) {
        int a, b, p;
        if (!(in >> a >> b >> p)) return false;
        if (a < 0 || b <= 0 || p < 0) return false;
        tmp.push_back(Process(i + 1, a, b, p));
    }
    out = tmp;
    return true;
}
