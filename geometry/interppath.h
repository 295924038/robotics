/*
 * interppath.h
 *
 *  Created on: Oct 6, 2017
 *      Author: zhian
 */

#ifndef INTERPPATH_H_
#define INTERPPATH_H_

#include "point.h"
#include <vector>
namespace walgo
{
void interpPath(const std::vector<point3dd>& pos,
			    const std::vector<point3dd >& euler,
				std::vector<point3dd>& newpos,
				std::vector<point3dd >& neweuler,
				int sampleInterval,
				int acc=2);

void interpPathScale(const std::vector<point3dd>& pos,
			    	const std::vector<point3dd >& euler,
					std::vector<point3dd>& newpos,
					std::vector<point3dd >& neweuler,
					int sampleIntervalIn,
					int sampleIntervalOut,
					int acc=2);
}
#endif /* INTERPPATH_H_ */
