CC=gcc
SVN_REV= $(shell svnversion -n .)
CFLAGS= -g -Wall -O2 -D'SVN_REV="$(SVN_REV)"'
LIBS= 
DEPEND := makedepend
SRCS= crc.c remote_usb.c
OBJ_REMOTE_USB= crc.o remote_usb.o
DEPS=

OSNAME=$(shell uname -s)
CPUNAME=$(shell uname -m)

##
all: dep remote_usb undep

dep: $(SRCS)
	-$(DEPEND) $(CFLAGS) $(SRCS) 2> /dev/null

undep:
	-$(DEPEND) $(CFLAGS) 2> /dev/null

# binarky
remote_usb: $(OBJ_REMOTE_USB)
	$(CC) $(CFLAGS) $(LIBS) -o remote_usb $(OBJ_REMOTE_USB)

# code
%.o: %.c 
	$(CC) $(CFLAGS) -c -o $@ $<

#
clean:
	$(DEPEND) $(COPTS)
	-rm -f remote_usb $(OBJ_REMOTE_USB)

# DO NOT DELETE this line ; make dep uses this
