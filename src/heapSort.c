/* 힙 정렬 (배우지 않은 정렬) — 최대 힙을 이용한 제자리 정렬.
 *
 * 배열 자체를 완전 이진 트리로 본다. 0번부터 쓰면 i의 자식은 2i+1, 2i+2다.
 *  1단계(heapify): 아래쪽 내부 노드부터 거꾸로 sift-down 해서 최대 힙을 만든다.
 *                  이 단계는 O(n)이다.
 *  2단계(정렬):    루트(최댓값)를 배열 끝과 바꾸고 힙 크기를 1 줄인 뒤, 새 루트를
 *                  sift-down 한다. 이걸 n-1번 반복하면 뒤에서부터 큰 값이 쌓인다.
 * 입력과 상관없이 O(n log n)이고, 추가 메모리는 원소 한 칸뿐이다.
 * 루트와 끝을 바꾸는 과정에서 같은 값의 순서가 섞이므로 안정 정렬이 아니다.
 */
#include "sort.h"

#include "sortctx.h"

/* a[root]를 크기 n인 힙 안에서 제자리까지 내려보낸다.
 * 매번 교환하지 않고 root 원소를 tmp에 들고 구멍을 내리는 방식이라
 * 이동이 교환 방식(3회)의 1/3 수준이다. */
static void siftDown(SortCtx *c, size_t root, size_t n) {
    sortMove(c, c->tmp, sortElemAt(c, root));
    size_t hole = root;
    for (;;) {
        size_t child = 2 * hole + 1;
        if (child >= n) {
            break;
        }
        /* 두 자식 중 큰 쪽을 고른다. */
        if (child + 1 < n && sortCompareAt(c, child, child + 1) < 0) {
            child++;
        }
        /* 들고 있는 값이 큰 자식보다 작지 않으면 여기가 제자리다. */
        if (sortCompare(c, sortElemAt(c, child), c->tmp) <= 0) {
            break;
        }
        sortMove(c, sortElemAt(c, hole), sortElemAt(c, child));
        hole = child;
    }
    sortMove(c, sortElemAt(c, hole), c->tmp);
}

void heapSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats) {
    SortCtx c;
    if (!sortBegin(&c, base, n, size, cmp, stats)) {
        return;
    }
    /* 1단계: 마지막 내부 노드(n/2 - 1)부터 루트까지 거꾸로 */
    for (size_t i = n / 2; i-- > 0;) {
        siftDown(&c, i, n);
    }
    /* 2단계: 최댓값을 끝으로 보내고 힙을 줄인다 */
    for (size_t end = n - 1; end > 0; end--) {
        sortSwap(&c, 0, end);
        siftDown(&c, 0, end);
    }
    sortEnd(&c);
}
