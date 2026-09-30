"""비교 결과를 그래프(SVG)로 그린다. 표준 모듈만 쓴다.

    make charts          # 또는 python3 tools/plot.py

src/main.out --csv 를 돌려 측정값을 받아 report/ 아래에 SVG와 results.csv를 쓴다.
"""

import csv
import io
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import svgchart  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
BINARY = ROOT / "src" / "main.out"
OUT_DIR = ROOT / "report"
ALGOS = ["mergeSort", "quickSort", "heapSort"]
KIND_KEYS = ["random", "sorted", "reversed", "nearly-sorted", "few-unique"]
KIND_LABEL = {
    "random": "무작위",
    "sorted": "정렬됨",
    "reversed": "역순",
    "nearly-sorted": "거의정렬",
    "few-unique": "중복많음",
}


REUSE = "--reuse" in sys.argv  # 다시 재지 않고 report/*.csv로 그림만 다시 그린다


def run_or_reuse(flag, name):
    if REUSE:
        return (OUT_DIR / name).read_text(encoding="utf-8")
    subprocess.run(["make", "-s", "src/main.out"], cwd=ROOT, check=True)
    return subprocess.run([str(BINARY), flag], cwd=ROOT, check=True,
                          capture_output=True, text=True).stdout


def load_rows():
    rows = list(csv.DictReader(io.StringIO(run_or_reuse("--csv", "results.csv"))))
    for row in rows:
        for key in ("n", "compares", "moves", "extraBytes", "maxDepth"):
            row[key] = int(row[key])
        row["millis"] = float(row["millis"])
    return rows


def by_algo(rows, field, keys, key_field):
    table = {}
    for algo in ALGOS:
        table[algo] = [next((r[field] for r in rows
                             if r["algo"] == algo and r[key_field] == k), 0)
                       for k in keys]
    return table


def main():
    OUT_DIR.mkdir(exist_ok=True)
    rows = load_rows()
    with open(OUT_DIR / "results.csv", "w", encoding="utf-8", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]))
        w.writeheader()
        w.writerows(rows)

    growth = [r for r in rows if r["scope"] == "growth"]
    kinds = [r for r in rows if r["scope"] == "kinds"]
    sizes = sorted({r["n"] for r in growth})
    labels = [KIND_LABEL[k] for k in KIND_KEYS]
    made = []

    # 1. n에 따른 시간 (로그-로그: 기울기 ≈ 1이면 n log n 계열)
    made.append(svgchart.line_chart(
        OUT_DIR / "growth-time-log.svg",
        "n이 커질 때 걸린 시간 — 로그-로그 축",
        "무작위 입력 · 세 정렬 모두 기울기 ≈ 1 (n log n). 차이는 상수항에서 난다",
        sizes, by_algo(growth, "millis", sizes, "n"), "n (원소 개수)", "시간 (ms)"))

    # 2. n에 따른 비교 횟수 (로그-로그: 기울기가 곧 복잡도 지수)
    made.append(svgchart.line_chart(
        OUT_DIR / "growth-compares-log.svg",
        "n이 커질 때 비교 횟수 — 로그-로그 축",
        "무작위 입력 · 비교 횟수는 시계와 달리 흔들리지 않는다. 세 선이 평행하다 = 차수가 같다",
        sizes, by_algo(growth, "compares", sizes, "n"), "n (원소 개수)", "비교 횟수"))

    # 3~5. 입력 모양별 시간 · 비교 · 이동 (n = 100,000)
    for field, unit, stem, label, fmt in (
            ("millis", "시간 (ms)", "input-shapes-time", "걸린 시간", svgchart.ms),
            ("compares", "비교 횟수", "input-shapes-compares", "비교 횟수", svgchart.si),
            ("moves", "이동 횟수", "input-shapes-moves", "이동 횟수", svgchart.si)):
        made.append(svgchart.grouped_bar_chart(
            OUT_DIR / f"{stem}.svg",
            f"입력 모양에 따른 {label}",
            "n = 100,000 · 5회 평균 · 로그 축 (중복많음에서 퀵 정렬이 다른 값보다 수십~수백 배 크다)"
            if field != "moves" else "n = 100,000 · 5회 평균",
            labels, by_algo(kinds, field, KIND_KEYS, "input"), unit,
            log_scale=(field != "moves"), value_label=fmt))

    # 재귀 깊이 (중복많음의 퀵 정렬 때문에 로그 축)
    made.append(svgchart.grouped_bar_chart(
        OUT_DIR / "input-shapes-depth.svg",
        "입력 모양에 따른 재귀 깊이 — 로그 축",
        "n = 100,000 · 힙은 반복문뿐이라 늘 1, 병합은 입력과 무관하게 18, 퀵만 입력을 탄다",
        labels, by_algo(kinds, "maxDepth", KIND_KEYS, "input"), "재귀 깊이",
        log_scale=True))

    # 6. 추가 메모리 (로그 축)
    made.append(svgchart.line_chart(
        OUT_DIR / "growth-memory-log.svg",
        "n이 커질 때 추가 메모리 — 로그-로그 축",
        "병합은 n에 비례(8n B), 퀵·힙은 원소 한 칸(8 B). 퀵의 재귀 스택은 제외",
        sizes, by_algo(growth, "extraBytes", sizes, "n"), "n (원소 개수)", "추가 메모리 (B)",
        annotate_slope=False))

    # 퀵 정렬 피벗 실험: 첫 원소 피벗 vs 랜덤 피벗
    out = run_or_reuse("--pivot", "pivot.csv")
    piv = list(csv.DictReader(io.StringIO(out)))
    with open(OUT_DIR / "pivot.csv", "w", encoding="utf-8", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(piv[0]))
        w.writeheader()
        w.writerows(piv)
    psizes = sorted({int(r["n"]) for r in piv})
    series = {}
    for mode, mlabel in (("first", "첫 원소 피벗"), ("random", "랜덤 피벗")):
        for inp, ilabel in (("sorted", "정렬됨"), ("random", "무작위")):
            series[f"{mlabel} · {ilabel}"] = [
                int(next(r["compares"] for r in piv if r["pivot"] == mode
                         and r["input"] == inp and int(r["n"]) == n))
                for n in psizes]
    made.append(svgchart.line_chart(
        OUT_DIR / "pivot-compares-log.svg",
        "퀵 정렬 피벗 선택 — 비교 횟수 (로그-로그)",
        "첫 원소 피벗 + 정렬된 입력만 기울기 2 (n(n-1)/2). 랜덤 피벗은 입력과 무관하게 n log n",
        psizes, series, "n (원소 개수)", "비교 횟수"))

    for path in made:
        print(f"wrote {Path(path).relative_to(ROOT)}")


if __name__ == "__main__":
    main()
