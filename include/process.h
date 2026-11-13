#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <string>

// Tek bir process'in bilgilerini tutar.
struct Process {
    int pid;          // Process ID
    int arrival;      // Varis zamani
    int burst;        // CPU burst suresi
    int priority;     // Oncelik (kucuk sayi = yuksek oncelik)

    // Simulasyon sonunda hesaplanan degerler
    int start;        // CPU'yu ilk aldigi an
    int completion;   // Bitis zamani
    int waiting;      // Waiting time    = turnaround - burst
    int turnaround;   // Turnaround time = completion - arrival
    int response;     // Response time   = start - arrival

    Process(int p = 0, int a = 0, int b = 0, int pr = 0)
        : pid(p), arrival(a), burst(b), priority(pr),
          start(-1), completion(0), waiting(0), turnaround(0), response(0) {}
};

// Gantt chart'taki bir dilim. pid = -1 ise CPU bostadir (idle).
struct GanttEntry {
    int pid;
    int start;
    int end;
};

// Bir algoritmanin calistirilmasi sonucu elde edilen cikti.
// Tum algoritmalar (FCFS, SJF, RR, Priority) ayni yapiyi dondurur.
struct SchedulingResult {
    std::string algorithm;
    std::vector<Process> processes;   // Hesaplanmis degerlerle
    std::vector<GanttEntry> gantt;
    double avgWaiting;
    double avgTurnaround;
    double avgResponse;

    // computeMetrics() tarafindan Gantt chart'tan hesaplanan ek olcumler
    int totalTime;          // Simulasyonun bittigi an
    int idleTime;           // CPU'nun bos kaldigi toplam sure
    int contextSwitches;    // Process'ler arasi gecis sayisi
    double cpuUtilization;  // Yuzde (busy / totalTime * 100)
    double throughput;      // Birim zamanda biten process sayisi

    SchedulingResult()
        : avgWaiting(0), avgTurnaround(0), avgResponse(0),
          totalTime(0), idleTime(0), contextSwitches(0),
          cpuUtilization(0), throughput(0) {}
};

#endif
