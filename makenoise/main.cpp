/* Gaussian noise, zero mean. SD is given by arg 1 */
#define _CRT_SECURE_NO_WARNINGS
#include "dealNoise.h"

using namespace cv;

int main()
{
	int i = 0, j = 0, k = 0, kk = 0;
	double x = 0.0, sd = 0.0;
	IMAGE im = 0;
	char name[128];

	// Try to read an image
	printf("Enter path to the image file to be processed: ");
	scanf("%s", name);
	printf("Opening f  ile '%s'\n", name);
	im = get_image(name);

	printf("Enter standard deviation: ");
	scanf("%lf", &sd);
	display_image(im);

	for (i = 0; i < 90; i++)
	{
		printf("%d. %lf\n", i, normal(0.0, sd));
	}

	for (i = 0; i < im->info->nr; i++)
		for (j = 0; j < im->info->nc; j++)
		{
			if (i == j) printf("%d.  %d -> ", i, im->data[i][j]);
			x = (double)(im->data[i][j] + normal(0.0, sd));
			if (i == j) printf(" %lf\n", x);
			if (x < 0) x = 0;
			else if (kk > 255) x = 255;
			im->data[i][j] = (unsigned char)(x + 0.5);
		}
	display_image(im);
	save_image(im, "noisy.jpg");
	waitKey(1000);

	return 0;
}