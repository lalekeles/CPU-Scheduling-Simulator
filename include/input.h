#ifndef INPUT_H
#define INPUT_H

#include "process.h"
#include <vector>
#include <string>

std::vector<Process> readProcessesFromUser();
bool readProcessesFromFile(const std::string &filename, std::vector<Process> &out);

// n adet rastgele process uretir (her cagirista farkli sayilar).
// Arrival: 0-10, Burst: 1-9, Priority: 1-5 (kucuk = oncelikli)
std::vector<Process> generateRandomProcesses(int n);

#endif
