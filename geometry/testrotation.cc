/*
 * testrotation.cc
 *
 *  Created on: Apr 8, 2017
 *      Author: zhian
 */
#include <iostream>
#include "rotation.h"
using namespace std;
using namespace walgo;

int main()
{
     //  姿态一
	double A = 169;
	double B = 50;
	double C = 60;
	Rotation::EULERTYPE t =  static_cast<Rotation::EULERTYPE>(323);
	Rotation r1(A,B,C, false, t, true);
	//定义旋转轴为yz方向, 以及绕该轴60度的旋转
	//注意nx,ny,nz 要组成三维单位矢量
   	double nx = 0;
   	double ny = 1.0*cos(PI/4.0);
   	double nz = 1.0*cos(PI/4.0);
	Rotation r2(PI/3.0, nx, ny, nz);
	// r3 为旋转后的姿态
	Rotation r3 = r2.times(r1);
	r3.getEulerAngles(A, B, C, true, t, true);

	//接设r1为坐标2相对与坐标1的转动， r2为相机在坐标2的姿态， 则相机相对与坐标1的姿态为 r2.times(r1).  与以上计算方法相同。

}
