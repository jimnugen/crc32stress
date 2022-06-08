CC=/data/home/jim/.HappyMake/cache/dependencies/newdb/gcc-10.2.1+git583-1.3.4.sap20201215-linuxppc64le-linuxppc64le/bin/gcc
CXX=/data/home/jim/.HappyMake/cache/dependencies/newdb/gcc-10.2.1+git583-1.3.4.sap20201215-linuxppc64le-linuxppc64le/bin/g++
AS=/data/home/jim/.HappyMake/cache/dependencies/newdb/binutils-2.32.0.sap20190319-linuxppc64le-linuxppc64le/bin-gold/as

CFLAGS+=-m64 -g -O2 -mcpu=power8 -mcrypto -mpower8-vector -maltivec -mvsx -Wall
ASFLAGS=-m64 -g -maltivec
LDFLAGS=-m64 -g

SHELL=/bin/bash

SOURCEDIR_BENCHIT = benchit
SOURCEDIR_CRC32_VPMSUM = crc32-vpmsum
BUILDDIR = build

#EXECUTABLES = benchit_vpmsum_as
#SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
#OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

all: dir $(BUILDDIR)/benchit_vpmsum_as

dir:
	mkdir -p $(BUILDDIR)

crc32-vpmsum: $(SOURCEDIR_CRC32_VPMSUM)/crc32_wrapper.o $(SOURCEDIR_CRC32_VPMSUM)/crc32.o
	cd crc32-vpmsum && make crc32_wrapper.o crc32.o

$(BUILDDIR)/benchit.o: $(SOURCEDIR_BENCHIT)/benchit.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/benchit_vpmsum_as: $(BUILDDIR)/benchit.o $(SOURCEDIR_CRC32_VPMSUM)/crc32.o $(SOURCEDIR_CRC32_VPMSUM)/crc32_wrapper.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(BUILDDIR)/* $(SOURCEDIR_CRC32_VPMSUM)/*.o
