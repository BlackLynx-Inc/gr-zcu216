UTILS = $(basename $(wildcard *.c))
HEADERS   := $(wildcard *.h)

all: $(UTILS)

% : %.c $(HEADERS)
	echo $@
	$(CC) $< $(CFLAGS) -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(UTILS) *.o

