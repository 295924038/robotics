
#include <iostream>
#include <fstream>
#include <string>
#include "point.h"
#include "3dutils.h"
#include "rpcmodel.h"
#include "grid.h"
#include "searchdc.h"
#include "bench.h"
using namespace std;
using namespace walgo;


int main()
{
	/**
	 * Data read section
	 */
	pairmap<float> pm1;
	pairmap<float> pm2;
	if (!readDataDc("data6.csv", pm1, pm2) )
	{
		cout << "unable to read file data" << endl;
	}
	grid gr(-39.26, -10.26, 10, 12.33, 32.33, 10, 182.47, 202.47, 10);

    /**
     * Calibration section
     */
	double range = 15;
	rpcmodel model1(range), model2(range);
	model1.setData(pm1, gr);
	model1.calibrate();
	float u,v;

	model2.setData(pm2, gr);
	model2.calibrate();
	model1.objToImg(-10.26,12.33,182.47,u,v);
	cout << " u = " << u << " v = " << v << endl;
	model2.objToImg(-10.26,12.33,182.47,u,v);
	cout << " u = " << u << " v = " << v << endl;
	float x, y;
	model2.imgToObj(u,v,182.47, x, y);
	cout << "at 182.47: x = "<< x << " y = "<< y << endl;

	model2.objToImg(-30.26, 12.33, 192.47, u, v);
	model2.imgToObj(u,v,192.47, x,y);
	cout << "at 192.47: x = " << x << " y = " << y << endl;

	// find z value from two camera coordinates
	float z = searchDc(328.75,523.74, 287.66, 459.30, x, y, &model1, &model2);
	cout << "z = " << z << " should be 192.47 " << endl;
	cout << "x = " << x << " y = " << y << endl;
    z = searchDc(453.64, 532.53, 396.93, 467.01, x, y, &model1, &model2);
    cout << "z = " << z << " should be 202.47 " << endl;
    cout << "x = " << x << " y = " << y << endl;
    bench b;
    b.start();
    z = searchDc(268.60,617.50,235.03,541.52, x, y, &model1, &model2);
    b.stop();
    cout << "search took: " << b.duration() << "ms" << endl;
    cout << "z = "<< z << " should be 202.47 " << endl;
    cout << "x = " << x << " y = " << y << endl;
    b.start();
    z = searchDc(398.52,418.20,348.71,366.75, x, y, &model1, &model2);
    b.stop();
    cout << "search took: " << b.duration() << "ms" << endl;
    cout << "z = " << z << " should be 182.47 "<< endl;
    cout << "x = " << x << " y = " << y << endl;
	exit(0);
}
