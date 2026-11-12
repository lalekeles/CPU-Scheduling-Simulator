#ifndef INPUT_H
#define INPUT_H

#include "process.h"
#include <vector>
#include <string>

std::vector<Process> readProcessesFromUser();
bool readProcessesFromFile(const std::string &filename, std::vector<Process> &out);

#endif
