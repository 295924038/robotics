include ../include/Make.default

DEPS = curve.h bezier.h spline.h bezierspline.h quaternion.h rotation.h \
       slerp.h slerpspline.h interppath.h fitplane.h fitarc.h fitellipse.h

LIB_OBJ =  rotation.o quaternion.o interppath.o fitplane.o fitarc.o fitellipse.o

BUILD_TEST_OBJ = $(addprefix $(BIN)/, testspline.o)
 
ROTATION_TEST_OBJ = $(addprefix $(BIN)/, testrotation.o)

SPLINE_TEST_OBJ = $(addprefix $(BIN)/, testspline2.o)

FITPLANE_TEST_OBJ = $(addprefix $(BIN)/, testfitplane.o)

FITARC_TEST_OBJ = $(addprefix $(BIN)/, testfitarc.o)

LOCAL_INC = $(BASIC_INC)

LOCAL_LIB = $(GEOMETRY_LIB) $(BASIC_LIB)

lib:  $(BUILD_LIB_OBJ)
	ar rvs $(BIN)/libgeometry.a $(BUILD_LIB_OBJ)

testspline: $(BUILD_TEST_OBJ) lib
	$(CC) -g -o $(BIN)/testspline $(BUILD_TEST_OBJ) $(LDFLAGS)

testspline2: $(SPLINE_TEST_OBJ) lib
	$(CC) -g -o $(BIN)/testspline2 $(SPLINE_TEST_OBJ) $(LDFLAGS)
 
testrotation: $(ROTATION_TEST_OBJ) lib
	$(CC) -g -o $(BIN)/testrotation $(ROTATION_TEST_OBJ) $(LDFLAGS)

testfitplane: $(FITPLANE_TEST_OBJ) lib
	$(CC) -g -o $(BIN)/testfitplane $(FITPLANE_TEST_OBJ) $(LDFLAGS) 

testfitarc: $(FITARC_TEST_OBJ) lib
	$(CC) -g -o $(BIN)/testfitarc $(FITARC_TEST_OBJ) $(LDFLAGS) 

all: default lib testspline testrotation testspline2 testfitplane testfitarc
