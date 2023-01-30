#define _CRT_SECURE_NO_WARNINGS

#include "edgeDetection.h"


int main()
{
	IMAGE x;
	char name[128];
	float max1 = 0.0f, max2 = 0.0f;

	// Try to read an image
	printf("[kirsch] Enter path to the image file to be processed: ");
	scanf("%s", name);
	printf("Opening file '%s'\n", name);
	x = get_image(name);

	if (x)
	{
		display_image(x);
		kirsch(x);
		display_image(x);
		save_image(x, "kirsch.jpg");
	}
	return 0;
}