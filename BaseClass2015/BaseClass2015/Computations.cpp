#include <stdio.h>
#include <exception>
#include "Computations.h"

using namespace std;

PlainImage& makeAverageElement(const char* filename, const char* directory)
{
	if (filename)
	{
		string location;
		if (directory)
		{
			location = directory;
			location = location + filename;
		}
		else
		{
			location = filename;
		}
		FILE* in;
		int error;
		unsigned int noOfImages;
		
		error = fopen_s(&in, location.data(), "r");
		if (error)
		{
			throw logic_error("Make average face: - couldn't open file");
		}

		error = fscanf_s(in, "%u", &noOfImages);
		if (error <= 0)
		{
			fclose(in);
			throw logic_error("Make average face: - couldn't read the number of images");
		}
		else if (noOfImages <= 0)
		{
			fclose(in);
			throw logic_error("Make average face: - number of images can't be 0");
		}

		PlainImage img;
		char imageName[100], *err;
		unsigned int *sum = nullptr;
		unsigned int len, width, height;

		fgets(imageName, 100, in);

		//Reading each image and summing it to the @sum vector
		for (unsigned int i = 0; i < noOfImages; ++i)
		{
			location = directory;
			err = fgets(imageName, 100, in);
			if (!err)
			{
				fclose(in);
				throw logic_error("Make average face: - reached EOF");
			}

			if (imageName[strlen(imageName) - 1] == '\n')
			{
				imageName[strlen(imageName) - 1] = '\0';
			}

			location = location + imageName;

			img.readImage(location.data());
			if (!sum)
			{
				len = img.getHeight() * img.getWidth();
				width = img.getWidth();
				height = img.getHeight();
				sum = new unsigned int[len];
				for (unsigned int i = 0; i < len; ++i)
				{
					sum[i] = 0;
				}
			}
			if (img.getWidth() * img.getHeight() != len)
			{
				delete[] sum;
				throw logic_error("Make average face: - images are of different sizes");
			}
			else
			{
				const Byte *pixels = img.imageData();
				if (pixels)
				{
					for (unsigned int i = 0; i < len; ++i)
					{
						sum[i] += pixels[i];
					}
				}
			}
		}//finished computing the sum of the images
		
		Byte *mean = new Byte[len];
		 //Calculate the average
		//Save the average in @mean vector
		for (unsigned int i = 0; i < len; ++i)
		{
			sum[i] /= noOfImages;
			mean[i] = (Byte)(sum[i]);
		}

		//Output the mean image
		PlainImage *meanImage = new PlainImage(width, height, mean, true);
		
		//Free the memory
		fclose(in);
		delete[] sum;
		delete[] mean;

		//Return result
		return *meanImage;
	}
	else
	{
		throw logic_error("Make average face: - no file given");
	}
}

PlainImage& computeThreshold(PlainImage mean, const char * filename, const char * directory)
{
	if (filename)
	{
		string location;
		if (directory)
		{
			location = directory;
			location = location + filename;
		}
		else
		{
			location = filename;
		}
		FILE* in;
		int error;
		unsigned int noOfImages;

		error = fopen_s(&in, location.data(), "r");
		if (error)
		{
			throw logic_error("Make average face: - couldn't open file");
		}

		error = fscanf_s(in, "%u", &noOfImages);
		if (error <= 0)
		{
			fclose(in);
			throw logic_error("Make average face: - couldn't read the number of images");
		}
		else if (noOfImages <= 0)
		{
			fclose(in);
			throw logic_error("Make average face: - number of images can't be 0");
		}

		PlainImage img;
		char imageName[100], *err;
		unsigned int *sum = nullptr;
		unsigned int len, width, height;

		fgets(imageName, 100, in);
		for (unsigned int i = 0; i < noOfImages; ++i)
		{
			
		}

	}
	else
	{
		throw logic_error("Make average face: - no file given");
	}
}
