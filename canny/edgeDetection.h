#pragma once

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

/* The image header data structure      */
struct header {
	int nr, nc;             /* Rows and columns in the image */
	int oi, oj;             /* Origin */
};

/*      The IMAGE data structure        */
struct image {
	struct header* info;            /* Pointer to header */
	unsigned char** data;           /* Pixel values */
};

typedef struct image* IMAGE;

int range(IMAGE im, int i, int j);
void print_se(IMAGE p);
IMAGE Input_PBM(char* fn);
IMAGE Output_PBM(IMAGE image, const char* filename);
void get_num_pbm(FILE* f, char* b, int* bi, int* res);
void pbm_getln(FILE* f, char* b);
void pbm_param(char* s);
struct image* newimage(int nr, int nc);
void freeimage(struct image* z);
void sys_abort(int val, char* mess);
void CopyVarImage(IMAGE* a, IMAGE* b);
void Display(IMAGE x);
void display_image(IMAGE x);
float** f2d(int nr, int nc);
void srand32(long k);
double drand32();
void disp_lo_grey(struct image* x);
void disp_hi_grey(struct image* x);
void copy(IMAGE* a, IMAGE b);
void sys_abort(int val, char* mess);
void freeimage(struct image* z);
double negexp(double beta);
double normal(double mean, double sd);
double gaussian(double mean, double sd);
IMAGE fromOpenCV(Mat& x);
Mat toOpenCV(IMAGE x);


IMAGE get_image(char* name);
int get_RGB(IMAGE* r, IMAGE* g, IMAGE* b, char* name);
int save_RGB(IMAGE r, IMAGE g, IMAGE b, char* name);
void save_image(IMAGE x, const char* name);

void grad1(IMAGE x);
void grad2(IMAGE x);

float distance(float i, float j, float ii, float jj);
double alpha(double a, double b);
void pixdir(int k, int* a, int* b);

void thresh(IMAGE z);
void sobel(struct image* x);
void kirsch(IMAGE x);
int apply_mask(IMAGE im, int K, int row, int col);
int all_masks(IMAGE im, int row, int col);

//add for marr
float gauss(float x, float sigma);
float LoG(float x, float sigma);
float meanGauss(float x, float sigma);
void marr(float s, IMAGE im);
void dolap(float** x, int nr, int nc, float** y);
void zero_cross(float** lapim, IMAGE im);
float norm(float x, float y);
float marr_distance(float a, float b, float c, float d);
void convolution(IMAGE im, float** mask, int nr, int nc, float** res,
	int NR, int NC);

//add for canny
int trace(int i, int j, int low, IMAGE im, IMAGE mag, IMAGE ori);
float dGauss(float x, float sigma);
void hysteresis(int high, int low, IMAGE im, IMAGE mag, IMAGE oriim);
void canny(float s, IMAGE im, IMAGE mag, IMAGE ori);
void seperable_convolution(IMAGE im, float* gau, int width,
	float** smx, float** smy);
void dxy_seperable_convolution(float** im, int nr, int nc, float* gau,
	int width, float** sm, int which);
void nonmax_suppress(float** dx, float** dy, int nr, int nc,
	IMAGE mag, IMAGE ori);
void estimate_thresh(IMAGE mag, int* low, int* hi);