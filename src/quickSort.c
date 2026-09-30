/* 퀵 정렬 (배운 정렬 2) — Lomuto 방식 파티션 + 랜덤 피벗.
 *
 *   QuickSort(A, lo, hi)
 *       if lo >= hi then return
 *       p <- Partition(A, lo, hi)
 *       QuickSort(A, lo, p-1)
 *       QuickSort(A, p+1, hi)
 *
 * 나누기(파티션)에서 일하고 결합은 할 일이 없다. 파티션은 a[lo]를 피벗으로 삼아
 * 피벗보다 작은 것을 왼쪽으로 모은 뒤, 피벗을 제자리에 놓는다.
 *
 * 첫 원소를 그대로 피벗으로 쓰면 이미 정렬된 입력이 최악 O(n^2)이다. 그래서
 * 구간에서 무작위 위치를 골라 a[lo]와 바꾼 뒤 같은 파티션을 돌린다(랜덤 피벗).
 * 난수는 minstd 생성기(state = state * 16807 mod 2^31-1)를 직접 만들어 쓰고,
 * 시드를 고정해 몇 번을 돌려도 같은 결과가 나오게 했다(재현성).
 *
 * quickSortRandomPivot을 0으로 두면 첫 원소 피벗 버전이 된다.
 * main.c --pivot 실험이 두 버전을 견준다.
 * 피벗을 멀리 떨어진 자리와 바꾸므로 안정 정렬이 아니다.
 */
#include "sort.h"

#include <stdint.h>

#include "sortctx.h"

int quickSortRandomPivot = 1;

/* --- minstd 난수 생성기 ------------------------------------------------- */

static int64_t state = 1;

static void setSeed(int64_t seed) {
    state = seed;
}

static int64_t nextRandom(void) {
    state = state * 16807 % 2147483647;
    return state;
}

/* --- 파티션 ------------------------------------------------------------ */

static size_t partition(SortCtx *c, size_t lo, size_t hi) {
    if (quickSortRandomPivot) {
        size_t r = lo + (size_t)(nextRandom() % (int64_t)(hi - lo + 1));
        if (r != lo) {
            sortSwap(c, lo, r); /* 뽑은 피벗을 맨 앞으로 */
        }
    }
    size_t i = lo;
    for (size_t j = lo + 1; j <= hi; j++) {
        if (sortCompareAt(c, j, lo) < 0) { /* a[j] < pivot */
            i++;
            if (i != j) {
                sortSwap(c, i, j);
            }
        }
    }
    if (i != lo) {
        sortSwap(c, lo, i); /* 피벗을 제자리에 놓는다 — 이 자리는 확정 */
    }
    return i;
}

static void quickSortRange(SortCtx *c, size_t lo, size_t hi, size_t depth) {
    sortTrackDepth(c, depth);
    if (lo >= hi) {
        return;
    }
    size_t p = partition(c, lo, hi);
    if (p > lo) {
        quickSortRange(c, lo, p - 1, depth + 1); /* 피벗 왼쪽 */
    }
    quickSortRange(c, p + 1, hi, depth + 1);     /* 피벗 오른쪽 */
}

void quickSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats) {
    SortCtx c;
    if (!sortBegin(&c, base, n, size, cmp, stats)) {
        return;
    }
    setSeed(20260930); /* 시드 고정: 같은 입력이면 늘 같은 피벗 순서 */
    quickSortRange(&c, 0, n - 1, 1);
    sortEnd(&c);
}
