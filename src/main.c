/* 정렬 비교 — 병합 · 퀵 · 힙.
 *
 *   make run                 사람이 읽는 비교 표
 *   ./src/main.out --csv     같은 측정을 CSV로 (tools/plot.py가 쓴다)
 *   ./src/main.out --pivot   퀵 정렬 첫 원소 피벗 vs 랜덤 피벗 (CSV)
 *
 * 부르는 쪽은 정렬 이름을 하나도 적지 않는다. 구현 표(SORT_ALGORITHMS)를 훑는다.
 * 무엇을 잴지는 아래 SPECS 한 곳에만 적는다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bench.h"
#include "sort.h"

typedef struct Spec {
    const char *scope; /* kinds: 입력 모양별 · growth: n을 키우며 */
    InputKind kind;
    size_t n;
    int reps;
} Spec;

#define KINDS_N 100000

static const Spec SPECS[] = {
    {"kinds", INPUT_RANDOM, KINDS_N, 5},
    {"kinds", INPUT_SORTED, KINDS_N, 5},
    {"kinds", INPUT_REVERSED, KINDS_N, 5},
    {"kinds", INPUT_NEARLY_SORTED, KINDS_N, 5},
    {"kinds", INPUT_FEW_UNIQUE, KINDS_N, 5},
    {"growth", INPUT_RANDOM, 1000, 20},
    {"growth", INPUT_RANDOM, 4000, 10},
    {"growth", INPUT_RANDOM, 16000, 5},
    {"growth", INPUT_RANDOM, 64000, 10},
    {"growth", INPUT_RANDOM, 256000, 5},
    {"growth", INPUT_RANDOM, 1024000, 3},
};

static const size_t SPEC_COUNT = sizeof(SPECS) / sizeof(SPECS[0]);

typedef void (*RowSink)(const Spec *spec, const BenchResult *r);

static void measureAll(RowSink sink, void (*onSpec)(const Spec *spec)) {
    for (size_t s = 0; s < SPEC_COUNT; s++) {
        const Spec *spec = &SPECS[s];
        Record *input = (Record *)malloc(spec->n * sizeof(Record));
        if (input == NULL) {
            return;
        }
        makeInput(input, spec->n, spec->kind, 20260930u);
        if (onSpec != NULL) {
            onSpec(spec);
        }
        for (size_t k = 0; k < SORT_ALGORITHM_COUNT; k++) {
            BenchResult r = benchRun(&SORT_ALGORITHMS[k], input, spec->n, spec->reps);
            sink(spec, &r);
        }
        free(input);
    }
}

/* --- 사람이 읽는 표 ---------------------------------------------------- */

#define ROW_FORMAT "%-10s %10.3f %12zu %12zu %10zu B %6zu %5s %6s\n"
#define ROW_HEADER "알고리즘     시간(ms)         비교         이동       추가메모리 깊이  정렬 안정성\n"
#define ROW_RULE   "-------------------------------------------------------------------------------\n"

static void tableRow(const Spec *spec, const BenchResult *r) {
    (void)spec;
    printf(ROW_FORMAT, r->algo->name, r->millis, r->stats.compares, r->stats.moves,
           r->stats.extraBytes, r->stats.maxDepth, r->sorted ? "yes" : "NO!",
           r->stable ? "yes" : "no");
}

static void tableSpecHeader(const Spec *spec) {
    static const char *lastScope = NULL;

    if (lastScope == NULL || strcmp(lastScope, spec->scope) != 0) {
        if (strcmp(spec->scope, "kinds") == 0) {
            printf("입력 모양별 비교 (n = %zu, %d회 평균)\n", spec->n, spec->reps);
        } else {
            printf("\nn을 키우며 (무작위 입력)\n");
        }
        lastScope = spec->scope;
    }
    if (strcmp(spec->scope, "kinds") == 0) {
        printf("\n[%s]\n", inputKindName(spec->kind));
    } else {
        printf("\n[n = %zu]\n", spec->n);
    }
    printf("%s%s", ROW_HEADER, ROW_RULE);
}

