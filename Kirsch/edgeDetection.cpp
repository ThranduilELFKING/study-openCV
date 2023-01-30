#define _CRT_SECURE_NO_WARNINGS

#include "edgeDetection.h"

#define SQRT2 1.414213562
#define BLACK 0
#define WHITE 1

long seed = 132531;
/* Fraction of pixels that should be above the HIGH threshold */
float ratio = 0.1f;
int WIDTH = 0;

int di[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
int dj[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };

#if defined (MAX)
int    PBM_SE_ORIGIN_COL = 0, PBM_SE_ORIGIN_ROW = 0;
char** arg;
int maxargs;
#else
extern int PBM_SE_ORIGIN_COL, PBM_SE_ORIGIN_ROW;
#endif

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */

int range(IMAGE im, int i, int j)
{
	if ((i < 0) || (i >= im->info->nr)) return 0;
	if ((j < 0) || (j >= im->info->nc)) return 0;
	return 1;
}

/*      PRINT_SE - Print a structuring element to the screen    */

void print_se(IMAGE p)
{
	int i, j;

	printf("\n=====================================================\n");
	if (p == NULL)
		printf(" Structuring element is NULL.\n");
	else
	{
		printf("Structuring element: %dx%d origin at (%d,%d)\n",
			p->info->nr, p->info->nc, p->info->oi, p->info->oj);
		for (i = 0; i < p->info->nr; i++)
		{
			printf("	");
			for (j = 0; j < p->info->nc; j++)
				printf("%4d ", p->data[i][j]);
			printf("\n");
		}
	}
	printf("\n=====================================================\n");
}

IMAGE Input_PBM(char* fn)
{
	int i, j, k, n, m, bi, b;
	unsigned char ucval;
	int val;
	long here;
	char buf1[256];
	FILE* f;
	IMAGE im;

	strcpy(buf1, fn);
	f = fopen(buf1, "r");
	if (f == NULL)
	{
		printf("Can't open the PBM file named '%s'\n", buf1);
		return 0;
	}

	pbm_getln(f, buf1);
	if (buf1[0] == 'P')
	{
		switch (buf1[1])
		{
		case '1':       k = 1; break;
		case '2':       k = 2; break;
		case '3':       k = 3; break;
		case '4':       k = 4; break;
		case '5':       k = 5; break;
		case '6':       k = 6; break;
		default:        printf("Not a PBM/PGM/PPM file.\n");
			return 0;
		}
	}
	bi = 2;

	get_num_pbm(f, buf1, &bi, &m);         /* Number of columns */
	get_num_pbm(f, buf1, &bi, &n);         /* Number of rows */
	if (k != 1 && k != 4) get_num_pbm(f, buf1, &bi, &b);      /* Max value */
	else b = 1;

	fprintf(stderr, "\nPBM file class %d size %d columns X %d rows Max=%d\n",
		k, m, n, b);

	/* Binary file? Re-open as 'rb' */
	if (k > 3)
	{
		here = ftell(f);
		fclose(f);
		f = fopen(fn, "rb");
		here++;
		if (fseek(f, here, 0) != 0)
		{
			printf("Input_PBM: Sync error, file '%s'. Use ASCII PGM.\n", fn);
			exit(3);
		}
	}

	/* Allocate the image */
	if (k == 3 || k == 6)       /* Colour */
		//sys_abort(0, "Colour image.");
		printf("Colour image\n");
	else
	{
		im = (IMAGE)newimage(n, m);
		im->info->oi = PBM_SE_ORIGIN_ROW;
		im->info->oj = PBM_SE_ORIGIN_COL;
		PBM_SE_ORIGIN_ROW = 0;
		PBM_SE_ORIGIN_COL = 0;
		for (i = 0; i < n; i++)
			for (j = 0; j < m; j++)
				if (k < 3)
				{
					fscanf(f, "%d", &val);
					im->data[i][j] = (unsigned char)val;
				}
				else {
					fscanf(f, "%c", &ucval);
					im->data[i][j] = ucval;
				}
	}
	return im;
}

IMAGE Output_PBM(IMAGE image, const char* filename)
{
	FILE* f;
	int i, j, k, perline;
	char buf1[64];

	strcpy(buf1, filename);
	if (image->info->nc > 20) perline = 20;
	else perline = image->info->nc - 1;
	f = fopen(buf1, "w");
	//if (f == 0) sys_abort(0, "Can't open output file.");

	fprintf(f, "P2\n#origin %d %d\n", image->info->oj, image->info->oi);
	fprintf(f, "%d %d %d\n", image->info->nc, image->info->nr, 255);
	k = 0;
	for (i = 0; i < image->info->nr; i++)
		for (j = 0; j < image->info->nc; j++)
		{
			fprintf(f, "%d ", image->data[i][j]);
			k++;
			if (k > perline)
			{
				fprintf(f, "\n");
				k = 0;
			}
		}
	fprintf(f, "\n");
	fclose(f);
	return image;
}

void get_num_pbm(FILE* f, char* b, int* bi, int* res)
{
	int i;
	char str[80];

	while (b[*bi] == ' ' || b[*bi] == '\t' || b[*bi] == '\n')
	{
		if (b[*bi] == '\n')
		{
			pbm_getln(f, b);
			*bi = 0;
		}
		else
			*bi += 1;
	}

	i = 0;
	while (b[*bi] >= '0' && b[*bi] <= '9')
		str[i++] = b[(*bi)++];
	str[i] = '\0';
	sscanf(str, "%d", res);
}

/* Get the next non-comment line from the PBM file f into the
   buffer b. Look for 'pragmas' - commands hidden in the comments */

void pbm_getln(FILE* f, char* b)
{
	int i;
	char c;

	/* Read the next significant line (non-comment) from f into buffer b */
	do
	{

		/* Read the next line */
		i = 0;
		do
		{
			fscanf(f, "%c", &c);
			b[i++] = c;
			if (c == '\n') b[i] = '\0';
		} while (c != '\n');

		/* If a comment, look for a special parameter */
		if (b[0] == '#') pbm_param(b);

	} while (b[0] == '\n' || b[0] == '#');
}

/*      Look for a parameter hidden in a comment        */
void pbm_param(char* s)
{
	int i, j;
	char key[24];

	/* Extract the key word */
	for (i = 0; i < 23; i++)
	{
		j = i;
		if (s[i + 1] == ' ' || s[i + 1] == '\n') break;
		key[i] = s[i + 1];
	}
	key[j] = '\0';

	/* Convert to lower case */
	for (i = 0; i < j; i++)
		if ((key[i] >= 'A') && (key[i] <= 'Z'))
			key[i] = (char)((int)key[i] - (int)'A' + (int)'a');

	/* Which key word is it? */
	if (strcmp(key, "origin") == 0)         /* ORIGIN key word */
	{
		sscanf(&(s[j + 1]), "%d %d",
			&PBM_SE_ORIGIN_COL, &PBM_SE_ORIGIN_ROW);
		return;
	}
}

struct image* newimage(int nr, int nc)
{
	struct image* x;                /* New image */
	int i;
	unsigned char* p;

	if (nr < 0 || nc < 0) {
		printf("Error: Bad image size (%d,%d)\n", nr, nc);
		return 0;
	}

	/*      Allocate the image structure    */
	x = (struct image*)malloc(sizeof(struct image));
	if (!x) {
		printf("Out of storage in NEWIMAGE.\n");
		return 0;
	}

	/*      Allocate and initialize the header      */

	x->info = (struct header*)malloc(sizeof(struct header));
	if (!(x->info))
	{
		printf("Out of storage in NEWIMAGE.\n");
		return 0;
	}
	x->info->nr = nr;       x->info->nc = nc;
	x->info->oi = x->info->oj = 0;

	/*      Allocate the pixel array        */

	x->data = (unsigned char**)malloc(sizeof(unsigned char*) * nr);

	/* Pointers to rows */
	if (!(x->data))
	{
		printf("Out of storage in NEWIMAGE.\n");
		return 0;
	}

	x->data[0] = (unsigned char*)malloc(nr * nc);
	p = x->data[0];
	if (x->data[0] == 0)
	{
		printf("Out of storage. Newimage  \n");
		exit(1);
	}

	for (i = 1; i < nr; i++)
	{
		x->data[i] = (p + i * nc);
	}

	return x;
}

double negexp(double beta)
{
	double x;

	x = drand32();
	return -log(x);
}

double gaussian(double mean, double sd)
{
	static double r2p = 0.0;

	if (r2p == 0.0) r2p = 1.0 / sqrt(2.0 * 3.1415926535);
	return drand32() * sd;
}

double normal(double mean, double sd)
{
	static double a = 0, w2 = 0;
	double u1, u2, b, v, w1;
	int s, t;

	if (a == 1)
	{
		a = 0;
		return mean + w2 * sd;
	}

	do {
		a = 1;
		u1 = drand32();
		u1 = u1 + u1;
		if (u1 > 1.0)
		{
			s = 0; u1 -= 1.0;
		}
		else s = 1;
		u2 = drand32();
		u2 = u2 + u2;
		if (u2 > 1.0)
		{
			t = 0; u2 -= 1.0;
		}
		else t = 1;
		b = u1 * u1 + u2 * u2;
	} while (b > 1);

	v = negexp(1.0);
	w1 = u1 * sqrt(2 * v / b);
	w2 = u2 * sqrt(2.0 * v / b);
	if (s == 0) w1 = -w1;
	if (t == 0) w2 = -w2;
	return mean + w1 * sd;
}

void freeimage(struct image* z)
{
	/*      Free the storage associated with the image Z    */
	int i;

	if (z != 0)
	{
		for (i = 0; i < z->info->nr; i++)
			free(z->data[i]);
		free(z->info);
		free(z->data);
		free(z);
	}
}

void sys_abort(int val, char* mess)
{
	fprintf(stderr, "**** System library ABORT %d: %s ****\n",
		val, mess);
	exit(2);
}

void copy(IMAGE* a, IMAGE b)
{
	CopyVarImage(a, &b);
}

void CopyVarImage(IMAGE* a, IMAGE* b)
{
	int i, j;

	if (a == b) return;
	if (*a) freeimage(*a);
	*a = newimage((*b)->info->nr, (*b)->info->nc);
	//if (*a == 0) sys_abort(0, "No more storage.\n");

	for (i = 0; i < (*b)->info->nr; i++)
		for (j = 0; j < (*b)->info->nc; j++)
			(*a)->data[i][j] = (*b)->data[i][j];
	(*a)->info->oi = (*b)->info->oi;
	(*a)->info->oj = (*b)->info->oj;
}

void Display(IMAGE x)
{
	Output_PBM(x, "x.tmp");
	system("disp  x.tmp");
	system("del x.tmp");

	/*        if (x->info->nr <= 200 && x->info->nc <= 320)
		  disp_lo_grey (x);
		else disp_hi_grey (x);   */
}

float** f2d(int nr, int nc)
{
	float** x, * y;
	int i;

	x = (float**)calloc(nr, sizeof(float*));
	if (x == 0)
	{
		fprintf(stderr, "Out of storage: F2D.\n");
		exit(1);
	}

	for (i = 0; i < nr; i++)
	{
		x[i] = (float*)calloc(nc, sizeof(float));
		if (x[i] == 0)
		{
			fprintf(stderr, "Out of storage: F2D %d.\n", i);
			exit(1);
		}
	}
	return x;
}

/* Small system random number generator */


double drand32()
{
	static long a = 16807L, m = 2147483647L,
		q = 127773L, r = 2836L;
	long lo, hi, test;

	hi = seed / q;
	lo = seed % q;
	test = a * lo - r * hi;
	if (test > 0) seed = test;
	else seed = test + m;

	return (double)seed / (double)m;
}

void srand32(long k)
{
	seed = k;
}

/*      Display the image X using grey color map in 320x200 resolution  */
/* void disp_lo_grey (IMAGE x)                                          */
/* {                                                                    */
/*        int i, j, map[256], r, g, b;                                  */
/*        FILE *f;                                                      */
/*                                                                      */
/*        f = fopen ("debug", "w");                                     */
/*                                                                      */
/* Initialize the graphics system */
/*        GrSetMode (GR_default_graphics);                              */
/*                                                                      */
/* Set all colors in the palette to greys  */
/*        for (i=0; i<256; i++)                                         */
/*        {                                                             */
/*          map[i] = GrAllocColor (i, 0, 55);                           */
/*          GrQueryColor (i, &r, &g, &b);                               */
/*          fprintf (f, "Colour %d is [%d, %d, %d]\n", i, r, g, b);     */
/*        }                                                             */

/* Copy pixels to the VGA */
/*          for(i=0; i<x->info->nr; i++)                                 */
/*          {                                                            */
/*            for (j=0; j<x->info->nc; j++)                              */
/*            {                                                          */
/*              GrPlot3d (j, i, map[(int)(x->data[i][j])] );             */
/*              fprintf (f, "%3d ", GrPixel (j, i));                     */
/*              if (j%14==0) fprintf (f, "\n");                          */
/*            }                                                          */
/*            fprintf (f, "\n");                                         */
/*          }                                                            */
/*          fclose (f);                                                  */
/*                                                                       */
/* Stop displaying when the user pushes the 'enter' key                */
/*        bioskey (0);                                                   */
/*        GrSetMode (GR_default_text);                                   */
/*}                                                                      */

/*      Display the image X as grey levels, 640x480 pixels */

/*void disp_hi_grey (struct image *x)                      */
/*{                                                        */
/*        int i,j, nr, nc;                                 */
/*                                                         */
/*  Initialize the graphics system */
/*        GrSetMode (GR_default_graphics);                 */
/*        for (i=0; i<256; i++)                            */
/*                GrSetColor (i, i, i, i);                 */
/*                                                         */
/*        nr = x->info->nr;       nc = x->info->nc;        */
/*        if (nr > 480) nr = 480;                          */
/*        if (nc > 640) nc = 640;                          */
/*                                                         */
/* Send the pixels to the VGA */
/*        for(i=0; i<nr; i++)                              */
/*          for (j=0; j<nc; j++)                           */
/*            GrPlot (j, i, (int)x->data[i][j]);           */
/*                                                         */
/*        bioskey(0);                                      */
/*        GrSetMode(GR_default_text);                      */
/*}                                                        */

Mat toOpenCV(IMAGE x)
{
	Mat img;
	int i = 0, j = 0;
	Scalar s;

	img = Mat(Size(x->info->nc, x->info->nr), CV_8U, Scalar(1));
	for (i = 0; i < x->info->nr; i++)
	{
		for (j = 0; j < x->info->nc; j++)
		{
			s.val[0] = x->data[i][j];
			////设置原图像中某点的BGR颜色值
			//img.at<Vec3b>(i, j) = Vec3b(s(0), s(1), s(2));
			img.at<uchar>(i, j) = s(0);
		}
	}
	return img;
}

IMAGE fromOpenCV(Mat& x)
{
	IMAGE img;
	int color = 0, i = 0;
	int k = 0, j = 0;
	cv::Scalar s;

	if ((x.depth() == CV_8U) && (x.channels() == 1))								// 1 Pixel (grey) image
		img = newimage(x.rows, x.cols);
	else if ((x.depth() == 8) && (x.channels() == 3)) //Color
	{
		color = 1;
		img = newimage(x.rows, x.cols);
	}
	else {
		printf("[fromOpenCV] x is invalid , depth : %d, channels : %d \n", x.depth(), x.channels());
		return 0;
	}

	for (i = 0; i < x.rows; i++)
	{
		for (j = 0; j < x.cols; j++)
		{
			if (color) {
				s = x.at<Vec3b>(i, j);//读取原图像(i, j)的BGR颜色值，如果是灰度图像，将Vec3b改为uchar
				k = (unsigned char)((s.val[0] + s.val[1] + s.val[2]) / 3);
			}
			else { 
				s = x.at<uchar>(i, j);
				k = (unsigned char)(s.val[0]); 
			}
			img->data[i][j] = k;
		}
	}
	return img;
}


IMAGE get_image(char* name)
{
	IMAGE x = 0;
	Mat image;

	image = imread(name, 0);
	if (image.empty()) {
		printf("[get_image] image is empty \n");
		return 0;
	}
	x = fromOpenCV(image);
	return x;
}

int get_RGB(IMAGE* r, IMAGE* g, IMAGE* b, char* name)
{
	Mat image;
	IMAGE i1, i2, i3;
	int i, j;

	// Read the image from a file into Ip1Image
	image = imread(name, 1);
	if (image.empty()) return 0;

	// Create three AIPCV images of correct size
	i1 = newimage(image.rows, image.cols);
	i2 = newimage(image.rows, image.cols);
	i3 = newimage(image.rows, image.cols);

	// Copy pixels from Ip1Image to AIPCV images
	for (i = 0; i < image.rows; i++)
		for (j = 0; j < image.cols; j++)
		{
			i1->data[i][j] = (image.data + i * image.step)[j * image.channels() + 0];
			i2->data[i][j] = (image.data + i * image.step)[j * image.channels() + 1];
			i3->data[i][j] = (image.data + i * image.step)[j * image.channels() + 2];
		}

	*r = i3;
	*g = i2;
	*b = i1;
	return 1;
}

int save_RGB(IMAGE r, IMAGE g, IMAGE b, char* name)
{
	Mat image;
	int i, j, k;

	if ((r->info->nc != g->info->nc) || (r->info->nr != g->info->nr)) return 0;
	if ((r->info->nc != b->info->nc) || (r->info->nr != b->info->nr)) return 0;

	// Create an  IplImage
	//image = Mat(Size(r->info->nc, r->info->nr), 8, 3);
	image = Mat(Size(r->info->nc, r->info->nr), CV_8U, Scalar(3));

	if (image.empty()) return 0;


	// Copy pixels from AIPCV images into Ip1Image
/*		for (i=0; i<r->info->nr; i++)
		{
			for (j=0; j<r->info->nc; j++)
			{
				s.val[0] = b->data[i][j];
				s.val[1] = g->data[i][j];
				s.val[2] = r->data[i][j];
				cvSet2D (image, i,j,s);
		}
	} */

	for (i = 0; i < image.rows; i++)
		for (j = 0; j < image.cols; j++)
		{
			(image.data + i * image.step)[j * image.channels() + 0] = b->data[i][j];
			(image.data + i * image.step)[j * image.channels() + 1] = g->data[i][j];
			(image.data + i * image.step)[j * image.channels() + 2] = r->data[i][j];
		}
	k = imwrite(name, image);
	return 1 - k;
}

/* Display an image on th escreen */
void display_image(IMAGE x)
{
	Mat image;
	char wn[20];
	int i;

	image = toOpenCV(x);
	if (image.empty()) return;

	for (i = 0; i < 19; i++) wn[i] = (char)((drand32() * 26) + 'a');
	wn[19] = '\0';
	namedWindow(wn, WINDOW_AUTOSIZE);
	imshow("nosie show", image);
	waitKey(0);
}

void save_image(IMAGE x, const char* name)
{
	Mat image;

	image = toOpenCV(x);
	if (image.empty()) return;

	imwrite(name, image);
}

void grad1(IMAGE x)
{
	int i, j;
	double z, dx, dy;
	int pmax = 0, pmin = 255, thresh = 128;

	for (i = x->info->nr - 1; i >= 1; i--)
		for (j = x->info->nc - 1; j >= 1; j--)
		{
			dx = (x->data[i][j] - x->data[i - 1][j]);
			dy = (x->data[i][j] - x->data[i][j - 1]);
			z = sqrt(dx * dx + dy * dy);

			if (z > 255.0) z = 255.0;
			else if (z < 0.0) z = 0.0;
			x->data[i][j] = (unsigned char)z;
			if (pmax < (unsigned char)z) pmax = (unsigned char)z;
			if (pmin > (unsigned char)z) pmin = (unsigned char)z;
		}
	thresh = (pmax - pmin) / 2;

	for (i = 0; i < x->info->nr; i++)
	{
		x->data[i][0] = 0;
		x->data[i][x->info->nc - 1] = 0;
	}
	for (j = 0; j < x->info->nc; j++)
	{
		x->data[0][j] = 0;
		x->data[x->info->nr - 1][j] = 0;
	}

	/*	Threshold	*/
	for (i = 0; i < x->info->nr; i++)
		for (j = 0; j < x->info->nc; j++)
			if (x->data[i][j] > thresh) x->data[i][j] = 0;
			else x->data[i][j] = 255;
}


void grad2(IMAGE x)
{
	int i, j, k;
	double z, dx, dy;
	IMAGE y;
	int pmax = 0, pmin = 255, thresh = 128;

	y = newimage(x->info->nr, x->info->nc);
	for (i = 1; i < x->info->nr - 1; i++)
		for (j = 1; j < x->info->nc - 1; j++)
		{
			dx = (double)(x->data[i][j + 1] - x->data[i][j - 1]);
			dy = (double)(x->data[i + 1][j] - x->data[i - 1][j]);

			z = sqrt(dx * dx + dy * dy);
			if (z > 255.0) z = 255.0;
			else if (z < 0.0) z = 0.0;
			y->data[i][j] = (unsigned char)z;
			if (pmax < (unsigned char)z) pmax = (unsigned char)z;
			if (pmin > (unsigned char)z) pmin = (unsigned char)z;
		}
	thresh = (pmax - pmin) / 2;

	for (i = 0; i < x->info->nr; i++)
	{
		y->data[i][0] = 0;
		y->data[i][x->info->nc - 1] = 0;
	}
	for (j = 0; j < x->info->nc; j++)
	{
		y->data[0][j] = 0;
		y->data[x->info->nr - 1][j] = 0;
	}
	for (i = 0; i < x->info->nr; i++)
		for (j = 0; j < x->info->nc; j++)
			x->data[i][j] = y->data[i][j];

	/*      Threshold       */
	for (i = 0; i < x->info->nr; i++)
		for (j = 0; j < x->info->nc; j++)
			if (x->data[i][j] > thresh) x->data[i][j] = 0;
			else x->data[i][j] = 255;
}

float distance(float i, float j, float ii, float jj)
{
	double x, y;

	if (ii > 1000 || jj > 1000) return 1000.0;
	x = (double)((i - ii) * (i - ii) + (j - jj) * (j - jj));
	y = sqrt(x);
	return (float)y;
}

double max3(double a, double b, double c)
{
	if (a >= b && a >= c) return a;
	if (b >= a && b >= c) return b;
	if (c >= a && c >= b) return c;
}

double alpha(double a, double b)
{
	return fabs((180.0 - fabs(a - b)) / 180.0);
}

void pixdir(int k, int* a, int* b)
{
	switch (k)
	{
	case 0:	*a = 0;   *b = 1; break;
	case 1:	*a = -1;  *b = 1; break;
	case 2:	*a = -1;  *b = 0; break;
	case 3:	*a = -1;  *b = -1; break;
	case 4:	*a = 0;  *b = -1; break;
	case 5:	*a = 1;  *b = -1; break;
	case 6:	*a = 1;  *b = 0; break;
	case 7:	*a = 1;  *b = 1; break;
	default:	printf("Bad direction %d. Must be 0-7.\n", k);
		exit(1);
	}
}

void thresh(IMAGE z)
{
	int histo[256];
	int i, j, t;

	/* Compute a grey level histogram */
	for (i = 0; i < 256; i++) histo[i] = 0;
	for (i = 1; i < z->info->nr - 1; i++)
		for (j = 1; j < z->info->nc - 1; j++)
		{
			histo[z->data[i][j]]++;
		}

	/* Threshold at the middle of the occupied levels */
	i = 255;
	while (histo[i] == 0) i--;
	j = 0;
	while (histo[j] == 0) j++;
	t = (i + j) / 2;

	/* Apply the threshold */
	for (i = 1; i < z->info->nr - 1; i++)
		for (j = 1; j < z->info->nc - 1; j++)
			if (z->data[i][j] >= t) z->data[i][j] = 0;
			else z->data[i][j] = 255;
}


/*	Apply a Sobel edge mask to the image X	*/
void sobel(struct image* x)
{
	int i, j, n, m, k;
	IMAGE z;

	z = 0;
	copy(&z, x);

	for (i = 0; i < x->info->nr; i++)
		for (j = 0; j < x->info->nc; j++)
			z->data[i][j] = 255;

	/* Now compute the convolution, scaling */
	for (i = 1; i < x->info->nr - 1; i++)
		for (j = 1; j < x->info->nc - 1; j++)
		{
			n = (x->data[i - 1][j + 1] + 2 * x->data[i][j + 1] + x->data[i + 1][j + 1]) -
				(x->data[i - 1][j - 1] + 2 * x->data[i][j - 1] + x->data[i + 1][j - 1]);
			m = (x->data[i + 1][j - 1] + 2 * x->data[i + 1][j] + x->data[i + 1][j + 1]) -
				(x->data[i - 1][j - 1] + 2 * x->data[i - 1][j] + x->data[i - 1][j + 1]);
			k = (int)(sqrt((double)(n * n + m * m)) / 4.0);
			z->data[i][j] = k;
		}

	thresh(z);

	for (i = 0; i < x->info->nr; i++)
		for (j = 0; j < x->info->nc; j++)
			x->data[i][j] = z->data[i][j];
}


/*	Apply a Sobel edge mask to the image X	*/
void kirsch(IMAGE x)
{
	int i, j, n, m, k, rmax, rmin;
	IMAGE z;
	float rng, zmax, zmin;

	z = newimage(x->info->nr, x->info->nc);
	for (i = 0; i < x->info->nr; i++)
		for (j = 0; j < x->info->nc; j++)
			z->data[i][j] = 255;

	zmax = 0; zmin = 1000;
	for (i = 1; i < x->info->nr - 1; i++)
		for (j = 1; j < x->info->nc - 1; j++)
		{
			if (i > 60 && j >= 126)
			{
				rng = 0.0;
			}
			rng = (float)(all_masks(x, i, j) / 15.0);
			if (zmax < rng) zmax = rng;
			if (zmin > rng) zmin = rng;
			z->data[i][j] = (unsigned char)rng;
		}
	printf("Max value seen was %f min was %f\n", zmax, zmin);

	thresh(z);

	for (i = 0; i < x->info->nr; i++)
		for (j = 0; j < x->info->nc; j++)
			x->data[i][j] = z->data[i][j];
}


/* Apply Kirsch mask K to pixel (row, col) */
int apply_mask(IMAGE im, int K, int row, int col)
{
	int i, j, k, n, m;

	n = (int)(im->data[di[K] + row][dj[K] + col])
		+ (int)(im->data[di[(K + 1) % 8] + row][dj[(K + 1) % 8] + col])
		+ (int)(im->data[di[(K + 7) % 8] + row][dj[(K + 7) % 8] + col]);
	m = 0;
	for (i = 2; i < 7; i++)
		m += (int)(im->data[di[(K + i) % 8] + row][dj[(K + i) % 8] + col]);
	k = abs(n * 5 - m * 3);
	return k;
}

/* Apply ALL masks to pixel (row, col) and return the largest */
int all_masks(IMAGE im, int row, int col)
{
	int resp[8], k, kb;

	kb = 0;
	for (k = 0; k < 8; k++)
	{
		resp[k] = apply_mask(im, k, row, col);
		if (resp[k] > resp[kb]) kb = k;
	}
	return resp[kb];
}
