#define _CRT_SECURE_NO_WARNINGS

#include "edgeDetection.h"


int main()
{
	int i, j, n;
	float s = 1.0;
	FILE* params;
	IMAGE im1, im2;
	char name[128];

	// Try to read an image
	printf("Enter path to the image file to be processed: ");
	scanf("%s", name);
	printf("Opening file '%s'\n", name);
	im1 = get_image(name);
	printf("Enter standard deviation: ");
	scanf("%f", &s);

	display_image(im1);

	/* Look for parameter file */
	im2 = newimage(im1->info->nr, im1->info->nc);
	for (i = 0; i < im1->info->nr; i++)
		for (j = 0; j < im1->info->nc; j++)
			im2->data[i][j] = im1->data[i][j];

	/* Apply the filter */
	marr(s - 0.8, im1);
	marr(s + 0.8, im2);
	display_image(im2);

	for (i = 0; i < im1->info->nr; i++)
		for (j = 0; j < im1->info->nc; j++)
			if (im1->data[i][j] > 0 && im2->data[i][j] > 0)
				im1->data[i][j] = 0;
			else im1->data[i][j] = 255;

	display_image(im1);
	save_image(im1, "marr.jpg");

	return 0;
}