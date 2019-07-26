CXX = g++
CXXFLAGS = -std=c++11

OBJECT = echo_server

SRC_PATH = .
BASKET_PATH = ../..
INCLUDE_PATH = -I$(BASKET_PATH)
OUT_PATH = ./output
LIB_PATH = -L$(BASKET_PATH)/forward/lib/

FORWARD = $(BASKET_PATH)/forward/lib/libforward.a
LIBS = -lforward \
			 -lpthread

BASE_OBJS := $(wildcard $(SRC_PATH)/*.cc)
BASE_OBJS += $(wildcard $(SRC_PATH)/*.c)
BASE_OBJS += $(wildcard $(SRC_PATH)/*.cpp)
OBJS = $(patsubst %.cc,%.o,$(BASE_OBJS))

.PHONY : clean

all : $(OBJECT)
	mkdir -p $(OUT_PATH)/bin
	mv $(OBJECT) $(OUT_PATH)/bin
	@echo "oh, baby, success, go, go, go ..."
	
$(OBJECT) : $(FORWARD) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(INCLUDE_PATH) $(LIB_PATH) $(LIBS)

$(FORWARD) :
	make -C $(BASKET_PATH)/forward

$(OBJS) : %.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_PATH)

clean :
	rm -f $(SRC_PATH)/*.o
	rm -rf $(OUT_PATH)
	rm -f core*
	@echo "oh, baby, clean success..."

distclean :
	rm -f $(SRC_PATH)/*.o
	rm -rf $(OUT_PATH)
	rm -f core*
	make -C $(BASKET_PATH)/forward clean
	@echo "oh, baby, distclean success..."
