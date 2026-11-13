# -*- coding: utf-8 -*-
"""
CPU Scheduling Simulator - Analiz ve gorsellestirme.

C++ programinin urettigi CSV dosyalarini (output/results.csv, output/gantt.csv,
output/summary.csv) okur, algoritmalari karsilastirir ve grafikleri
output/charts/ klasorune PNG olarak kaydeder.

Kullanim:
    # 1) once C++ tarafinda sonuclari uret:
    #    scheduler data/sample.txt --all --export
    # 2) sonra:
    python cpu_project/visualizer.py              # varsayilan: <proje>/output
    python cpu_project/visualizer.py --dir output --show

Gerekli paketler: pip install -r requirements.txt  (pandas, matplotlib)
"""
import argparse
import sys
from pathlib import Path

# Windows konsolunda Turkce karakter hatasi olmasin
try:
    sys.stdout.reconfigure(encoding="utf-8")
except Exception:
    pass

import pandas as pd
import matplotlib

PROJECT_ROOT = Path(__file__).resolve().parent.parent


def parse_args():
    ap = argparse.ArgumentParser(description="CPU scheduling sonuclarini analiz et ve grafikle")
    ap.add_argument("--dir", default=str(PROJECT_ROOT / "output"),
                    help="C++ programinin CSV dosyalarini yazdigi klasor (varsayilan: <proje>/output)")
    ap.add_argument("--show", action="store_true", help="grafikleri ekranda da goster")
    return ap.parse_args()


def load_csv(path: Path) -> pd.DataFrame:
    if not path.exists():
        print(f"[HATA] '{path}' bulunamadi.")
        print("       Once C++ programiyla sonuclari uretin, ornegin:")
        print("         scheduler data/sample.txt --all --export")
        sys.exit(1)
    return pd.read_csv(path)


def chart_averages(summary: pd.DataFrame, out: Path, plt):
    """Algoritma basina ortalama waiting / turnaround / response (grouped bar)."""
    metrics = [("AvgWaitingTime", "Ortalama Bekleme (Waiting)"),
               ("AvgTurnaroundTime", "Ortalama Donus (Turnaround)"),
               ("AvgResponseTime", "Ortalama Yanit (Response)")]
    algos = list(summary["Algorithm"])
    x = range(len(algos))
    width = 0.27
    fig, ax = plt.subplots(figsize=(11, 6))
    for k, (col, lbl) in enumerate(metrics):
        bars = ax.bar([i + (k - 1) * width for i in x], summary[col], width=width, label=lbl)
        for b in bars:
            ax.annotate(f"{b.get_height():.1f}", (b.get_x() + b.get_width() / 2, b.get_height()),
                        ha="center", va="bottom", fontsize=8)
    ax.set_xticks(list(x))
    ax.set_xticklabels(algos, rotation=15, ha="right")
    ax.set_ylabel("Zaman birimi")
    ax.set_title("CPU Zamanlama Algoritmalari - Ortalama Sureler")
    ax.legend()
    ax.grid(axis="y", linestyle="--", alpha=0.6)
    fig.tight_layout()
    fig.savefig(out / "comparison_averages.png", dpi=200)
    return fig


def chart_extra(summary: pd.DataFrame, out: Path, plt):
    """CPU kullanimi ve context switch sayisi."""
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    axes[0].bar(summary["Algorithm"], summary["CPUUtilization"], color="seagreen")
    axes[0].set_ylim(0, 105)
    axes[0].set_title("CPU Kullanimi (%)")
    axes[1].bar(summary["Algorithm"], summary["ContextSwitches"], color="darkorange")
    axes[1].set_title("Context Switch Sayisi")
    for ax in axes:
        ax.tick_params(axis="x", rotation=25)
        ax.grid(axis="y", linestyle="--", alpha=0.6)
    fig.tight_layout()
    fig.savefig(out / "cpu_and_context_switch.png", dpi=200)
    return fig


