all: rpl

.PHONY: format

format:
	astyle *.c *.h
