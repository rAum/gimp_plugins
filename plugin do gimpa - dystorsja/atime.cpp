/********************************************************************
 *                                       (c) Andrzej Lukaszewski 2009
 * atime.cpp :
 *             Simple time measurement
 *
 * This file:
 ********************************************************************/
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include "atime.hpp"

static long long starttime;
void ATimerStart() {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	starttime = ((long long)tv.tv_usec) +
				((long long)tv.tv_sec)*1000000;
}

void ATimerReport(char *fmt) {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	long long timediff;
	timediff = ((long long)tv.tv_usec) +
			   ((long long)tv.tv_sec)*1000000  - starttime;;
	float msec = ((float)timediff)/1000;
	printf(fmt, msec);
}

