include ../include/Make.default 

DEPS = 

PCL_TEST_OBJ = $(addprefix $(BIN)/, test.o)
PCL_VIEW_OBJ = $(addprefix $(BIN)/, view.o)

LOCAL_INC = $(PCL_INC) $(VTK_INC) $(3DVISION_INC) $(IMAGE_INC) $(BASIC_INC)

LOCAL_LIB = $(BOOST_LIB) $(PCL_LIB) $(VTK_LIB) $(FLANN_LIB) $(3DVISION_LIB) $(IMAGE_LIB) $(BASIC_LIB)


test:  $(PCL_TEST_OBJ)
	$(CC) -g -o $(BIN)/test $(PCL_TEST_OBJ) $(LDFLAGS)
view:  $(PCL_VIEW_OBJ)
	$(CC) -g -o $(BIN)/view $(PCL_VIEW_OBJ) $(LDFLAGS)

all: default test view
