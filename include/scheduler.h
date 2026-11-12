#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include <vector>

// Her algoritma bu imzayi kullanir. Girdi kopya olarak alinir,
// boylece ayni process listesi tum algoritmalara verilebilir.

SchedulingResult runFCFS(std::vector<Process> procs);                    // Kisi 1 (hazir)
SchedulingResult runSJF(std::vector<Process> procs);                     // Kisi 2
SchedulingResult runRoundRobin(std::vector<Process> procs, int quantum); // Kisi 3
SchedulingResult runPriority(std::vector<Process> procs);                // Kisi 4

// Ortak yardimci fonksiyonlar (metrics.cpp)
void computeMetrics(SchedulingResult &res);
void printResult(const SchedulingResult &res);
void printComparison(const std::vector<SchedulingResult> &results);

#endif
