/*
 * bench.h
 *
 *  Created on: Apr 6, 2016
 *      Author: zhian
 */

#ifndef BENCH_H_
#define BENCH_H_
#include <cstdlib>
#include <sys/time.h>

namespace walgo
{
class bench
{
private:
	timeval timer[2];

public:

	timeval start()
	{
		gettimeofday(&this->timer[0], NULL);
		return this->timer[0];
	}

	timeval stop()
	{
		gettimeofday(&this->timer[1], NULL);
		return this->timer[1];
	}

	int duration() const
	{
		int secs(this->timer[1].tv_sec - this->timer[0].tv_sec);
		int usecs(this->timer[1].tv_usec - this->timer[0].tv_usec);

		if(usecs < 0)
		{
			--secs;
			usecs += 1000000;
		}

		return static_cast<int>(secs * 1000 + usecs / 1000.0 + 0.5);
	}
};


}
#endif /* BENCH_H_ */
