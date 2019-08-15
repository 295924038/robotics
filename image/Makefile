include ../include/Make.default

DEPS = detectors.h filtergen.h linemodel.h linefeature.h lineanalysis.h \
	seamdetector.h lineseamdetector.h enddetector.h fitpoly.h \
	abslinemodel.h bsplinemodel.h d2seamdetector.h pathdetector.h \
	findpeaks.h histimage.h getxuva.h model1d.h polymodel1d.h sepmodel3d.h 

LIB_OBJ = detectors.o filtergen.o linefeature.o seamdetector.o \
	lineanalysis.o linemodel.o contourseamdetector.o \
        lineseamdetector.o detectorsavx2.o enddetector.o \
	testfindseam.o fitpoly.o bsplinemodel.o \
        d2seamdetector.o pathdetector.o abslinemodel.o \
	findpeaks.o histimage.o getxuva.o model1d.o polymodel1d.o \
	sepmodel3d.o getxuvv.o getxuvu.o getxuvw.o

BUILD_SMOOTH_OBJ = $(addprefix $(BIN)/, smooth.o)	

THINNING_OBJ = $(addprefix $(BIN)/, testthinning.o)

CIRCLE_OBJ = $(addprefix $(BIN)/, testcircle.o)

SHOWIMAGE_OBJ = $(addprefix $(BIN)/, showimage.o)

TESTCONTOUR_OBJ = $(addprefix $(BIN)/, testcontour.o)

TESTLINE_OBJ = $(addprefix $(BIN)/, testline.o)

TESTFINDSEAM_OBJ = $(addprefix $(BIN)/, testfindseam.o)

TESTSHORTSEAM_OBJ = $(addprefix $(BIN)/, testshortseam.o)

TESTENDDETECTOR_OBJ = $(addprefix $(BIN)/, testenddetector.o)

TESTFITPOLY_OBJ = $(addprefix $(BIN)/, testfitpoly.o)

LOCAL_INC = $(OPENCV_INC) $(BASIC_INC) $(3DVISION_INC)

LOCAL_LIB = $(IMAGE_LIB) $(BASIC_LIB) $(OPENCV_LIB)
lib:  $(BUILD_LIB_OBJ)
	ar rvs $(BIN)/libimage.a $(BUILD_LIB_OBJ)

smooth:  $(BUILD_SMOOTH_OBJ) lib
	$(CC) -g -o $(BIN)/smooth $(BUILD_SMOOTH_OBJ) $(LDFLAGS)

testthinning:  $(THINNING_OBJ) lib
	$(CC) -g -o $(BIN)/testthinning $(THINNING_OBJ) $(LDFLAGS)

testcircle:    $(CIRCLE_OBJ) lib
	$(CC) -g -o $(BIN)/testcircle $(CIRCLE_OBJ) $(LDFLAGS)
	
showimage:    $(SHOWIMAGE_OBJ) lib
	$(CC) -g -o $(BIN)/showimage $(SHOWIMAGE_OBJ) $(LDFLAGS)
	
testcontour:    $(TESTCONTOUR_OBJ) lib 
	$(CC) -g -o $(BIN)/testcontour $(TESTCONTOUR_OBJ) $(LDFLAGS)
	
testline:    $(TESTLINE_OBJ) lib
	$(CC) -g -o $(BIN)/testline $(TESTLINE_OBJ) $(LDFLAGS)

testfitpoly:    $(TESTFITPOLY_OBJ) lib
	$(CC) -g -o $(BIN)/testfitpoly $(TESTFITPOLY_OBJ) $(LDFLAGS)

testfindseam:    $(TESTFINDSEAM_OBJ) lib
	$(CC) -g -o $(BIN)/testfindseam $(TESTFINDSEAM_OBJ) $(LDFLAGS)
	
testshortseam:    $(TESTSHORTSEAM_OBJ) lib
	$(CC) -g -o $(BIN)/testshortseam $(TESTSHORTSEAM_OBJ) $(LDFLAGS)
	
testenddetector:    $(TESTENDDETECTOR_OBJ) lib
	$(CC) -g -o $(BIN)/testenddetector $(TESTENDDETECTOR_OBJ) $(LDFLAGS)
		
all: default lib smooth testfindseam testshortseam testfitpoly testthinning showimage testcontour testline testenddetector
