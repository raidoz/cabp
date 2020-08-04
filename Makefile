# Makefile

ROOT_DIR := .
BIN_DIR := $(ROOT_DIR)/bin
LIB_DIR := /opt/prolib
LOGFILE := $(BIN_DIR)/make.log

H_INCLUDE_PATH = -I $(ROOT_DIR)
H_INCLUDE_PATH = -I /usr/lib/jvm/java-6-openjdk/include/

CXX = g++
CFLAGS = -Wall -fPIC $(OPT)
COMPILE = $(CXX) $(CFLAGS) $(H_INCLUDE_PATH) -c

SOURCES1 := $(wildcard $(ROOT_DIR)/*.cpp) # All .cpp files are sources
objects1 := $(patsubst %.cpp, %.o, $(SOURCES1)) # All .cpp files have corresponding objects

all: Algo

test:
	@echo $(objects1) >> $(LOGFILE)

clean:
	-rm -f $(BIN_DIR)/*.o
	-rm -f $(BIN_DIR)/simple
	-rm -f $(BIN_DIR)/CABPFromFile
	-rm -f $(LOGFILE)

bdir:
	mkdir -p $(BIN_DIR)

simple: main.o Algo
	$(CXX) -o $(BIN_DIR)/simple $(BIN_DIR)/*.o

CABPFromFile: FromFileMain.o Algo
	$(CXX) -o $(BIN_DIR)/CABPFromFile $(BIN_DIR)/*.o

Algo: NodeSet.o PointSet.o CABP.o

library: Algo CABPproxy.o
	$(CXX) -shared -Wl,-soname,libCABP.so.2 $(BIN_DIR)/*.o -o $(BIN_DIR)/libCABP.so.2

package:
	dpkg-buildpackage -b --no-sign

jnilibrary: Algo core_CABPProxy.o cabpproxy.o
	$(CXX) -shared -Wl,-soname,$(LIB_DIR)/libCABPjni.so $(BIN_DIR)/*.o -o $(LIB_DIR)/libCABPjni.so

%.o : %.cpp bdir
	$(COMPILE)  $< -o $(BIN_DIR)/$@
