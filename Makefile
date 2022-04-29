.PHONY: all clean

CFLAGS = -I/usr/local/include/igraph \
         -L/usr/local/lib64 \
         -L/usr/lib64/atlas
all: smallworldness-igraph smallworldness-igraph-bran

smallworldness-igraph: smallworldness-igraph.c
	gcc -Wall $(CFLAGS) -ligraph -ltatlas -o $@ $<

smallworldness-igraph-bran: smallworldness-igraph-bran.c
	gcc -Wall $(CFLAGS) -ligraph -ltatlas -o $@ $<

test_random: test_random.c
	gcc -Wall $(CFLAGS) -ligraph -ltatlas -o $@ $<

install:
	-cp smallworldness-igraph /usr/local/bin
	-cp smallworldness-igraph-bran /usr/local/bin

clean:
	-rm smallworldness-igraph
