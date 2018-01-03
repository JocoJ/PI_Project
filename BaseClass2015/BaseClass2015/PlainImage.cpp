#include "PlainImage.h"

using namespace std;

unsigned short int readShortThroughChar(FILE *in)
{
	unsigned short int result;
	unsigned char High, Low;
	fscanf_s(in, "%c", &High, sizeof(High));
	fscanf_s(in, "%c", &Low, sizeof(Low));
	result = ((unsigned short int)High << 8) | Low;
	return result;
}

//No parameters constructor
//Sets all members to 0;
PlainImage::PlainImage()
{
	image = nullptr;
	header = nullptr;
	colorTable = nullptr;
	width = 0;
	height = 0;
	bitsPerPixel = 0;
	length = 0;
	offsetToImage = 0;
}

//Copy constructor
PlainImage::PlainImage(const PlainImage& original)
{
	width = original.width;
	height = original.height;
	bitsPerPixel = original.bitsPerPixel;
	length = original.length;
	offsetToImage = original.offsetToImage;

	if (original.header && original.image)
	{

		Byte *data = new Byte[length];
		memcpy(data, original.header, length);

		unsigned short int signature = ((unsigned short int)original.header[0] << 8) | original.header[1];
		if (signature == REVERSE_BMP)
		{
			header = data;
			colorTable = data + OFFSET_TO_COLOR_TABLE;
			image = data + offsetToImage;
		}
	}
}

//Creates a grayscale histogram of the given @data vector
void PlainImage::drawHistogram(int * data, int len, int numberOfColor)
{
	if (data)
	{
		int maxim = -1, index;
		for (int i = 0; i < len; ++i)
		{
			if (maxim < data[i])
			{
				index = i;
				maxim = data[i];
			}
		}

		width = len;
		height = 120;
		
		if (header)
		{
			delete[] header;
		}

		this->length = OFFSET_TO_COLOR_TABLE + SIZE_OF_COLOR_TABLE + width * height;
		header = new Byte[length];
		image = header + OFFSET_TO_COLOR_TABLE + SIZE_OF_COLOR_TABLE;
		colorTable = header + OFFSET_TO_COLOR_TABLE;

		for (unsigned int i = 0; i < length; ++i)
		{
			header[i] = 0;
		}
		Byte val;
		unsigned int i;
		for (i = 0, val = 0; i < SIZE_OF_COLOR_TABLE; i+=4, val++)
		{
			colorTable[i] = colorTable[i + 1] = colorTable[i + 2] = val;
		}

		//Set the signature
		header[0] = (BMP & 0xff);
		header[1] = (BMP >> 8);

		//Set offset to image data
		offsetToImage = OFFSET_TO_COLOR_TABLE + SIZE_OF_COLOR_TABLE;
		header[10] = (offsetToImage & 0xff);
		header[11] = ((offsetToImage >> 8) & 0xff);
		header[12] = ((offsetToImage >> 16) & 0xff);
		header[13] = ((offsetToImage >> 24) & 0xff);

		//Set BITMAPINFOHEADER
		header[14] = 40;
		
		//Set width of histogram
		header[18] = (width & 0xff);
		header[19] = ((width >> 8) & 0xff);
		header[20] = ((width >> 16) & 0xff);
		header[21] = ((width >> 24) & 0xff);

		//Set height of histogram
		header[22] = (height & 0xff);
		header[23] = ((height >> 8) & 0xff);
		header[24] = ((height >> 16) & 0xff);
		header[25] = ((height >> 24) & 0xff);

		//Set number of planes in the image
		header[26] = 1;

		//Set number of bits per pixel
		header[28] = 8;
		bitsPerPixel = 8;

		//Set size of image data in bytes
		header[34] = (width*height & 0xff);
		header[35] = ((width*height >> 8) & 0xff);
		header[36] = ((width*height >> 16) & 0xff);
		header[37] = ((width*height >> 24) & 0xff);

		//Set number of colors in color table
		header[46] = (256 & 0xff);
		header[47] = ((256 >> 8) & 0xff);
		header[48] = ((256 >> 16) & 0xff);
		header[49] = ((256 >> 24) & 0xff);

		PlainImage &temp = *this;

		for (int i = 0; i < len; ++i)
		{
			for (int j = 0; j < 100*data[i]/maxim; ++j)
			{
				temp[j][i] = 255;
			}
		}
		int a = 0;
		a++;
	}
}

//Destructor
//Deleting the header is enough, because the other pointers point to addresses contained within the @header
PlainImage::~PlainImage()
{
	if (header)
		delete[] header;
}

