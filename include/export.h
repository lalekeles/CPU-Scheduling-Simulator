#ifndef EXPORT_H
#define EXPORT_H

#include "process.h"
#include <vector>
#include <string>

// ---- Dosya yolu yardimcilari (Visual Studio / CMake / make hepsinde calissin diye) ----
// Visual Studio programi farkli calisma klasorlerinden baslatabilir
// (ornegin out/build/x64-Debug). Bu fonksiyonlar proje kokunu (data/ ve include/
// klasorlerinin bulundugu yer) ust klasorlere dogru arayip bulur.
bool fileExists(const std::string &path);
std::string findProjectRoot();                       // "" veya "../../" gibi (sonunda / var)
std::string resolveInputPath(const std::string &p);  // p bulunamazsa proje kokunde arar
std::string defaultOutputDir();                      // <proje koku>/output
bool ensureDirectory(const std::string &path);       // gerekirse ic ice olusturur

// ---- Sonuclari dosyaya yazma ----
// dir icine sunlari yazar:
//   results.csv  : her algoritma x her process icin tum degerler
//   gantt.csv    : Gantt chart dilimleri
//   summary.csv  : algoritma bazli ortalamalar ve ek olcumler
//   report.txt   : terminal ciktisinin aynisi (tablolar + Gantt + karsilastirma)
bool exportResults(const std::vector<SchedulingResult> &results,
                   const std::string &dir, std::string &error);

#endif
