# Sunum Rehberi – COME 361 Milestone 2 Demo

Bu dosya: (1) hocaya ne göstereceğiz, (2) hangi sırayla, (3) hangi çıktıları açacağız, (4) hoca ne sorabilir.

## 0) Gelmeden önce (5 dk)

- [ ] Bilgisayarda repo güncel: `git pull` (hepsi `main` üzerinde olsun)
- [ ] Visual Studio'da `CPU-Scheduling-Simulator.sln` aç → **x64 / Debug** → bir kez derle (Ctrl+Shift+B), 0 hata olmalı
- [ ] `SchedulerTests`'i bir kez çalıştırıp "TUM TESTLER GECTI" gördüğünü doğrula
- [ ] `output/` klasörünü önceden bir kez üret (internet/Python sorunu olursa yedek: `docs/ornek_cikti/`)
- [ ] Python grafiği için: `pip install -r requirements.txt` önceden yapılmış olsun
- [ ] Terminal fontunu büyüt, Gantt chart satırları taşmasın

## 1) Hocanın checklist'i → bizde karşılığı

| Hocanın istediği | Bizde | Göstereceğimiz |
|---|---|---|
| Repo + README (nasıl derlenir/çalıştırılır) | ✅ | GitHub sayfası, README "Derleme ve çalıştırma" |
| Data structures & skeleton | ✅ | `include/process.h` (`Process`, `GanttEntry`, `SchedulingResult`), `scheduler.h` |
| Mini demo / PoC (dosya okuma vb.) | ✅ | `data/sample.txt` okunup tablo olarak yazdırılır |
| Task distribution | ✅ | README "Görev dağılımı" + `git log` |
| Completion rate 40–80% | ✅ ~%100 | 6 algoritma + export + grafik + test |
| Working core logic (live) | ✅ | `--all` ile baştan sona |
| Module integration | ✅ | FCFS+SJF+RR+Priority+Visualizer tek build'de |
| ≥2 test case, ≥1 edge case | ✅ | sample + convoy; edge: idle, tie, tek process |
| Outputs & metrics (terminal veya dosya) | ✅ | terminal tabloları + `output/*.csv` + `report.txt` + PNG |
| Git history | ✅ | `git log --oneline --graph --all` |

## 2) Canlı demo akışı (≈ 8–10 dk)

**Adım 1 – Repo ve yapı (1 dk).** GitHub sayfasını göster: README, `include/`, `src/`, `data/`, `tests/`.
"Her algoritma aynı imzayı kullanıyor: `SchedulingResult runXXX(vector<Process>)`. Böylece 4 kişi paralel çalışıp birleştirebildik."

**Adım 2 – Veri yapıları (1 dk).** `process.h` aç: `Process` (arrival, burst, priority + hesaplanan start/completion/waiting/turnaround/response), `GanttEntry` (pid=-1 → IDLE), `SchedulingResult`.

**Adım 3 – Mini demo: girdi → bellek (1 dk).** Visual Studio'da Command Arguments: `data/sample.txt`, çalıştır, menüden **11** (yüklü process'leri göster).

**Adım 4 – Ana senaryo, canlı (3 dk).** Command Arguments: `data/sample.txt --all --export`
Göster ve açıkla:
1. Tek bir algoritmanın tablosu + Gantt chart (örn. FCFS: P1 0-8, P2 8-12 ...)
2. Sonda **KARSILASTIRMA** tablosu: SRTF en düşük waiting/turnaround/response (5.80 / 11.60 / 3.40); Round Robin'in response'u FCFS/SJF'ten çok daha iyi (4.60 vs 8.20) ama en yüksek avg waiting (11.00) ve en çok context switch (8)
3. "En iyi değerler" satırları

**Adım 5 – Dosyaya çıktı (1 dk).** `output/summary.csv` ve `output/report.txt` aç. "Terminal çıktısı dosyaya da yazılıyor, Python tarafı bu CSV'leri okuyor."

