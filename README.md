# CPU Scheduling Simulator

Isletim Sistemleri dersi (Chapter 6 - CPU Scheduling) donem projesi.
Proje; FCFS, SJF, Round Robin ve Priority algoritmalarini simule eder,
her biri icin **waiting time, turnaround time ve response time** hesaplar,
Gantt chart cikarir ve algoritmalari karsilastirir.

## Derleme ve Calistirma

Gereksinimler: `g++` (C++11 veya ustu), `make`

```bash
make                           # derle
./scheduler                    # menu ile calistir
./scheduler data/sample.txt    # process'leri dosyadan okuyarak calistir
make clean                     # derleme ciktilarini sil
```

Make yoksa:
```bash
g++ -std=c++11 -Iinclude -o scheduler src/*.cpp
```

## Girdi Dosyasi Formati

```
<process sayisi>
<arrival> <burst> <priority>     (her process icin bir satir)
```
Ornek: `data/sample.txt`

## Proje Yapisi

```
include/process.h     Process, GanttEntry, SchedulingResult yapilari
include/scheduler.h   Algoritma fonksiyon imzalari (ortak arayuz)
include/input.h       Girdi fonksiyonlari
src/main.cpp          Ana program ve menu
src/input.cpp         Klavye / dosyadan process okuma
src/fcfs.cpp          FCFS algoritmasi
src/sjf.cpp           SJF (TODO)
src/round_robin.cpp   Round Robin (TODO)
src/priority.cpp      Priority (TODO)
src/metrics.cpp       Ortalama hesaplari, tablo ve Gantt yazdirma, karsilastirma
data/sample.txt       Ornek girdi
```

## Gorev Dagilimi

| Uye | Sorumluluk | Durum |
|-----|-----------|-------|
| Kisi 1 | Ana sistem (main.cpp), Process veri yapisi, kullanici girisi, FCFS, entegrasyon | Process yapisi, giris, FCFS, main tamam |
| Kisi 2 | SJF | Iskelet hazir, yazilacak |
| Kisi 3 | Round Robin | Iskelet hazir, yazilacak |
| Kisi 4 | Priority Scheduling | Iskelet hazir, yazilacak |

## Diger Uyeler Icin Not

Her algoritma `SchedulingResult runXXX(std::vector<Process> procs)` imzasini
kullanir. Her process icin `start, completion, waiting, turnaround, response`
alanlarini doldurun, `gantt` listesine dilimleri ekleyin ve sonunda
`computeMetrics(res)` cagirin. Ornek icin `src/fcfs.cpp` dosyasina bakin.
