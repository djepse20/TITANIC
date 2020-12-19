GIT_COMMIT_HASH := "$(shell git describe --dirty --always --tags)"

all:
	gcc -DREVISION=\"$(GIT_COMMIT_HASH)\" -g -Wall -std=c99 -pedantic csv_parser/csv.c csv_parser/StrDup.c TimeSentinel.c userinteraction.c
