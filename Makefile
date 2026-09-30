# 빌드와 테스트를 한 단어로 돌리기 위한 Makefile.
# 컨테이너 안에서 실행한다 (docker compose exec lab bash).
#
#   make run     정렬 비교 표 출력
#   make test    유닛 테스트
#   make charts  비교 그래프(SVG)와 results.csv를 report/ 아래에 다시 만든다
#   make debug   디버그 심볼을 넣어 빌드 (VS Code의 F5가 쓴다)
#   make clean   빌드 산출물 정리
#
# 실행 파일은 `*.out`으로 만든다. .gitignore가 그것만 걸러낸다.

CC ?= gcc
CFLAGS ?= -std=c17 -Wall -Wextra -O2
DEBUGFLAGS ?= -std=c17 -Wall -Wextra -g -O0

.PHONY: all run run-c test test-c charts debug clean

all: test

run: run-c

run-c: src/main.out
	@./src/main.out

test: test-c

test-c: tests/test_sort.out
	@./tests/test_sort.out

charts: src/main.out
	@python3 tools/plot.py

debug: src/main.debug.out

%.out: %.c
	$(CC) $(CFLAGS) -I$(@D) -o $@ $(wildcard $(@D)/*.c)

%.debug.out: %.c
	$(CC) $(DEBUGFLAGS) -I$(@D) -o $@ $(wildcard $(@D)/*.c)

SORT_SRC = src/sort.c src/mergeSort.c src/quickSort.c src/heapSort.c

src/main.out: src/main.c $(SORT_SRC) src/bench.c src/sort.h src/sortctx.h src/bench.h
	$(CC) $(CFLAGS) -Isrc -o $@ src/main.c $(SORT_SRC) src/bench.c

tests/test_sort.out: tests/test_sort.c $(SORT_SRC) src/sort.h src/sortctx.h src/bench.c src/bench.h
	$(CC) $(CFLAGS) -Isrc -o $@ tests/test_sort.c $(SORT_SRC) src/bench.c

clean:
	rm -f src/*.out tests/*.out