static void printDeclarations(void) {
    printf("구현 표 (SortAlgorithm이 주장하는 값)\n");
    printf("알고리즘   평균          최악          추가메모리  안정성\n");
    printf("%s", ROW_RULE);
    for (size_t k = 0; k < SORT_ALGORITHM_COUNT; k++) {
        const SortAlgorithm *a = &SORT_ALGORITHMS[k];
        printf("%-10s %-13s %-13s %-11s %s\n", a->name, a->timeComplexity,
               a->worstComplexity, a->spaceComplexity, a->stable ? "stable" : "unstable");
    }
    printf("\n");
}

static void reportTable(void) {
    printf("=== 정렬 비교: 병합 · 퀵 · 힙 ===\n");
    printf("원소는 (key, tag) %zu바이트. key로 정렬하고 tag로 안정성을 본다.\n\n",
           sizeof(Record));
    printDeclarations();
    measureAll(tableRow, tableSpecHeader);
}

/* --- 기계가 읽는 CSV --------------------------------------------------- */

static const char *inputKindKey(InputKind kind) {
    switch (kind) {
        case INPUT_RANDOM:        return "random";
        case INPUT_SORTED:        return "sorted";
        case INPUT_REVERSED:      return "reversed";
        case INPUT_FEW_UNIQUE:    return "few-unique";
        case INPUT_NEARLY_SORTED: return "nearly-sorted";
        default:                  return "unknown";
    }
}

static void csvRow(const Spec *spec, const BenchResult *r) {
    printf("%s,%s,%zu,%s,%.3f,%zu,%zu,%zu,%zu,%d,%d\n", spec->scope,
           inputKindKey(spec->kind), spec->n, r->algo->name, r->millis,
           r->stats.compares, r->stats.moves, r->stats.extraBytes,
           r->stats.maxDepth, r->sorted, r->stable);
}

static void reportCsv(void) {
    printf("scope,input,n,algo,millis,compares,moves,extraBytes,maxDepth,sorted,stable\n");
    measureAll(csvRow, NULL);
}

/* --- 퀵 정렬 피벗 실험 ------------------------------------------------ */

/* 첫 원소 피벗과 랜덤 피벗을 같은 입력에서 견준다.
 * 첫 원소 피벗은 정렬된 입력에서 O(n^2)이므로 n을 작게 잡는다. */
static void reportPivot(void) {
    static const size_t SIZES[] = {1000, 2000, 4000, 8000, 16000};
    static const InputKind KINDS[] = {INPUT_RANDOM, INPUT_SORTED};
    const SortAlgorithm *quick = NULL;
    for (size_t k = 0; k < SORT_ALGORITHM_COUNT; k++) {
        if (strcmp(SORT_ALGORITHMS[k].name, "quickSort") == 0) {
            quick = &SORT_ALGORITHMS[k];
        }
    }
    if (quick == NULL) {
        return;
    }
    const int saved = quickSortRandomPivot;
    printf("pivot,input,n,millis,compares,moves,maxDepth,sorted\n");
    for (int mode = 0; mode <= 1; mode++) {
        quickSortRandomPivot = mode;
        for (size_t t = 0; t < 2; t++) {
            for (size_t s = 0; s < sizeof(SIZES) / sizeof(SIZES[0]); s++) {
                size_t n = SIZES[s];
                Record *input = (Record *)malloc(n * sizeof(Record));
                if (input == NULL) {
                    break;
                }
                makeInput(input, n, KINDS[t], 20260930u);
                BenchResult r = benchRun(quick, input, n, 3);
                printf("%s,%s,%zu,%.3f,%zu,%zu,%zu,%d\n", mode ? "random" : "first",
                       inputKindKey(KINDS[t]), n, r.millis, r.stats.compares,
                       r.stats.moves, r.stats.maxDepth, r.sorted);
                free(input);
            }
        }
    }
    quickSortRandomPivot = saved;
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--pivot") == 0) {
        reportPivot();
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "--csv") == 0) {
        reportCsv();
        return 0;
    }
    reportTable();
    return 0;
}
