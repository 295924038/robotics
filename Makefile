clean:
	(cd basic; make clean)
	(cd image; make clean)
	(cd geometry; make clean)
	(cd 3dvision; make clean)

all:
	(cd basic; make all)
	(cd image; make all)
	(cd 3dvision; make all)
	(cd geometry; make all)

release:   clean all
	rm -rf release
	mkdir -p release/lib release/include
	cp basic/bin/libbasic.a geometry/bin/libgeometry.a 3dvision/bin/lib3dvision.a image/bin/libimage.a release/lib
	cp basic/*.h image/*.h geometry/*.h 3dvision/*.h release/include	
 	
