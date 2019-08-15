include ../include/Make.default

DEPS = utils.h

LIB_OBJ = utils.o

lib:  $(BUILD_LIB_OBJ)
	ar rvs $(BIN)/libbasic.a $(BUILD_LIB_OBJ)

all: default lib
