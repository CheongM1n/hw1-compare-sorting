/* 정렬 구현끼리만 쓰는 작업 문맥. main.c·bench.c·테스트는 include하지 않는다. */
#ifndef SORTCTX_H
#define SORTCTX_H

#include <stddef.h>

#include "sort.h"

typedef struct SortCtx {
    char *base;
    size_t size;
    SortCompare cmp;
    SortStats *stats;
    char *tmp; /* 원소 한 칸짜리 임시 자리 */
} SortCtx;

/* 정렬할 것이 없으면 0을 돌려주고, 그때는 sortEnd를 부르지 않는다. */
int sortBegin(SortCtx *c, void *base, size_t n, size_t size,
              SortCompare cmp, SortStats *stats);
void sortEnd(SortCtx *c);

char *sortElemAt(const SortCtx *c, size_t i);
int sortCompare(SortCtx *c, const void *a, const void *b);
int sortCompareAt(SortCtx *c, size_t i, size_t j);
void sortMove(SortCtx *c, void *dst, const void *src);
void sortSwap(SortCtx *c, size_t i, size_t j);
void sortAddExtra(SortCtx *c, size_t bytes);
void sortTrackDepth(SortCtx *c, size_t depth);

#endif /* SORTCTX_H */
