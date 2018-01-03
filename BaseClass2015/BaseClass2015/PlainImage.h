#pragma once

#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include <exception>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <iostream>	//for testing alone

using namespace std;

#define JPG 0xFFD8
#define BMP 0x4D42
#define REVERSE_BMP 0x424D
#define OFFSET_TO_COLOR_TABLE 54
#define SIZE_OF_COLOR_TABLE 1024

typedef unsigned char Byte;

unsigned short int readShortThroughChar(FILE *in);

class PlainImage
{
private:
	Byte *header;
	Byte *colorTable;
	Byte *image;			//The image itself
	unsigned int width;		//The width of the image
	unsigned int height;	//The height of the image
	unsigned short int bitsPerPixel;
	unsigned long length;
	unsigned int offsetToImage;

public:
	PlainImage();

	PlainImage(const PlainImage& original);

	void drawHistogram(int* data, int len, int numberOfColor = 1);

	unsigned int getWidth();
	unsigned int getHeight();
	unsigned short int getBitsPerPixel();
	unsigned long getLength();
	unsigned int getOffsetToImage();

	Byte* operator[](int index);
	int* makeHistogram(int& length);

	void readImage(char *filename);
	void writeImage(char* filename);
	void writeImageHexFormat(char* filename);

	PlainImage& operator = (const PlainImage& original);

	~PlainImage();


};

