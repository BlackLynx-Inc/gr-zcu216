MAJOR   = 0
MINOR   = 1
RELEASE = 0

LIBNAME = libdmaproxy
SHARED_LIB_NAME = $(LIBNAME).so
SONAME  = $(SHARED_LIB_NAME).$(MAJOR)
SHARED_LIB_FULLNAME = $(SONAME).$(MINOR).$(RELEASE)

CFLAGS += "-std=c99 -O3 -pthread -fPIC"
LDFLAGS += "-lpthread"

all: shared_lib

clean:
	@rm -f *.o
	@rm -f $(SHARED_LIB_NAME)*

%.o: %.c
	$(CC) $(CLAGS) -c -o $@ $<

shared_lib: dma-proxy-lib.o
	$(CC) $(CLAGS) -shared -Wl,-soname,$(SONAME) -fPIC -o $(SHARED_LIB_FULLNAME) $<
	chmod 644 $(SHARED_LIB_FULLNAME)
	ln -sf $(SHARED_LIB_FULLNAME) $(SONAME)
	ln -sf $(SHARED_LIB_FULLNAME) $(SHARED_LIB_NAME)
	
install:
	mkdir -p $(DESTDIR)/usr/include $(DESTDIR)/usr/lib
	cp *.h $(DESTDIR)/usr/include
	cp -P *.so* $(DESTDIR)/usr/lib
