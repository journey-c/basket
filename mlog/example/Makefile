CXX = g++
CXXFLAGS = -std=c++11 -o2

OBJECT = example

SRC_PATH = .
BASKET_PATH = ../..
INCLUDE_PATH = -I$(BASKET_PATH)
OUT_PATH = ./output
LIB_PATH = -L$(BASKET_PATH)/mlog/lib/

MLOG = $(BASKET_PATH)/mlog/lib/libmlog.a
LIBS = -lmlog

BASE_OBJS := $(wildcard $(SRC_PATH)/*.cc)
BASE_OBJS += $(wildcard $(SRC_PATH)/*.c)
BASE_OBJS += $(wildcard $(SRC_PATH)/*.cpp)
OBJS = $(patsubst %.cc,%.o,$(BASE_OBJS))

.PHONY : clean

all : $(OBJECT)
	mkdir -p $(OUT_PATH)
	mv $(OBJECT) $(OUT_PATH)
	@echo "oh, baby, success, go, go, go ..."
	
$(OBJECT) : $(MLOG) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(INCLUDE_PATH) $(LIB_PATH) $(LIBS)

$(MLOG) :
	make -C $(BASKET_PATH)/mlog

$(OBJS) : %.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_PATH)

# clean
clean :
	rm -f $(SRC_PATH)/*.o
	rm -rf $(OUT_PATH)
	rm -rf log
	@echo "oh, baby, clean success..."

distclean :
	rm -f $(SRC_PATH)/*.o
	rm -rf $(OUT_PATH)
	rm -rf log
	make -C $(BASKET_PATH)/mlog clean
	@echo "oh, baby, distclean success..."	
