#define _CRT_SECURE_NO_WARNINGS

#include "dealNoise.h"


int main()
{
	IMAGE im;
	long i, j, k;
	float x, y, z;
	double mean, sd;
	char name[128];

	// Try to read an image
	printf("Enter path to the image file to be processed: ");
	scanf("%s", name);
	printf("Opening file '%s'\n", name);

	im = get_image(name);
	display_image(im);

	/* Measure */
	k = 0;
	x = y = 0.0;
	for (i = 0; i < im->info->nr; i++)
	{
		for (j = 0; j < im->info->nc; j++)
		{
			x += (float)(im->data[i][j]);
			y += (float)(im->data[i][j]) * (float)(im->data[i][j]);
			k += 1;
		}
	}

	/* Compute estimate - mean noise is 0 */
	sd = ((float)(k - 1) > 0.0) ? (double)(y - x * x / (float)k) / (float)(k - 1) : 1.0;
	mean = (k != 0) ? (double)(x / (float)k) : -1;
	sd = sqrt(sd);
	printf("Image mean is %10.5f Standard deviation is %10.5f\n",
		mean, sd);

	/* Now assume that the uniform level is the mean, and compute the
		mean and SD of the differences from that!                       */
	x = y = z = 0.0;
	for (i = 0; i < im->info->nr; i++)
	{
		for (j = 0; j < im->info->nc; j++)
		{
			z = (float)(im->data[i][j] - mean);
			x += z;
			y += z * z;
		}
	}

	sd = ((float)(k - 1) > 0.0) ? (double)(y - x * x / (float)k) / (float)(k - 1) : 1.0;
	mean = (k != 0) ? (double)(x / (float)k) : -1;
	sd = sqrt(sd);
	printf("Noise mean is %10.5f Standard deviation is %10.5f\n",
		mean, sd);

	//	display_image (im);
	//	save_image (im, "canny.jpg");

	return 0;
}