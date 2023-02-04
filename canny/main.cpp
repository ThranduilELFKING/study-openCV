#define _CRT_SECURE_NO_WARNINGS

#include "edgeDetection.h"

constexpr const int WIDTH = 0;

int main()
{
	int i, j;
	float s = 1.0;
	int low = 0, high = -1;
	FILE* params;
	IMAGE im, magim, oriim;
	char name[128];

	// Try to read an image
	printf("Enter path to the image file to be processed: ");
	scanf("%s", name);
	printf("Opening file '%s'\n", name);

	/* Read parameters from the file canny.par */
	params = fopen("canny.par", "r");
	if (params)
	{
		fscanf(params, "%d", &low);  /* Lower threshold */
		fscanf(params, "%d", &high); /* High threshold */
		fscanf(params, "%f", &s);   /* Gaussian standard deviation */
		printf("Parameters from canny.par: HIGH: %d LOW %d Sigma %f\n",
			high, low, s);
		fclose(params);
	}
	else printf("Parameter file 'canny.par' does not exist.\n");

	im = get_image(name);
	display_image(im);

	/* Create local image space */
	magim = newimage(im->info->nr, im->info->nc);
	if (magim == NULL)
	{
		printf("Out of storage: Magnitude\n");
		exit(1);
	}

	oriim = newimage(im->info->nr, im->info->nc);
	if (oriim == NULL)
	{
		printf("Out of storage: Orientation\n");
		exit(1);
	}

	/* Apply the filter */
	canny(s, im, magim, oriim);

	/* Hysteresis thresholding of edge pixels */
	hysteresis(high, low, im, magim, oriim);

	for (i = 0; i < WIDTH; i++)
		for (j = 0; j < im->info->nc; j++)
			im->data[i][j] = 255;

	for (i = im->info->nr - 1; i > im->info->nr - 1 - WIDTH; i--)
		for (j = 0; j < im->info->nc; j++)
			im->data[i][j] = 255;

	for (i = 0; i < im->info->nr; i++)
		for (j = 0; j < WIDTH; j++)
			im->data[i][j] = 255;

	for (i = 0; i < im->info->nr; i++)
		for (j = im->info->nc - WIDTH - 1; j < im->info->nc; j++)
			im->data[i][j] = 255;



	display_image(im);
	save_image(im, "canny.jpg");

	return 0;
}