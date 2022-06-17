CC=g++
SRC=Container.cpp
BJS=$(SRC:.cpp=.o)
TARGETS=container

TAR=tar
TARFLAGS=-cvf
TARNAME=ex5.tar
TARSRCS=$(SRC)Makefile README

all:$(TARGETS)

container: Container.cpp Container.h
	$(CC) $^ -lstdc++fs -o container
clean:
	$(RM)$(TARGETS)*~ *core
tar:
	$(TAR)$(TARFLAGS)$(TARNAME)$(TARSRCS)
