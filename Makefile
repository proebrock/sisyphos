# set to yes/no for debug/shipping version
DEBUG  = no

CC     = gcc
DEPCC  = gcc -M
LEX    = flex
YACC   = bison -y --locations
SHELL  = /bin/sh

DEPDIR = dep
OBJDIR = obj
MODS   = board chess cpc cpc_scanner cpc_parser globals lut main move position \
         rotate search thread wrapper
DEPS   = $(addprefix $(DEPDIR)/,$(addsuffix .d,$(MODS)))
OBJS   = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(MODS)))
PROG   = sisyphos
TMP    = cpc_scanner.c cpc_parser.c cpc_parser.h tags

CFLAGS = -Wall -W -pedantic -Wno-long-long #-ansi -DARCH_COMMON
LFLAGS = -lpthread
LIBS   =

ifeq ($(DEBUG),yes)
CFLAGS += -O0 -ggdb -UNDEBUG
LFLAGS += -ggdb
else
CFLAGS += -O3 -DNDEBUG
LFLAGS += -s
endif



.PHONY: all
all: $(PROG)


# compile and link program

$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LFLAGS) $(LIBS)

# prevent targets "clean" and "distclean" creating dependencies before cleaning project
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include $(DEPS)
endif
endif


# create parser

cpc_parser.c cpc_parser.h: cpc_parser.y
	$(YACC) -d -o cpc_parser.c cpc_parser.y

cpc_scanner.c: cpc_scanner.l
	$(LEX) -o$@ cpc_scanner.l


# rule for compiling objects

$(OBJDIR)/%.o: %.c
	mkdir -p $(OBJDIR)
	$(CC) -o $@ -c $(CFLAGS) $<

# rule for creating dependencies

$(DEPDIR)/%.d: %.c
	mkdir -p $(DEPDIR)
	$(DEPCC) $(CFLAGS) $< | sed 's|$*.o|$(OBJDIR)/& $@|g' > $@


# misc targets

.PHONY: tags
tags:
	ctags -R -o tags --totals=yes .

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS) $(TMP)

.PHONY: distclean
distclean: clean
	rm -rf $(OBJDIR) $(DEPDIR)