**Adım 6 – Görselleştirme (1 dk).** `python cpu_project/visualizer.py` → `output/charts/` içindeki `comparison_averages.png` ve `gantt_charts.png`'yi aç.

**Adım 7 – Edge case'ler (2 dk).**
- `data/edge_idle_start.txt --algo fcfs` → Gantt'ta `IDLE (0-3)`: CPU ilk process gelene kadar boş
- `data/edge_same_arrival.txt --algo sjf` → eşit durumda deterministik sıralama (küçük PID)
- `data/convoy_effect.txt --all` → **convoy effect**: FCFS avg waiting 17.00 iken SJF 3.00. (Hocaya en etkili gösterilecek örnek)

**Adım 8 – Testler (30 sn).** `SchedulerTests` çalıştır → "147167 kontrol, 0 basarisiz". "Elle hesapladığımız değerler + 300 rastgele senaryo + formül tutarlılığı kontrol ediliyor."

**Adım 9 – Git geçmişi (1 dk).** `git log --oneline --graph --all`. Herkes kendi commit'lerini ve modülünü anlatır.

### Elle doğrulama için hızlı hesap (hoca "nasıl hesapladın" derse)
`sample.txt`, FCFS: P1(0-8) P2(8-12) P3(12-21) P4(21-26) P5(26-29).
P2: turnaround = 12−1 = 11, waiting = 11−4 = 7, response = 8−1 = 7.

## 3) Hoca ne sorabilir? (kısa cevaplarla)

**Algoritmalar**

1. **FCFS'in dezavantajı nedir?** Convoy effect: uzun bir iş başta ise kısa işler uzun süre bekler. `convoy_effect.txt` → avg waiting 17.00.
2. **SJF neden optimal?** Non-preemptive SJF, tüm process'ler aynı anda gelirse ortalama waiting time'ı minimum yapar. Ama burst süresini önceden bilmeyi gerektirir (gerçekte tahmin edilir, exponential averaging).
3. **SJF ile SRTF farkı?** SJF kesilmez; SRTF'de daha kısa kalan süreli process gelirse CPU ondan alınır. Bizde SRTF avg waiting 5.80, SJF 7.40.
4. **Starvation nedir, hangi algoritmalarda olur?** Uzun işlerin (SJF/SRTF) veya düşük öncelikli işlerin (Priority) sürekli öne geçilmesi. Çözüm: **aging** (bekledikçe önceliği artırma). Bizde aging yok; bilinçli sadelik.
5. **Round Robin'de quantum seçimi nasıl etkiler?** Çok büyük → FCFS'e dönüşür (testimiz bunu doğruluyor). Çok küçük → çok context switch, overhead artar. Genelde quantum, burst'lerin %80'inden büyük seçilir.
6. **RR'de neden ortalama waiting yüksek ama response iyi?** Herkes sırayla hemen CPU görür (response düşük) ama işler parça parça bittiği için turnaround/waiting artar.
7. **Priority'de küçük sayı mı büyük sayı mı öncelikli?** Bizde küçük = yüksek öncelik (README'de belirtilmiş). Eşitlikte erken gelen, sonra küçük PID.
8. **Preemptive ile non-preemptive farkı?** Preemptive'de çalışan process dışarıdan kesilebilir (daha öncelikli/kısa gelirse veya quantum bitince). Avantaj: response düşer; dezavantaj: context switch artar.
9. **Hangi algoritma en iyi?** Tek cevap yok: ortalama waiting için SRTF/SJF, response/interaktiflik için RR, basitlik için FCFS, önem sırası için Priority. Bu yüzden karşılaştırma modülü yazdık.

**Metrikler**

