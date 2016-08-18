#######################################################################
# Makefile template for Isis applications
#
# Warning: In order for this Makefile to work, Isis must be included in
# your environment.  See the Isis web site under "Getting started" for info.
#
# To create an application:
#
# 1. Copy this template to your source directory and call it "Makefile".
# 2. Edit the Makefile appropriately (see instructions below).
# 3. Use one of the following commands:
#    "make" will compile the source files and create the program.
#    "make install" will make the program and install it.
#    "make clean" will clear out .o files and other garbage (to start over).
#    "make clean install" will do all of this from scratch.  
#
# Be sure to put a backslash '\' at the end of lines if you run over
# one line for any entry.

#######################################################################
# Name your program here: 

PROGNAME = isis

#######################################################################
# List all your source (.c) files here: 

SRCS = main.c

#######################################################################
# Enter the directory in which to install the program here.
# Typically $(ISIS_ROOT)/bin, but you could enter a private directory.

BINDIR = $(ISIS_ROOT)/bin

#######################################################################
# Specify libraries to link to here.  If you access libraries from any
# other directories besides those specified in LIBDIRS, add
# -L<directory> to that list.  Then, list all the libraries in the
# form -l<libname> in the LIBS list.  Order matters.  For example, if
# library X uses stuff in library Y, then library X should appear
# before library Y in this list.  (-lisis and -lm are necessary for
# Isis interpreters and should be near the end of the list.)

LIBDIRS = -L$(ISIS_ROOT)/lib -L/usr/X11R6/lib -L/usr/local/lib

LIBS =  -lisis_unix -lisis_improc -lisis_seg -lisis_color -lisis_stats \
	-lisis_gif -lisis_jpeg -lisis_xwin -lisis_readline \
	-lisis_gl12 -lisis_glu13 -lisis_glut -lisis_glx \
	-lisis_alsa -lisis_v4l2 \
	-lisis -lreadline -lhistory -lcurses  \
	-lasound -lGL -lGLU -lglut -ljpeg -lX11 -lpthread -lm

# -lisis_sdl -lSDL -lisis_cmuface -ldetect 

#######################################################################
# If you access header files from other locations, add -I<directory>
# to the INCLUDES list.  To add extra definitions, use -D<variable>.

INCLUDES = -I$(ISIS_ROOT)/include
DEFINES = 

#######################################################################
# Do not modify the following lines

include $(ISIS_ROOT)/misc/Makefile.rules.app
