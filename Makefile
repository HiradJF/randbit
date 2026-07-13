TARGET_IS_WINDOWS ?= n

ifeq ($(TARGET_IS_WINDOWS),y)
	LDLIBS += -lbcrypt
	EXEC_NAME ?= randbit.exe
	CC ?= x86_64-w64-mingw32-gcc
else
	EXEC_NAME ?= randbit
	CC ?= cc
endif


STD ?= c23
SOURCES := main.c
CFLAGS_COMMON := -std=$(STD) -Wall -Wextra -Wpedantic
BUILD ?= release
LDFLAGS ?=
OUTDIR_BASE ?= build

ifeq ($(BUILD),debug)
    BUILD_CFLAGS := -O0 -g
else ifeq ($(BUILD),release)
    BUILD_CFLAGS := -O2 -DNDEBUG
else ifeq ($(BUILD),min_size_rel)
    BUILD_CFLAGS := -Os -DNDEBUG
else
    $(error Unknown BUILD '$(BUILD)')
endif

OUTDIR := $(OUTDIR_BASE)/$(BUILD)
TARGET := $(OUTDIR)/$(EXEC_NAME)



CFLAGS += $(CFLAGS_COMMON) $(BUILD_CFLAGS)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p -- $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -rf $(OUTDIR_BASE) 



PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
MANDIR := $(PREFIX)/share/man
LICENSEDIR := $(PREFIX)/share/licenses/randbit

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/$(EXEC_NAME)
	install -Dm644 randbit.1 $(DESTDIR)$(MANDIR)/man1/randbit.1
	install -Dm644 LICENSE $(DESTDIR)$(LICENSEDIR)/LICENSE

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(EXEC_NAME)
	rm -f $(DESTDIR)$(MANDIR)/man1/randbit.1
	rm -f $(DESTDIR)$(LICENSEDIR)/LICENSE
