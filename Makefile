CC = gcc
CFLAGS = -Wall -Wextra -g -I./include -lcapstone
BUILDDIR = build

all: $(BUILDDIR) bin main

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

bin: bin.c
	$(CC) $(CFLAGS) -o $(BUILDDIR)/bin bin.c

main: main.c src/core.c
	$(CC) $(CFLAGS) -o $(BUILDDIR)/main main.c src/core.c 

clean:
	rm -rf $(BUILDDIR)
