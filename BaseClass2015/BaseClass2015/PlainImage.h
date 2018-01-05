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
#define PGM 0x5035
#define OFFSET_TO_COLOR_TABLE 54
#define SIZE_OF_COLOR_TABLE 1024

typedef unsigned char Byte;

unsigned short int readShortThroughChar(FILE *in);
unsigned int distanceSquared(int a, int b);

class FrequencySpecter;

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

	Byte* initBMPStructure();
	void updateOffsetToImage(unsigned int aux);
	void PGMReadParam(FILE* in, const char* format, void* param, string err = "");

public:
	PlainImage();

	PlainImage(unsigned int width, unsigned int height, Byte* imageData);
	PlainImage(const PlainImage& original);

	void drawHistogram(int* data, int len, int numberOfColor = 1);
	void convertPGM2BMP();

	unsigned int getWidth();
	unsigned int getHeight();
	unsigned short int getBitsPerPixel();
	unsigned long getLength();
	unsigned int getOffsetToImage();
	Byte* imageData();

	Byte* operator[](int index);
	int* makeHistogram(int& length);

	void readImage(char *filename);
	void writeImage(char* filename);
	void writeImageHexFormat(char* filename);

	bool isPGM();

	PlainImage& operator = (const PlainImage& original);

	FrequencySpecter& makeFourierTransform();

	~PlainImage();
};

class FrequencySpecter
{
	float* specter;
	unsigned int width;
	unsigned int height;
public:
	
	FrequencySpecter();
	FrequencySpecter(unsigned int width, unsigned int height, float* specter);
	FrequencySpecter(const FrequencySpecter&);

	FrequencySpecter& operator=(const FrequencySpecter&  original);

	unsigned int getWidth();
	unsigned int getHeight();


	~FrequencySpecter();

	void filterIdealLowPass(float threshold);
	void filterIdealHighPass(float threshold);
	void filterButterworthHighPass(float threshold, unsigned int order);
	void filterButterworthLowPass(float threshold, unsigned int order);

	float* getSpecter();

	PlainImage&	makeFourierTransform();
};