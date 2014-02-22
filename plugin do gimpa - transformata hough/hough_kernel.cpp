/********************************************************************
 *                                  (c) Andrzej Lukaszewski 2009-2011
 * Gimp Plugin :
 *             Simple plugin template without dialog only message box
 *
 * This file: filtering/processing for cpu
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#define GR(x, y) (orig[(y*W+x)*3 + 0] > 10) ? 1 : 0
#define GG(x, y) (orig[(y*W+x)*3 + 1] > 10) ? 1 : 0
#define GB(x, y) (orig[(y*W+x)*3 + 2] > 10) ? 1 : 0

#define SR(theta, rho) (ht[(rho*tw+theta)*3 + 0])
#define SG(theta, rho) (ht[(rho*tw+theta)*3 + 1])
#define SB(theta, rho) (ht[(rho*tw+theta)*3 + 2])

#define RAD(x) (M_PI * double(x) / 180.0)

unsigned char limit(int n)
{
	if (n < 0)
		return 0;
	if (n > 255)
		return 255;
	return n;
}

unsigned char *houghtransform(unsigned char *orig, int W, int H)
{
	FILE *f = fopen("/tmp/hough.ppm", "w");
	int rho, theta, y, x;
	int th = sqrt(W*W + H*H)/2.0;
	int tw = 360;
	fprintf(f, "P3\n%d %d\n255\n", tw, th);
	unsigned char *ht = new unsigned char[th*tw*3];
	memset(ht, 0, 3*th*tw); // black bg

	for(rho = 0; rho < th; rho++)
	{
		for(theta = 0; theta < tw/*720*/; theta++)
		{
			double C = cos(RAD(theta));
			double S = sin(RAD(theta));
			unsigned int totalred = 0;
			unsigned int totalgreen = 0;
			unsigned int totalblue = 0;
			unsigned int totalpix = 0;
			if ( theta < 45 || (theta > 135 && theta < 225) || theta > 315) {
				for(y = 0; y < H; y++) {
					double dx = W/2.0 + (rho - (H/2.0-y)*S)/C;
					if ( dx < 0 || dx >= W ) continue;
					x = floor(dx+.5);
					if (x == W) continue;
					totalpix++;
					totalred += GR(x, y);
					totalgreen += GG(x, y);
					totalblue += GB(x, y);
				}
			} else {
				for(x = 0; x < W; x++) {
					double dy = H/2.0 - (rho - (x - W/2.0)*C)/S;
					if ( dy < 0 || dy >= H ) continue;
					y = floor(dy+.5);
					if (y == H) continue;
					totalpix++;
					totalred += GR(x, y);
					totalgreen += GG(x, y);
					totalblue += GB(x, y);
				}
			}
		
			double dp = totalpix;
			if (dp == 0) dp = 100000;
			SR(theta, rho) = limit(100*totalred/dp);
			SG(theta, rho) = limit(100*totalgreen/dp);
			SB(theta, rho) = limit(188*totalblue/dp);
			fprintf(f, "%d %d %d\t", SR(theta, rho), SG(theta, rho), SB(theta, rho));
			
		}
		fprintf(f, "\n");
	}

	fclose(f);

	return ht;
}

void cpu_process(unsigned char *buffer,int width,int height,int channels)
{
	int mem_size=width*height*channels;
	int idx;
	printf("Wd=%3d Ht=%3d  Channels=%2d\n", width, height, channels);

	delete [] houghtransform(buffer, width, height);
}
