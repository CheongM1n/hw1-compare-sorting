/* 공통 토대 — 구현 셋이 함께 쓰는 도구와 구현 표.
 * 정렬 알고리즘 자체는 mergeSort.c · quickSort.c · heapSort.c 에 있다. */
#include "sort.h"

#include <stdlib.h>
#include <string.h>

#include "sortctx.h"

void sortStatsReset(SortStats *stats) {
    if (stats == NULL) {
        return;
    }
    stats->compares = 0;
    stats->moves = 0;
    stats->extraBytes = 0;
    stats->maxDepth = 1;
}

int sortCompareInt(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y); /* 뺄셈은 overflow 위험이 있어 쓰지 않는다 */
}

int sortBegin(SortCtx *c, void *base, size_t n, size_t size,
              SortCompare cmp, SortStats *stats) {
    sortStatsReset(stats);
    if (base == NULL || cmp == NULL || size == 0 || n < 2) {
        return 0;
    }
    c->base = (char *)base;
    c->size = size;
    c->cmp = cmp;
    c->stats = stats;
    c->tmp = (char *)malloc(size);
    if (c->tmp == NULL) {
        return 0;
    }
    sortAddExtra(c, size);
    return 1;
}

void sortEnd(SortCtx *c) {
    free(c->tmp);
    c->tmp = NULL;
}

char *sortElemAt(const SortCtx *c, size_t i) {
    return c->base + i * c->size;
}

int sortCompare(SortCtx *c, const void *a, const void *b) {
    if (c->stats != NULL) {
        c->stats->compares++;
    }
    return c->cmp(a, b);
}

int sortCompareAt(SortCtx *c, size_t i, size_t j) {
    return sortCompare(c, sortElemAt(c, i), sortElemAt(c, j));
}

void sortMove(SortCtx *c, void *dst, const void *src) {
    memcpy(dst, src, c->size);
    if (c->stats != NULL) {
        c->stats->moves++;
    }
}

void sortSwap(SortCtx *c, size_t i, size_t j) {
    sortMove(c, c->tmp, sortElemAt(c, i));
    sortMove(c, sortElemAt(c, i), sortElemAt(c, j));
    sortMove(c, sortElemAt(c, j), c->tmp);
}

void sortAddExtra(SortCtx *c, size_t bytes) {
    if (c->stats != NULL) {
        c->stats->extraBytes += bytes;
    }
}

void sortTrackDepth(SortCtx *c, size_t depth) {
    if (c->stats != NULL && depth > c->stats->maxDepth) {
        c->stats->maxDepth = depth;
    }
}

/* 정렬을 하나 더 만들면 여기에 한 줄 넣는다. main.c도 테스트도 이 표만 훑는다. */
const SortAlgorithm SORT_ALGORITHMS[] = {
    {"mergeSort", "O(n log n)", "O(n log n)", "O(n)",     1, mergeSort},
    {"quickSort", "O(n log n)", "O(n^2)",     "O(log n)", 0, quickSort},
    {"heapSort",  "O(n log n)", "O(n log n)", "O(1)",     0, heapSort},
};

const size_t SORT_ALGORITHM_COUNT = sizeof(SORT_ALGORITHMS) / sizeof(SORT_ALGORITHMS[0]);
