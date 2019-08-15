
#include <iostream>
#include <fstream>
#include <string>
#include "point.h"
#include "3dutils.h"
#include "image3d.h"
#include "upsample.h"
#include "grid.h"
#include "search.h"

using namespace std;
using namespace walgo;

int main()
{
	/**
	 * Data read section
	 */
	pairmap<float> pm;
	if (!readData("data4.csv", pm) )
	{
		cout << "unable to read file data" << endl;
	}

    /**
     * Calibration section
     */
	grid gr(155.2375, 295.2375, 10, -107.4968, -17.4968, 10, -110.3391, -20.3391, 10);
	image3d<float>* uimage;
	image3d<float>* vimage;
	createImages(pm, uimage, vimage, gr);
	int sx = gr.getSizeX();
	int sy = gr.getSizeY();
	int sz = gr.getSizeZ();
	int ratio = 50;
	image3d<float>* upuimage = new image3d<float>(sx*ratio, sy*ratio, sz*ratio);
	image3d<float>* upvimage = new image3d<float>(sx*ratio, sy*ratio, sz*ratio);

	upsample ups(ratio);
	ups.apply(*uimage, *upuimage);
	ups.apply(*vimage, *upvimage);

	upuimage->saveToFile("uimage.dat");
	upvimage->saveToFile("vimage.dat");
	gr.saveToFile("grid.dat");

	exit(0);
}
