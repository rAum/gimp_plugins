/********************************************************************
 *                                       (c) Andrzej Lukaszewski 2009
 * Gimp Plugin :
 *             Simple plugin template with dialog
 *
 * This file: filtering/processing for cpu or gpu
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "atime.hpp"

inline double sqr(double r)
{
	return r*r;
}

inline double quad(double r)
{
	return r*r*r*r;
}

inline unsigned char limit(int val)
{
	if (val < 0)
		return 0;
	if (val > 255)
		return 255;
	return val;
}

inline int max(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

inline unsigned char getPixel(unsigned char *buffer, int width, int height, int channels,
				int x, int y, int channel)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return 0;

	return buffer[(y * width + x) * channels + channel];
}

#define origAt(x,y)   (getPixel(orig, width, height, channels, x, y, channel))
#define bufferAt(x,y) (getPixel(orig, width, height, channels, x, y, channel))

void cpu_process(unsigned char *buffer,int width,int height,int channels,
				 const double a1, const double a2)
{
	int mem_size=width*height*channels;
	int idx;
	printf("Wd=%3d Ht=%3d  Channels=%2d\n", width, height, channels);
	ATimerStart();

	unsigned char *orig = new unsigned char[width*height*channels];
	memcpy(orig, buffer, sizeof(unsigned char) * width*height*channels);

	for (int channel = 0; channel < channels; channel++)
	{
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
			{
				double xc = x - width/2;
				double yc = y - height/2;
				double size = max(width, height);
				double radiussq = (sqr(xc) + sqr(yc)) / (size*size);
				double newradius = sqrt(radiussq) * (1 + radiussq * (a1 + radiussq * a2));
				double f = newradius / sqrt(radiussq);

				double newxc = xc * f;
				double newyc = yc * f;
				
				double newx = newxc + (width/2);
				double newy = newyc + (height/2);

	//			printf("[%d, %d] -> [%d, %d] (%.2lf -> %.2lf\n", x, y, int(newx), int(newy), sqrt(radiussq), newradius);

				int x1 = int(newx);
				int y1 = int(newy);
				int x2 = x1 + 1;
				int y2 = y1 + 1;
				double dx = newx - x1;
				double dy = newy - y1;

				double val_up   = dx * origAt(x1, y1) + (1-dx) * origAt(x2, y1);
				double val_down = dx * origAt(x1, y2) + (1-dx) * origAt(x2, y2);

				double val = dy * val_up + (1-dy) * val_down;

				buffer[(y * width + x) * channels + channel] = limit(val);
			}
	}
	
	ATimerReport("CPU filtering: %3.2f ms\n");
}

