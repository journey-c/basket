CXX = g++
CXXFLAGS = -std=c++11 -O2 -DDEBUG

OBJECT = example

SRC_DIR = .

OUTPUT = ./output

LIB_PATH = -L../lib/

LIBS = -lmlog \

INCLUDE_PATH = -I../../

.PHONY : clean

BASE_BOJS := $(wildcard $(SRC_DIR)/*.cc)
BASE_BOJS += $(wildcard $(SRC_DIR)/*.c)
BASE_BOJS += $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst %.cc,%.o,$(BASE_BOJS))

all : ${OBJECT} 	
	rm -rf $(OUTPUT)
	mkdir -p $(OUTPUT)
	mkdir -p $(OUTPUT)/bin
	cp $(OBJECT) $(OUTPUT)/bin/
	rm -rf $(OBJECT)
	@echo "Success, go, go, go..."

example : main.cc 
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDE_PATH) $(LIB_PATH) $(LIBS)

$(OBJS): %.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_PATH) ${LIB_PATH} ${LIBS}

clean :
	rm -rf $(OUTPUT)
	rm -f $(SRC_DIR)/*.o
	rm -rf $(OBJECT)
	rm -rf log
