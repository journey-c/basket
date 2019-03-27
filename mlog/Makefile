CXX = g++
CXXFLAGS = -std=c++11 -O2 -lpthread

SRC_DIR = ./src

LIB_DIR = ./lib

LIBS = -lpthread

INCLUDE_PATH = -I../

LIBRARY = libmlog.a

BASE_OBJS := $(wildcard $(SRC_DIR)/*.cc)
BASE_OBJS += $(wildcard $(SRC_DIR)/*.c)
BASE_OBJS += $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst %.cc,%.o,$(BASE_OBJS))

.PHONY : clean

all : ${LIBRARY}
	rm -rf ${LIB_DIR}
	mkdir ${LIB_DIR}
	mv ${LIBRARY} ${LIB_DIR}
	@echo "Success, go, go, go..."

$(LIBRARY): $(OBJS)
	rm -rf $(LIBRARY)
	ar -rcs $(LIBRARY) $(OBJS)

$(OBJS): %.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_PATH) ${LIBS}

clean : 
	rm -rf $(SRC_DIR)/*.o
	rm -rf $(LIB_DIR)

