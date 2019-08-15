#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//#include "port.h"
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>

#define BUFSIZE 2048

using namespace std;

int
main(int argc, char **argv)
{
	// c stuffs
	struct sockaddr_in myaddr;	/* our address */
	struct sockaddr_in remaddr;	/* remote address */
	socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
	int recvlen;			/* # bytes received */
	int fd;				/* our socket */
	int msgcnt = 0;			/* count # of messages we received */
	unsigned char buf[BUFSIZE];	/* receive buffer */


	/* create a UDP socket */

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return 0;
	}

	/* bind the socket to any valid IP address and a specific port */
	if ( argc < 4) {
		cout << "usage: " << argv[0] << " " << "<port number> <debug level> (4 print everything) <coordinate file>" << endl;
		exit(0);
	}
	int port = atoi(argv[1]);
	cout << "port = " << port << endl;
	int debug = atoi(argv[2]);
	cout << "debug level = " << debug << endl;
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		exit(0);
	}
	// reading coordinate:
	ifstream cfile(argv[3]);
	if ( !cfile ) {
		cout << "can't read coordinate file " << endl;
		exit(0);
	}
	string line;
	vector<double> xs, ys, zs, as, bs, cs;
	while ( std::getline(cfile, line) ) {
		istringstream sl(line);
		double x,y,z,a,b,c;
		sl >> x >> y >> z >> a >> b >> c;
		xs.push_back(x);
		ys.push_back(y);
		zs.push_back(z);
		as.push_back(a);
		bs.push_back(b);
		cs.push_back(c);
		line.clear();
	}

	/* now loop, receiving data and printing what we received */
	cout << "waiting on port "  <<  port << endl;
	for (;;) {
		recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		if (recvlen > 0) {
			buf[recvlen] = 0;
			if ( debug > 3 )
				cout << "received " << recvlen << " bytes. Content: "<< buf << endl;
			QXmlStreamReader xmlReader(buf);
			QString value;
			if ( xmlReader.readNextStartElement() && xmlReader.name() == "Rob") {
				if ( debug > 3 )
					cout << "Received Rob element" << endl;
				while ( xmlReader.readNextStartElement() ) {
					if ( xmlReader.name() == "DEF_RIst") {
						QString empty;
						QXmlStreamAttributes attr = xmlReader.attributes();
						double x = attr.value(empty, QString("X")).toDouble();
						double y = attr.value(empty, QString("Y")).toDouble();
						double z = attr.value(empty, QString("Z")).toDouble();
						double a = attr.value(empty, QString("A")).toDouble();
						double b = attr.value(empty, QString("B")).toDouble();
						double c = attr.value(empty, QString("C")).toDouble();
						if ( debug > 3 )
							cout << "RIST coordinates: x = " << x <<  " y = " << y << " z = " << z << " a = " << a << " b = " << b << " c = " << c << endl;
					}
				}
			}

		}
		else {
			cout << "received a zero lengh message " << endl;
			exit(0);
		}
		//sprintf(buf, "ack %d", msgcnt++);
		//cout << "sending response " << buf << endl;
                QString reply;
                QXmlStreamWriter xmlWriter(&reply);
                xmlWriter.setAutoFormatting(true);
                xmlWriter.writeStartDocument();
                QString empty;
                xmlWriter.writeStartElement(empty, "Sen");
                xmlWriter.writeAttribute("Type", "ImFree");
                xmlWriter.writeStartElement(empty, "DEF_RSol");
	        xmlWriter.writeAttribute(QString("X"),QString::number(xs[msgcnt])); 
	        xmlWriter.writeAttribute(QString("Y"),QString::number(ys[msgcnt])); 
	        xmlWriter.writeAttribute(QString("Z"),QString::number(zs[msgcnt])); 
	        xmlWriter.writeAttribute(QString("A"),QString::number(as[msgcnt])); 
	        xmlWriter.writeAttribute(QString("B"),QString::number(bs[msgcnt])); 
	        xmlWriter.writeAttribute(QString("C"),QString::number(cs[msgcnt])); 
                xmlWriter.writeEndElement();
	        xmlWriter.writeEndElement();
                xmlWriter.writeEndDocument();
                QByteArray array = reply.toLocal8Bit();
                 char* sendBuf = array.data();
                 if ( debug > 3 ) 
                    cout << "sending message with length: " << strlen(sendBuf) << endl;
		if (sendto(fd, sendBuf, strlen(sendBuf), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			perror("sendto");
	}
	/* never exits */
}