10. **Waiting, turnaround, response formülleri?** Turnaround = completion − arrival; waiting = turnaround − burst; response = ilk CPU − arrival.
11. **Response ile waiting ne zaman aynıdır?** Non-preemptive algoritmalarda (process bir kez çalışıp bitirir). Preemptive'de response ≤ waiting.
12. **CPU utilization nasıl hesaplanıyor?** Gantt'taki dolu süre / toplam süre × 100. Başta veya ortada IDLE varsa %100'ün altına düşer (`edge_idle_gap` → %42.31 FCFS).
13. **Context switch'i nasıl saydınız? Maliyeti var mı?** Gantt'taki process dilimi sayısı − 1. Maliyeti 0 varsaydık, sadece sayısını raporluyoruz.

**Kod / tasarım**

14. **Neden `std::vector<Process>` değer olarak (kopya) alınıyor?** Aynı girdi tüm algoritmalara verilip birbirini bozmasın diye.
15. **CPU boş kalırsa ne oluyor?** Hazır process yoksa zaman bir sonraki varış anına atlanır ve Gantt'a `pid = -1` (IDLE) dilimi eklenir.
16. **RR'de yeni gelen process ile quantum'u biten process aynı anda kuyruğa girerse?** Önce yeni gelen kuyruğa girer, sonra yarım kalan process (`admit()` çağrısı push'tan önce). Bu standart kabuldür.
17. **SRTF'de eşit kalan sürede ne olur?** Çalışan process kesilmez (gereksiz context switch olmasın diye).
18. **Zaman karmaşıklığı?** SJF/Priority/SRTF seçim döngüsü O(n) × adım sayısı; sınıf projesi boyutunda yeterli. Gerçek kernel'de öncelik kuyruğu/heap kullanılır.
19. **Doğruluğu nasıl test ettiniz?** Elle hesaplanmış değerler, ders kitabı örneği (convoy), edge case'ler ve rastgele senaryolarda formül/Gantt tutarlılığı. Ayrıca SRTF'nin waiting'i minimize ettiği teorik sonucu 300 senaryoda kontrol ediyoruz.
20. **Girdi hatalıysa?** Eksik dosya, eksik satır, negatif burst/arrival reddedilir; menüde geçersiz giriş programı kapatmaz.

**Süreç / Git**

21. **Kim neyi yaptı?** README görev dağılımı + `git log --format='%an %s'`. Her üye kendi modülünün kodunu açıp anlatabilmeli.
22. **Neden birden fazla branch var?** Her modül ayrı branch'te geliştirildi (`sjf`, `priority`, `birlestirme`), Pull Request ile `main`'e birleştirildi.
23. **Entegrasyonda ne sorun yaşadınız?** SJF ilk başta kendi bağımsız arayüzüyle yazılmıştı; ortak `SchedulingResult` arayüzüne uyarlandı (`sjf-duzeltme`, PR #4). Visualizer CSV formatı bekliyordu, C++ tarafına `--export` eklendi.
24. **Gerçek bir işletim sisteminden farkı?** I/O burst yok, context switch maliyeti yok, tek CPU çekirdeği, burst süreleri önceden biliniyor.
25. **Gelecekte ne eklenir?** Aging, multilevel feedback queue, çok çekirdek, I/O burst, rastgele iş yükü üretici.

## 4) Dürüstlük notu

Hoca "bunu kim yazdı?" diye sorarsa gerçeği söyleyin. Commit'ler bazen tek kişinin hesabından gitmiş olabilir
(ör. birleştirme sırasında). Önemli olan, herkesin kendi modülünün mantığını açıklayabilmesidir – önceden birbirinize kodu anlatın.

## 5) Sorun çıkarsa yedek plan

| Sorun | Çözüm |
|---|---|
| VS derlemiyor | Terminalde `g++ -std=c++11 -Iinclude -o scheduler src/*.cpp` (veya `make`) |
| "Dosya okunamadı" | Komut satırını proje kökünden ver: `scheduler.exe data\sample.txt` |
| Python/matplotlib yok | `docs/ornek_cikti/sample/charts/*.png` dosyalarını göster |
| İnternet yok | Hiçbir şey internet gerektirmiyor; repo yerelde hazır olsun |
