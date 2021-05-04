# Makefile example. Compile all .c files and build execution file
QUIET = @
ECHO  = echo
RM = rm -rf

CC := g++
CINC = -I.
GCC_GXX_WARNINGS = -Wall -Wno-error -Wno-packed -Wpointer-arith -Wredundant-decls -Wstrict-aliasing=3 -Wswitch-enum -Wundef -Wwrite-strings -Wextra -Wno-unused-parameter \

CFLAGS  = -Os
LDFLAGS = -lm
OBJS  = $(patsubst %.cpp,%.o,$(wildcard $(PWD)/*.cpp))

all: default

centos: osbuild_centos default

osbuild_centos:
	$(eval OS_BUILD = _CENTOS_BUILD_)

ubuntu: osbuild_ubuntu default

osbuild_ubuntu:
	$(eval OS_BUILD = _UBUNTU_BUILD_)

default: $(OBJS)
	@echo "###### Compile $(OBJS) done!! ######"

%.o: %.cpp
	$(QUIET)$(ECHO) "Compliling " $<

	@echo "$(QUIET)$(CC) $(CFLAGS) -DLINUX $(GCC_GXX_WARNINGS) -c -O2 $(CINC) $< -o $@"
	$(QUIET)$(CC) $(CFLAGS) -DLINUX $(GCC_GXX_WARNINGS) -c -O2 $(CINC) $< -o $@

clean:
	$(QUIET)$(RM) $(OBJS)
