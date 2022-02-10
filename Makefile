.PHONY: all clean test

PROG=   noprivexec

UNAME_SYS := $(shell uname -s)
ifeq ($(UNAME_SYS), Linux)
    CFLAGS ?= -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong \
              -Wformat -Werror=format-security \
              -fno-strict-aliasing
    LDFLAGS += -Wl,-z,relro,-z,now
	  NOPRIVEXEC ?= prctl
else ifeq ($(UNAME_SYS), OpenBSD)
    CFLAGS ?= -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong \
              -Wformat -Werror=format-security \
              -fno-strict-aliasing
    LDFLAGS += -Wno-missing-braces -Wl,-z,relro,-z,now
    NOPRIVEXEC ?= pledge
else ifeq ($(UNAME_SYS), FreeBSD)
    CFLAGS ?= -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong \
              -Wformat -Werror=format-security \
              -fno-strict-aliasing
    LDFLAGS += -Wno-missing-braces -Wl,-z,relro,-z,now
    NOPRIVEXEC ?= capsicum
else ifeq ($(UNAME_SYS), Darwin)
    CFLAGS ?= -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-strong \
              -Wformat -Werror=format-security \
              -fno-strict-aliasing
    LDFLAGS += -Wno-missing-braces
endif

RM ?= rm

NOPRIVEXEC ?= rlimit
NOPRIVEXEC_CFLAGS ?= -g -Wall -fwrapv -pedantic -pie -fPIE

CFLAGS += $(NOPRIVEXEC_CFLAGS) \
          -DNOPRIVEXEC=\"$(NOPRIVEXEC)\" \
          -DNOPRIVEXEC_$(NOPRIVEXEC)

LDFLAGS += $(NOPRIVEXEC_LDFLAGS)

all: $(PROG)

$(PROG):
	$(CC) $(CFLAGS) -o $@ $@.c $(LDFLAGS)

clean:
	-@$(RM) $(PROG)

test: $(PROG)
	@PATH=.:$(PATH) bats test
