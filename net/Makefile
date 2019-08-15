CXX=g++ 
CXXFLAGS = -g -fpermissive -fPIC \
	    -I/usr/include/x86_64-linux-gnu/qt5
LIBS = -L/usr/lib/x86_64-linux-gnu -lQt5Xml -lQt5Core
all: client server

server: udpserver.o 
	$(CXX) -o udpserver udpserver.o $(LIBS)

client: udpclient.o 
	$(CXX) -o udpclient udpclient.o $(LIBS)

udpclient.o: udpclient.cc 

udpserver.o: udpserver.cc

clean:
	rm -f udpserver udpclient udpclient.o udpserver.o 