//Read an image
void PlainImage::readImage(char* filename)
{
	FILE *in = nullptr;
	if (!filename)
		throw exception("No file given\n");
	fopen_s(&in, filename, "rb");
	if (!in)
		throw exception("Couldn't open file\n");
	fseek(in, 0, SEEK_END);
	long int size = ftell(in);
	if (size <= 0)
	{
		fclose(in);
		throw exception("Empty file\n");
	}

	rewind(in);
	Byte *data = new Byte[size];
	long int readResult = fread(data, sizeof(Byte), size, in);
	if (readResult != size)
	{
		delete[] data;
		string err = "Incorrect read: ";
		if (ferror(in))
			err = err + "Error while reading ";
		if (feof(in))
			err = err + "Reached EOF ";
		fclose(in);
		throw exception(err.data());
	}
	fclose(in);
	unsigned short int signature = ((unsigned short int)data[0]<<8) | data[1];

	if (header)
	{
		delete[] header;
		header = nullptr;
	}

	if (signature == REVERSE_BMP)
	{
		header = data;
		width = ((unsigned int)header[21] << 24) | ((unsigned int)header[20] << 16) | ((unsigned int)header[19] << 8) | header[18];
		height = ((unsigned int)header[25] << 24) | ((unsigned int)header[24] << 16) | ((unsigned int)header[23] << 8) | header[22];
		bitsPerPixel = ((unsigned short int)header[29] << 8) | header[28];
		offsetToImage = ((unsigned int)header[13] << 24) | ((unsigned int)header[12] << 16) | ((unsigned int)header[11] << 8) | header[10];
		length = size;
		image = data + offsetToImage;
		colorTable = data + OFFSET_TO_COLOR_TABLE;
	}
	else if (signature == JPG)
	{
		//TO_DO
	}
	else
	{
		throw exception("Unknown format");
	}
}

//Writes an image to the output with @filename
void PlainImage::writeImage(char* filename)
{
	FILE *out = nullptr;
	int error = fopen_s(&out, filename, "wb");
	if (error)
	{
		throw exception("Couldn't open the file");
	}
	long int writeResult = fwrite(header, sizeof(Byte), length, out);
	fclose(out);
	if (writeResult != length)
	{
		throw exception("Incorrect write");
	}
}

//Writes an image in formatted hex format to a text file
//For verifications
void PlainImage::writeImageHexFormat(char * filename)
{
	FILE *out = nullptr;
	int error = fopen_s(&out, filename, "w");
	if (error)
	{
		throw exception("Couldn't open the file");
	}
	for (long int i = 0; i < length; ++i)
	{
		if (header[i] <= 0xf)
			fputc('0', out);
		fprintf(out, "%X", header[i]);
		fputc(' ', out);
		if ((i + 1) % 16 == 0)
			fputc('\n', out);
	}
	fclose(out);
}

//Attribution operator
PlainImage& PlainImage::operator=(const PlainImage & original)
{
	if (header)
	{
		delete[] header;
		header = nullptr;
		image = nullptr;
	}

	width = original.width;
	height = original.height;
	length = original.length;
	bitsPerPixel = original.bitsPerPixel;
	offsetToImage = original.offsetToImage;

	if (original.header)
	{
		header = new Byte[length];
		for (unsigned long i = 0; i < length; ++i)
		{
			header[i] = original.header[i];
		}
		image = header + offsetToImage;
		colorTable = header + OFFSET_TO_COLOR_TABLE;
	}
	return *this;
}

unsigned int PlainImage::getWidth()
{
	return width;
}

unsigned int PlainImage::getHeight()
{
	return height;
}

unsigned short int PlainImage::getBitsPerPixel()
{
	return bitsPerPixel;
}

unsigned long PlainImage::getLength()
{
	return length;
}

unsigned int PlainImage::getOffsetToImage()
{
	return offsetToImage;
}

Byte * PlainImage::operator[](int index)
{
	if (header && image)
	{
		return &image[index*width];
	}
	else
	{
		return nullptr;
	}
}

//Returns a vector corresponding to the histogram of the image (grayscale only)
int* PlainImage::makeHistogram(int &length)
{
	if (image)
	{
		length = (bitsPerPixel / 8) * 256;
		int* histogram = new int[length];
		for (int i = 0; i < length; ++i)
		{
			histogram[i] = 0;
		}
		for (unsigned int i = 0; i < width*height; ++i)
		{
			histogram[(int)image[i]]++;
		}
		return histogram;
	}
	return nullptr;
}

