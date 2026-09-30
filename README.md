# hw1-compare-sorting

2026-2 고급알고리즘 과제 1 — **병합 · 퀵 · 힙 정렬 비교**.
`lec-algorithm/algorithm-env` template으로 만든 저장소입니다.

- 보고서: [report/REPORT.md](report/REPORT.md) (제출본은 PDF)
- 구현: [`src/mergeSort.c`](src/mergeSort.c) · [`src/quickSort.c`](src/quickSort.c) · [`src/heapSort.c`](src/heapSort.c)

## 돌려보기

```sh
docker compose up -d
docker compose exec lab bash
make test     # 유닛 테스트 50개
make run      # 입력 모양별 · 크기별 비교 표
make charts   # 다시 재고 report/ 아래 그래프·CSV 재생성 (python3 tools/plot.py --reuse: 그림만)
```

| 명령 | 하는 일 |
| --- | --- |
| `make run` | 비교 표 출력 |
| `make test` | 유닛 테스트 |
| `make charts` | 그래프(SVG) · `report/results.csv` 재생성 |
| `make debug` | 디버그 빌드 |
| `make clean` | 빌드 산출물 정리 |

## 구조

```plaintext
src/
├── sort.h · sortctx.h · sort.c   공통 인터페이스 · 구현 표
├── mergeSort.c                   병합 정렬 (배운 정렬)
├── quickSort.c                   퀵 정렬 (배운 정렬)
├── heapSort.c                    힙 정렬 (배우지 않은 정렬)
├── bench.h · bench.c             입력 생성 · 측정
└── main.c                        비교 표 / CSV 출력
tests/test_sort.c                 유닛 테스트 (표준 C만 사용)
tools/plot.py · svgchart.py       그래프 생성 (표준 모듈만 사용)
report/                           보고서 · 그래프 · 측정값
```

측정 구조와 `tools/svgchart.py`는 `lec-algorithm/hw1-sample-2026`을 참고·차용했습니다.
