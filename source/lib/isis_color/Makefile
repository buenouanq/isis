#######################################################################
# Makefile template for Isis libraries
#
# Warning: In order for this Makefile to work, Isis must be included in
# your environment.  See the Isis web site under "Getting started" for info.
#
# To create a library:
#
# 1. Copy this template to your source directory and call it "Makefile".
# 2. Edit the Makefile appropriately (see instructions below).
# 3. Use one of the following commands:
#    "make" will compile the source files and create the library.
#    "make install" will make the library and install it and its headers.
#    "make clean" will clear out .o files and other garbage (to start over).
#    "make clean install" will do all of this from scratch.  
#
# Be sure to put a backslash '\' at the end of lines if you run over
# one line for any entry.

#######################################################################
# Name your library here.  If your library is a collection of isis
# interface functions, please prefix your library name with "isis_".
# If your library is external to isis and could be used stand-alone,
# you do not have to use this prefix.

LIBNAME = isis_color

#######################################################################
# List all your source (.c) files here: 

SRCS = isis_color.c isis_color_br.c isis_color_const.c isis_color_yiq.c

#######################################################################
# List header (.h) files that should be installed here:

HEADERS = isis_color.h

#######################################################################
# Enter the directories in which to install the library and headers.
# Typically these are $(ISIS_ROOT)/include and $(ISIS_ROOT)/lib,
# but you can change these to private directories if you wish.

INCDIR = $(ISIS_ROOT)/include

LIBDIR = $(ISIS_ROOT)/lib

#######################################################################
# If you access header files from other locations, add -I<directory>
# to the INCLUDES list.  To add extra definitions, use -D<variable>.

INCLUDES = -I$(ISIS_ROOT)/include
DEFINES = 

#######################################################################
# Do not modify the following lines

include $(ISIS_ROOT)/misc/Makefile.rules.lib