def chart_per_process(results: pd.DataFrame, out: Path, plt):
    """Her process'in her algoritmadaki bekleme suresi - adaleti (fairness) gosterir."""
    pivot = results.pivot(index="PID", columns="Algorithm", values="WaitingTime")
    pivot = pivot.loc[sorted(pivot.index, key=lambda s: int(str(s).lstrip("P")))]
    ax = pivot.plot(kind="bar", figsize=(12, 6), width=0.85)
    ax.set_ylabel("Waiting time")
    ax.set_title("Process Bazinda Bekleme Suresi (algoritmalara gore)")
    ax.grid(axis="y", linestyle="--", alpha=0.6)
    ax.legend(fontsize=8)
    ax.figure.autofmt_xdate(rotation=0)
    ax.figure.tight_layout()
    ax.figure.savefig(out / "waiting_per_process.png", dpi=200)
    return ax.figure


def chart_gantt(gantt: pd.DataFrame, out: Path, plt):
    """Her algoritma icin Gantt chart (tek sayfada alt alta)."""
    algos = list(dict.fromkeys(gantt["Algorithm"]))           # sirayi koru
    pids = sorted({p for p in gantt["PID"] if p != "IDLE"}, key=lambda s: int(s[1:]))
    cmap = plt.get_cmap("tab20")
    colors = {pid: cmap(i % 20) for i, pid in enumerate(pids)}
    colors["IDLE"] = "#d9d9d9"

    fig, axes = plt.subplots(len(algos), 1, figsize=(12, 1.5 * len(algos) + 1), sharex=True)
    if len(algos) == 1:
        axes = [axes]
    for ax, algo in zip(axes, algos):
        for _, row in gantt[gantt["Algorithm"] == algo].iterrows():
            ax.broken_barh([(row["Start"], row["End"] - row["Start"])], (0, 1),
                           facecolors=colors[row["PID"]], edgecolors="black", linewidth=0.6)
            if row["End"] - row["Start"] >= 1:
                ax.text((row["Start"] + row["End"]) / 2, 0.5, row["PID"],
                        ha="center", va="center", fontsize=8)
        ax.set_ylim(0, 1)
        ax.set_yticks([])
        ax.set_ylabel(algo, rotation=0, ha="right", va="center", fontsize=9)
    axes[-1].set_xlabel("Zaman")
    fig.suptitle("Gantt Chart Karsilastirmasi", fontweight="bold")
    fig.tight_layout()
    fig.savefig(out / "gantt_charts.png", dpi=200)
    return fig


def main():
    args = parse_args()
    if not args.show:
        matplotlib.use("Agg")                                   # ekran yoksa da calissin
    import matplotlib.pyplot as plt

    src = Path(args.dir)
    results = load_csv(src / "results.csv")
    gantt = load_csv(src / "gantt.csv")
    summary = load_csv(src / "summary.csv")
    out = src / "charts"
    out.mkdir(parents=True, exist_ok=True)

    print("--- Algoritma Bazli Ozet ---")
    print(summary.to_string(index=False))

    # En iyi algoritmalar (en dusuk deger)
    print("\n--- En iyi (en dusuk) algoritmalar ---")
    for col, name in [("AvgWaitingTime", "Waiting"), ("AvgTurnaroundTime", "Turnaround"),
                      ("AvgResponseTime", "Response"), ("ContextSwitches", "Context switch")]:
        best = summary[summary[col] == summary[col].min()]["Algorithm"].tolist()
        print(f"  {name:15s}: {', '.join(best)}  ({summary[col].min():g})")

    # Process bazli ek analiz: en uzun bekleyen process (starvation gostergesi)
    worst = results.loc[results.groupby("Algorithm")["WaitingTime"].idxmax()][["Algorithm", "PID", "WaitingTime"]]
    print("\n--- Her algoritmada en uzun bekleyen process ---")
    print(worst.to_string(index=False))

    summary.to_csv(out / "summary_report.csv", index=False)

    chart_averages(summary, out, plt)
    chart_extra(summary, out, plt)
    chart_per_process(results, out, plt)
    chart_gantt(gantt, out, plt)
    print(f"\n[Bilgi] Grafikler kaydedildi -> {out}")
    for f in sorted(out.glob("*.png")):
        print("   ", f.name)

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
