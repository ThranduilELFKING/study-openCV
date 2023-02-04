#define _CRT_SECURE_NO_WARNINGS

#include "edgeDetection.h"

//RGB方案
void color_edge1();
//T1 T2方案
void color_edge2();
//HSV方案
void color_edge3();


int main() {

	//color_edge1();
	//color_edge2()
	color_edge3();

	waitKey(0);
	return 0;
}

void color_edge1() {
	IMAGE x, y, z;
	char name[128];
	int i, j;

	// Try to read an image
	printf("Enter path to the image file to be processed: ");
	scanf("%s", name);
	printf("Opening file '%s'\n", name);
	if (get_RGB(&x, &y, &z, name))
	{
		sobel(x);
		sobel(y);
		sobel(z);
		//		for (i=0; i<x->info->nr; i++)
		//			for (j=0; j<x->info->nc; j++)
		//				x->data[i][j] = 255-(x->data[i][j]+y->data[i][j]+z->data[i][j])/3;
		save_RGB(x, y, z, "./color_edge.png");
	}
}

void color_edge2() {
	IMAGE x, y, z;
	char name[128];
	int i, j;
	float t1, t2, max1 = 0.0f, max2 = 0.0f;
	int k;

	// Try to read an image
	printf("Enter path to the image file to be processed: ");
	scanf("%s", name);
	printf("Opening file '%s'\n", name);
	if (get_RGB(&x, &y, &z, name))
	{
		for (i = 0; i < x->info->nr; i++)
			for (j = 0; j < x->info->nc; j++)
			{
				k = (float)(x->data[i][j] + y->data[i][j] + z->data[i][j]);
				if (k > 0.0)
				{
					t1 = (float)x->data[i][j] / k;
					t2 = (float)y->data[i][j] / k;
					if (t1 > max1) max1 = t1;
					if (t2 > max2) max2 = t2;
				}
			}

		for (i = 0; i < x->info->nr; i++)
			for (j = 0; j < x->info->nc; j++)
			{
				k = (float)(x->data[i][j] + y->data[i][j] + z->data[i][j]);
				if (k > 0.0)
				{
					t1 = (float)x->data[i][j] / k;
					t2 = (float)y->data[i][j] / k;
					t1 = t1 / max1;
					t2 = t2 / max2;
					x->data[i][j] = (unsigned char)(t1 * 255);
					y->data[i][j] = (unsigned char)(t2 * 255);
				}
				else
				{
					x->data[i][j] = 0;
					y->data[i][j] = 0;
				}
			}
		sobel(x);
		sobel(y);

		save_image(x, "./t1.jpg");
		waitKey(0);
		save_image(y, "./t2.jpg");
	}
}

void color_edge3() {
	IMAGE x, y, z;
	char name[128];
	int i, j;
	float t1, t2, max1 = 0.0f, max2 = 0.0f;
	int k;

	// Try to read an image
	printf("Enter path to the image file to be processed: ");
	scanf("%s", name);
	printf("Opening file '%s'\n", name);
	if (get_RGB(&x, &y, &z, name))
	{
		display_image(x);
		for (i = 0; i < x->info->nr; i++)
			for (j = 0; j < x->info->nc; j++)
				x->data[i][j] = hue(x->data[i][j], y->data[i][j], z->data[i][j]);
		display_image(x);
		sobel(x);
		save_image(x, "./hue.jpg");
	}
}