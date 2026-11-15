# CPU Scheduling Simulator

Isletim Sistemleri dersi (Chapter 6 - CPU Scheduling) donem projesi.
Proje; FCFS, SJF, Round Robin ve Priority algoritmalarini simule eder,
her biri icin **waiting time, turnaround time ve response time** hesaplar,
Gantt chart cikarir ve algoritmalari karsilastirir.

## Derleme ve Calistirma

Gereksinimler: `g++` (C++11 veya ustu)

Windows (PowerShell):
```
g++ -std=c++11 -Wall -Wextra -Iinclude -o scheduler (Get-ChildItem src\*.cpp).FullName
.\scheduler.exe data\sample.txt
```

Linux / macOS (make ile):
```
make
./scheduler data/sample.txt
```

Istenirse dosya parametresiyle de baslatilabilir (`.\scheduler.exe data\sample.txt`), process'ler otomatik yuklenir.

## Menu

| Secim | Islem |
|-------|-------|
| 1 | Process'leri klavyeden gir |
| 2 | Rastgele process olustur (process sayisi sorulur, her seferinde farkli sayilar) |
| 3 | FCFS |
| 4 | SJF (non-preemptive) |
| 5 | Round Robin (time quantum sorulur) |
| 6 | Priority (non-preemptive) |
| 7 | Hepsini calistir ve karsilastir (Round Robin icin quantum sorulur) |
| 0 | Cikis |

## Algoritma Kurallari

- **FCFS:** Gelis sirasina gore, kesilmez.
- **SJF:** Hazir process'ler arasinda en kisa burst secilir, kesilmez. Esitlikte once gelen, sonra kucuk PID.
- **Round Robin:** Her process en fazla quantum kadar calisir, bitmediyse kuyrugun sonuna gider. Calisirken gelen yeni process, kesilen process'ten once kuyruga girer.
- **Priority:** Kucuk sayi = yuksek oncelik. Kesilmez. Esitlikte once gelen, sonra kucuk PID.
- CPU bos kalirsa Gantt chart'ta `IDLE` gosterilir.
- Waiting = Turnaround - Burst, Turnaround = Completion - Arrival, Response = Ilk CPU zamani - Arrival.

## Rastgele Process Uretimi

Menude `2` secilince process sayisi sorulur, process'ler rastgele uretilir ve tablo olarak ekrana yazilir.
Arrival 0-10, Burst 1-9, Priority 1-5 araligindadir. Ayni veriyle farkli algoritmalar calistirilabilir (3-7);
yeni veri icin tekrar `2` secilir.

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
src/fcfs.cpp          FCFS
src/sjf.cpp           SJF
src/round_robin.cpp   Round Robin
src/priority.cpp      Priority
src/metrics.cpp       Ortalama hesaplari, tablo ve Gantt yazdirma, karsilastirma
data/sample.txt       Ornek girdi
cpu_project/visualizer.py   (Opsiyonel) results.csv'den grafik cizer; program su an bu dosyayi uretmez
```

## Ornek Sonuclar (data/sample.txt)

| Algoritma | Avg Waiting | Avg Turnaround | Avg Response |
|-----------|-------------|----------------|--------------|
| FCFS | 8.20 | 14.00 | 8.20 |
| SJF | 7.40 | 13.20 | 7.40 |
| Round Robin (q=3) | 12.00 | 17.80 | 3.60 |
| Priority | 7.40 | 13.20 | 7.40 |
