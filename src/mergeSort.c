/* 병합 정렬 (배운 정렬 1) — 하향식(top-down) 재귀.
 *
 *   ALGORITHM MergeSort(A, lo, hi)
 *       if lo >= hi then return
 *       mid <- (lo + hi) / 2
 *       MergeSort(A, lo, mid)
 *       MergeSort(A, mid+1, hi)
 *       Merge(A, lo, mid, hi)
 *
 * 나누기는 공짜이고 결합(merge)에서 일한다. 레벨마다 비용 n, 레벨 수 log n이라
 * 최선·평균·최악 모두 O(n log n)이다. temp가 n칸 필요하다(공간 O(n)).
 * merge의 '<=' 덕분에 같은 값이면 왼쪽이 먼저 나온다 — 안정 정렬.
 */
#include "sort.h"

#include <stdlib.h>

#include "sortctx.h"

/* 정렬된 a[lo..mid]와 a[mid+1..hi]를 temp에 합친 뒤 제자리로 되돌린다. */
static void merge(SortCtx *c, char *temp, size_t lo, size_t mid, size_t hi) {
    const size_t s = c->size;
    size_t i = lo;
    size_t j = mid + 1;
    size_t k = lo;

    while (i <= mid && j <= hi) {
        /* 두 묶음의 맨 앞끼리 비교해 작은 쪽을 꺼낸다. '<='라서 안정하다. */
        if (sortCompareAt(c, i, j) <= 0) {
            sortMove(c, temp + k++ * s, sortElemAt(c, i++));
        } else {
            sortMove(c, temp + k++ * s, sortElemAt(c, j++));
        }
    }
    /* 한쪽이 바닥나면 남은 쪽은 비교 없이 부어 넣는다. */
    while (i <= mid) {
        sortMove(c, temp + k++ * s, sortElemAt(c, i++));
    }
    while (j <= hi) {
        sortMove(c, temp + k++ * s, sortElemAt(c, j++));
    }
    for (k = lo; k <= hi; k++) {
        sortMove(c, sortElemAt(c, k), temp + k * s);
    }
}

static void mergeSortRange(SortCtx *c, char *temp, size_t lo, size_t hi, size_t depth) {
    sortTrackDepth(c, depth);
    if (lo >= hi) {
        return; /* 원소 하나면 이미 정렬 */
    }
    size_t mid = lo + (hi - lo) / 2;
    mergeSortRange(c, temp, lo, mid, depth + 1);
    mergeSortRange(c, temp, mid + 1, hi, depth + 1);
    merge(c, temp, lo, mid, hi);
}

void mergeSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats) {
    SortCtx c;
    if (!sortBegin(&c, base, n, size, cmp, stats)) {
        return;
    }
    char *temp = (char *)malloc(n * size);
    if (temp != NULL) {
        sortAddExtra(&c, n * size);
        mergeSortRange(&c, temp, 0, n - 1, 1);
        free(temp);
    }
    sortEnd(&c);
}
