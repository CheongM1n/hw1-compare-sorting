/* 정렬 비교 과제 — 병합 · 퀵 · 힙 정렬을 하나의 공통 인터페이스로 묶는다.
 *
 * C에는 interface가 없으므로 함수 포인터를 담은 구조체를 쓴다.
 * 비교 함수 규약은 표준 라이브러리 qsort와 같다.
 */
#ifndef SORT_H
#define SORT_H

#include <stddef.h>

/* a<b면 음수, a==b면 0, a>b면 양수. */
typedef int (*SortCompare)(const void *a, const void *b);

/* 정렬 한 번 동안 모인 측정값. 시계와 무관하게 재현되는 값만 담는다. */
typedef struct SortStats {
    size_t compares;   /* 비교 함수 호출 횟수 */
    size_t moves;      /* 원소 복사 횟수 (교환 한 번 = 3) */
    size_t extraBytes; /* 입력 배열 밖에 잡은 작업 공간(바이트) */
    size_t maxDepth;   /* 재귀 깊이 최댓값. 반복문만 쓰면 1 */
} SortStats;

typedef struct SortAlgorithm {
    const char *name;
    const char *timeComplexity;  /* 평균 */
    const char *worstComplexity; /* 최악 */
    const char *spaceComplexity; /* 추가 메모리 */
    int stable;                  /* 안정 정렬이라고 주장하는 값 (테스트가 실측과 대조) */
    void (*sort)(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats);
} SortAlgorithm;

void mergeSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats);
void quickSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats);
void heapSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats);

/* 1이면 랜덤 피벗(기본), 0이면 첫 원소 피벗. --pivot 실험용. */
extern int quickSortRandomPivot;

extern const SortAlgorithm SORT_ALGORITHMS[];
extern const size_t SORT_ALGORITHM_COUNT;

void sortStatsReset(SortStats *stats);
int sortCompareInt(const void *a, const void *b);

#endif /* SORT_H */
