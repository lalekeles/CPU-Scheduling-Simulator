import pandas as pd
import matplotlib.pyplot as plt

def analyze_cpu_scheduling(csv_file='results.csv'):
    try:
        # 1. C++ ekibinin üreteceği results.csv dosyasını pandas ile okumak
        df = pd.read_csv(csv_file)
        print("--- Okunan Simülasyon Verileri ---")
        print(df)
        print("\n" + "="*40 + "\n")

        # 2. Algoritma başına ortalama Waiting Time ve Turnaround Time hesaplama
        if 'Algorithm' in df.columns:
            avg_metrics = df.groupby('Algorithm')[['WaitingTime', 'TurnaroundTime']].mean().reset_index()
            print("Algoritma Bazlı Ortalama Performans Değerleri:")
            print(avg_metrics)

            # 3. Algoritmaları karşılaştıran çubuk grafik (Bar Chart) oluşturmak
            plt.figure(figsize=(10, 6))
            
            x = range(len(avg_metrics['Algorithm']))
            width = 0.35

            plt.bar([i - width/2 for i in x], avg_metrics['WaitingTime'], width=width, label='Ortalama Bekleme Süresi (Waiting Time)', color='skyblue')
            plt.bar([i + width/2 for i in x], avg_metrics['TurnaroundTime'], width=width, label='Ortalama Dönüş Süresi (Turnaround Time)', color='salmon')

            plt.xlabel('Algoritmalar', fontsize=12, fontweight='bold')
            plt.ylabel('Süre (ms)', fontsize=12, fontweight='bold')
            plt.title('CPU Zamanlama Algoritmaları Performans Karşılaştırması', fontsize=14, fontweight='bold')
            plt.xticks(x, avg_metrics['Algorithm'])
            plt.legend()
            plt.grid(axis='y', linestyle='--', alpha=0.7)
            plt.tight_layout()

            # 4. Grafiği ekranda göstermek
            plt.show()
        else:
            print("Hata: CSV dosyasında 'Algorithm' sütunu bulunamadı!")

    except FileNotFoundError:
        print(f"Bilgi: '{csv_file}' dosyası henüz bulunamadı. C++ ekibi dosyayı oluşturduğunda bu betik doğrudan çalışacaktır.")

if __name__ == "__main__":
    analyze_cpu_scheduling()