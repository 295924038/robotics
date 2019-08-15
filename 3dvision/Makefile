include ../include/Make.default 

DEPS = array2d.h image.h point.h image3d.h grid.h search.h rpcmodel.h model.h lasermodel.h lasermodel2.h lasermodel3.h

BUILD_CALIB_OBJ = $(addprefix $(BIN)/, calibrate.o)

BUILD_VISION_OBJ = $(addprefix $(BIN)/, vision.o)

BUILD_CALIBDC_OBJ = $(addprefix $(BIN)/, calibratedc.o)

BUILD_CALIBLASER_OBJ = $(addprefix $(BIN)/, calibratelaser.o)

BUILD_CALIBLASER3_OBJ = $(addprefix $(BIN)/, calibratelaser3.o)

LIB_OBJ = 3dutils.o upsample.o upsamplepoly.o search.o grid.o rpcmodel.o searchdc.o lasermodel.o lasermodel2.o lasermodel3.o

LOCAL_INC = $(3DVISION_INC) $(IMAGE_INC) $(BASIC_INC)

LOCAL_LIB = $(3DVISION_LIB) $(IMAGE_LIB) $(BASIC_LIB)


vision: $(BUILD_VISION_OBJ) lib
	$(CC) -g -o $(BIN)/vision $(BUILD_VISION_OBJ) $(LDFLAGS)

calibrate: $(BUILD_CALIB_OBJ) lib
	$(CC) -g -o $(BIN)/calibrate $(BUILD_CALIB_OBJ) $(LDFLAGS)

calibratedc: $(BUILD_CALIBDC_OBJ) lib
	$(CC) -g -o $(BIN)/calibratedc $(BUILD_CALIBDC_OBJ) $(LDFLAGS)

calibratelaser: $(BUILD_CALIBLASER_OBJ) lib
	$(CC) -g -o $(BIN)/calibratelaser $(BUILD_CALIBLASER_OBJ) $(LDFLAGS)

calibratelaser3: $(BUILD_CALIBLASER3_OBJ) lib
	$(CC) -g -o $(BIN)/calibratelaser3 $(BUILD_CALIBLASER3_OBJ) $(LDFLAGS)


lib:  $(BUILD_LIB_OBJ)
	ar rvs $(BIN)/lib3dvision.a $(BUILD_LIB_OBJ)

all: default lib calibrate calibratedc calibratelaser calibratelaser3 vision
