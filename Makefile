VERSION = 1.0.5
PROJECT = randbit

TARGET_IS_WINDOWS ?= n

ifeq ($(TARGET_IS_WINDOWS),y)
	LDLIBS += -lbcrypt
	EXEC_NAME ?= randbit.exe
	CC = x86_64-w64-mingw32-gcc
else
	EXEC_NAME ?= randbit
	CC = cc
endif


STD ?= c23
SOURCES := main.c
CFLAGS_COMMON := -std=$(STD) -Wall -Wextra -Wpedantic
BUILD ?= release
LDFLAGS ?=
OUTDIR_BASE ?= build
PROJ_BASE_DIR = $(shell pwd)



#==============STYLING==============#
COLORED ?= y

ifeq ($(COLORED),y)
	ANSI_RESET   := "\033[0m"
	ANSI_BOLD    := "\033[1m"
	ANSI_FAINT   := "\033[2m"
	
	ANSI_RED     := "\033[31m"
	ANSI_GREEN   := "\033[32m"
	ANSI_YELLOW  := "\033[33m"
	ANSI_BLUE    := "\033[34m"
	ANSI_MAGENTA := "\033[35m"
	ANSI_CYAN    := "\033[36m"
	
	
	STYLE_HEADING     := $(ANSI_BOLD)$(ANSI_BLUE)
	STYLE_SUCCESS     := $(ANSI_BOLD)$(ANSI_GREEN)
	STYLE_ERR         := $(ANSI_BOLD)$(ANSI_RED)
	STYLE_WARN        := $(ANSI_BOLD)$(ANSI_YELLOW)
	STYLE_DBG         := $(ANSI_FAINT)$(ANSI_CYAN)
endif




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

.PHONY: all clean install uninstall dist

all: $(TARGET)

$(TARGET): $(SOURCES)
	@printf "%b%s%b\n"\
	   	$(STYLE_DBG) "TARGET_IS_WINDOWS=$(TARGET_IS_WINDOWS)" $(ANSI_RESET)
	@printf "%b%s%b\n" $(STYLE_DBG) "CC=$(CC)"  $(ANSI_RESET)
	mkdir -p -- $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS) $(CPPFLAGS)

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


DISTDIR = dist
LINUX_DISTDIR_NAME = randbit-$(VERSION)-linux-x86_64
WINDOWS_DISTDIR_NAME = randbit-$(VERSION)-windows-x86_64
LINUX_DISTDIR = $(DISTDIR)/$(LINUX_DISTDIR_NAME)
WINDOWS_DISTDIR = $(DISTDIR)/$(WINDOWS_DISTDIR_NAME)

dist:
	@#=================setup==============#
	
	@# ====== dir setup
	mkdir -p -- $(WINDOWS_DISTDIR) $(LINUX_DISTDIR)
	
	@# ======= debug messages
	@printf "%b%s%b\n" $(STYLE_DBG)\
	   	"DISTDIR=$(DISTDIR)" $(ANSI_RESET)
	
	@printf "%b%s%b\n" $(STYLE_DBG) \
		"LINUX_DISTDIR_NAME=$(LINUX_DISTDIR_NAME)" $(ANSI_RESET)
	
	@printf "%b%s%b\n" $(STYLE_DBG) \
		"WINDOWS_DISTDIR_NAME=$(WINDOWS_DISTDIR_NAME)" $(ANSI_RESET)
	
	@printf "%b%s%b\n" $(STYLE_DBG) \
		"LINUX_DISTDIR=$(LINUX_DISTDIR)" $(ANSI_RESET)
	
	@printf "%b%s%b\n" $(STYLE_DBG)\
	   	"WINDOWS_DISTDIR=$(WINDOWS_DISTDIR)" $(ANSI_RESET)
	
	@#===========build=========#
	
	@#========== Linux
	@printf "%b%s%b\n" $(STYLE_HEADING) "Building (Linux)..." $(ANSI_RESET)
	$(MAKE) 
	@printf "%b%s%b\n" $(STYLE_SUCCESS)\
	   	"Finished building (Linux)" $(ANSI_RESET)
	
	@#========== Windows
	
	$(MAKE) TARGET_IS_WINDOWS=y #windows
	
	@#==========Staging============#
	
	@# ======== Linux
	@printf "%b%s%b\n" $(STYLE_HEADING) "Staging (Linux)..." $(ANSI_RESET)
	cp LICENSE README.md randbit.1 $(TARGET) $(LINUX_DISTDIR)/
	@echo "%b%s%b\n" $(STYLE_SUCCESS) "Finished staging (linux)" $(ANSI_RESET)
	
	@# ======= Windows
	@printf "%b%s%b\n" $(STYLE_HEADING)\
	   	"Staging (Windows)..." $(ANSI_RESET)
	cp README.md $(TARGET).exe $(WINDOWS_DISTDIR)/
	cp randbit.1 $(WINDOWS_DISTDIR)/randbit.txt 
	cp LICENSE $(WINDOWS_DISTDIR)/LICENSE.txt
	@printf "%b%s%b\n" $(STYLE_SUCCESS)\
	   	"Finished staging (Windows)" $(ANSI_RESET)
	
	@#============Compressing============#
	
	@# ===== Linux
	@printf "%b%s%b\n" $(STYLE_HEADING) "Compressing (Linux) ..."
	cd $(DISTDIR) &&\
	   	tar -czf $(LINUX_DISTDIR_NAME).tar.gz $(LINUX_DISTDIR_NAME)
	@printf "%b%s%b\n" $(STYLE_SUCCESS) "Finished compressing (Linux)"
	
	@# ===== Windows
	@printf "%b%s%b\n" $(STYLE_HEADING)\
	   	"Compressing (Windows) ..." $(ANSI_RESET)
	cd $(DISTDIR) &&\
		zip -rl $(WINDOWS_DISTDIR_NAME).zip $(WINDOWS_DISTDIR_NAME)
	@printf "%b%s%b\n" $(STYLE_SUCCESS)\
	   	"Finished compressing (Windows)" $(ANSI_RESET)
	
	@#============END=============# 
	@printf "%b%s%b\n" $(STYLE_SUCCESS) "Done!" $(ANSI_RESET)


cleandist:
	rm -rf -- $(DISTDIR)

cleanall:
	$(MAKE) clean
	$(MAKE) cleandist


ansitest:
	@printf "%btest\n" $(ANSI_RED)
