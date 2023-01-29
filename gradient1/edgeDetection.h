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
