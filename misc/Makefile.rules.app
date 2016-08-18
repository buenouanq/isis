# Isis application Makefile rules
# Stefan Agamanolis

# The including Makefile defines the following:
# PROGNAME, SRCS, LIBS, LIBDIRS, BINDIR, INCLUDES, DEFINES

# standard settings

SHELL = /bin/sh
LD = ld
RM = rm -f
CC = cc
CP = cp

# Note the reason that all SRCS are put on the same command line
# is to provide the ability to make inter-file optimizations

$(PROGNAME): $(SRCS)
	$(RM) $@
	$(CC) -o $@ -O3 $(INCLUDES) $(DEFINES) \
		$(SRCS) $(LIBDIRS) $(LIBS)

all:: $(PROGNAME)

clean::
	$(RM) $(PROGNAME) *.o *.a core *~ .emacs_* 

install:: $(PROGNAME)
	$(CP) $(PROGNAME) $(BINDIR)

