#include "PlainImage.h"
#include "FrequencyImage.h"

using namespace std;

unsigned short int readShortThroughChar(FILE *in)
{
	unsigned short int result;
	unsigned char High, Low;
	fscanf_s(in, "%c", &High, 1);
	fscanf_s(in, "%c", &Low, 1);
	result = ((unsigned short int)High << 8) | Low;
	return result;
}

unsigned int distanceSquared(int a, int b)
{
	return (a*a + b*b);
}

//For conversion and creation purposes
//Creates the structure for a 256 color grayscale bitmap
//Updates @bitsPerPixel, @offsetToImage
//Returns a raw pointer to the newly created BMP pixel array
//Does not update @image pointer or @colorTable pointer
Byte* PlainImage::initBMPStructure()
{
	Byte *data = nullptr;

	this->length = OFFSET_TO_COLOR_TABLE + SIZE_OF_COLOR_TABLE + width * height;
	data = new Byte[length];
	Byte* colorTable = data + OFFSET_TO_COLOR_TABLE;

	for (unsigned int i = 0; i < length; ++i)
	{
		data[i] = 0;
	}
	Byte val;
	unsigned int i;
	for (i = 0, val = 0; i < SIZE_OF_COLOR_TABLE; i += 4, val++)
	{
		colorTable[i] = colorTable[i + 1] = colorTable[i + 2] = val;
	}

	//Set the signature
	data[0] = (BMP & 0xff);
	data[1] = (BMP >> 8);

	//Set offset to image data
	offsetToImage = OFFSET_TO_COLOR_TABLE + SIZE_OF_COLOR_TABLE;
	data[10] = (offsetToImage & 0xff);
	data[11] = ((offsetToImage >> 8) & 0xff);
	data[12] = ((offsetToImage >> 16) & 0xff);
	data[13] = ((offsetToImage >> 24) & 0xff);

	//Set BITMAPINFOHEADER
	data[14] = 40;

	//Set width of BMP
	data[18] = (width & 0xff);
	data[19] = ((width >> 8) & 0xff);
	data[20] = ((width >> 16) & 0xff);
	data[21] = ((width >> 24) & 0xff);

	//Set height of BMP
	data[22] = (height & 0xff);
	data[23] = ((height >> 8) & 0xff);
	data[24] = ((height >> 16) & 0xff);
	data[25] = ((height >> 24) & 0xff);

	//Set number of planes in the image
	data[26] = 1;

	//Set number of bits per pixel
	data[28] = 8;
	bitsPerPixel = 8;

	//Set size of image data in bytes
	data[34] = (width*height & 0xff);
	data[35] = ((width*height >> 8) & 0xff);
	data[36] = ((width*height >> 16) & 0xff);
	data[37] = ((width*height >> 24) & 0xff);

	//Set number of colors in color table
	data[46] = (256 & 0xff);
	data[47] = ((256 >> 8) & 0xff);
	data[48] = ((256 >> 16) & 0xff);
	data[49] = ((256 >> 24) & 0xff);

	return data;
}

void PlainImage::updateOffsetToImage(unsigned int aux)
{
	while (aux)
	{
		aux /= 10;
		offsetToImage++;
	}
}

//Private method for reading a parameter for a PGM format image
//It is made for parsing around any comments (starting with '#', found in the header)
//Updates @offsetToImage
void PlainImage::PGMReadParam(FILE * in, const char * format, void * param, string err)
{
	string errstr;
	int error = 0;
	char buffer[100];
	do
	{
		error = fscanf_s(in, format, param);
		if (error < 0)
		{
			errstr = errstr + err;
			if (error == EOF)
				errstr = errstr + ", EOF";
			throw exception(errstr.data());
		}
		else if (error == 0)
		{
			char* aux = fgets(buffer, 100, in);
			unsigned int n = (unsigned int)strlen(buffer);
			offsetToImage += n;
			if (aux && buffer[0] == '#')
			{
				while (aux && buffer[n - 1] != '\n')
				{
					aux = fgets(buffer, 100, in);
					n = (unsigned int)strlen(buffer);
					offsetToImage += n;
				}
				if (!aux)
				{
					errstr = errstr + err;
					if (feof(in))
						errstr = errstr + ", EOF";
					throw exception(errstr.data());
				}
			}
		}

	} while (error <= 0);
}

bool PlainImage::isPGM()
{
	unsigned short int signature;
	signature = ((unsigned short int)header[0] << 8) | (header[1]);
	return (signature == PGM);
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

//Parameters constructor for creating a grayscale BMP image with the given data
PlainImage::PlainImage(unsigned int width, unsigned int height, Byte * imageData)
{
	if (imageData)
	{
		this->width = width;
		this->height = height;
		header = initBMPStructure();
		colorTable = header + OFFSET_TO_COLOR_TABLE;
		image = header + offsetToImage;
		memcpy(image, imageData, width*height * sizeof(Byte));
	}
	else
	{
		throw exception("NULL imageData\n");
	}
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
		else if (signature == PGM)
		{
			header = data;
			colorTable = nullptr;
			image = data + offsetToImage;
		}
		else if (signature == JPG)
		{
			//TO_DO
		}
		else
		{
			throw exception("Unknown format (How the fuck did you do that?)");
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

		header = initBMPStructure();
		colorTable = header + OFFSET_TO_COLOR_TABLE;
		image = header + offsetToImage;

		PlainImage &temp = *this;

		for (int i = 0; i < len; ++i)
		{
			for (int j = 0; j < 100*data[i]/maxim; ++j)
			{
				temp[j][i] = 255;
			}
		}
	}
}

//Converts a PGM image to a BMP one
//No BMP to PGM conversion exists yet
void PlainImage::convertPGM2BMP()
{
	if (this->isPGM())
	{
		Byte* data = initBMPStructure();

		memcpy(data + offsetToImage, image, sizeof(Byte)*width*height);

		if (header)
		{
			delete[] header;
		}
		header = data;
		colorTable = header + OFFSET_TO_COLOR_TABLE;
		image = header + this->offsetToImage;
		PlainImage &aux = *this;
		for (int i = 0; i < height / 2; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				Byte swap;
				swap = aux[i][j];
				aux[i][j] = aux[height - i - 1][j];
				aux[height - i - 1][j] = swap;
			}
		}
	}
	else
	{
		throw exception("Image is not a PGM format");
	}
}

//Destructor
//Deleting the header is enough, because the other pointers point to addresses contained within the @header
PlainImage::~PlainImage()
{
	if (header)
		delete[] header;
}

//Read an image PGM or BMP
void PlainImage::readImage(char* filename)
{
	FILE *in = nullptr;
	if (!filename)
		throw exception("No file given\n");
	fopen_s(&in, filename, "rb");
	if (!in)
		throw exception("Couldn't open file or file couldn't be found\n");
	fseek(in, 0, SEEK_END);
	long int size = ftell(in);
	if (size <= 0)
	{
		fclose(in);
		throw exception("Empty file\n");
	}

	rewind(in);
	Byte *data = new Byte[size];
	unsigned long int readResult = (unsigned long int)fread(data, sizeof(Byte), size, in);
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
	else if (signature == PGM)			//only works with 8 bits per pixel PGM images
	{
		string errstr = "Incorrect read from PMG file";
		header = data;
		fopen_s(&in, filename, "r");	//reopen the same file as a text file
		fseek(in, 2, 0);				//skip over the signature

		offsetToImage = 6;

		PGMReadParam(in, "%u", &width, errstr + ": width");		//the first integer is the width
		updateOffsetToImage(width);

		PGMReadParam(in, "%u", &height, errstr + ": height");	//the second integer is the height
		updateOffsetToImage(height);

		unsigned int maxVal;
		PGMReadParam(in, "%u", &maxVal, errstr + ": maxVal");	//the third integer is the maximum value found in the image
		updateOffsetToImage(maxVal);

		bitsPerPixel = 0;	//computing the number of bits per pixel
		while (maxVal)
		{
			bitsPerPixel++;
			maxVal >>= 1;
		}
		if (bitsPerPixel < 8)
			bitsPerPixel = 8;
		else if (bitsPerPixel >8)
			bitsPerPixel = 16;
		length = size;
		//offsetToImage = ftell(in);
		image = header + offsetToImage;
		colorTable = nullptr;
		fclose(in);
	}
	else
	{
		throw exception("Unknown format\nIf you are trying to open an image in PGM format, note that the first line must be the signature (\"P5\")");
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
	unsigned long int writeResult = (unsigned long int)fwrite(header, sizeof(Byte), length, out);
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
	for (unsigned long i = 0; i < length; ++i)
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

FrequencySpecter& PlainImage::makeFourierTransform()
{
	FrequencySpecter *out = nullptr;
	float* specter = new float[width*height];
	float* img = new float[width*height];
	for (unsigned int i = 0; i < width*height; ++i)
	{
		img[i] = (float)image[i];
	}
	cv::Mat dst(height, width, CV_32F, specter), src(height, width, CV_32F, img);
	cv::dft(src, dst);
	out = new FrequencySpecter(width, height, specter);
	delete[] specter;
	delete[] img;
	return *out;
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

Byte * PlainImage::imageData()
{
	return image;
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

FrequencySpecter::FrequencySpecter()
{
	specter = nullptr;
	width = height = 0;
}

FrequencySpecter::FrequencySpecter(unsigned int width, unsigned int height, float * specter)
{
	if (specter)
	{
		this->width = width;
		this->height = height;
		if (width*height)
		{
			this->specter = new float[width*height];
			memcpy(this->specter, specter, width*height*sizeof(float));
		}
		else
		{
			this->specter = nullptr;
			throw exception("width or height is 0\n");
		}
	}
	else
	{
		throw exception("Nullptr in *specter\n");
	}
}

FrequencySpecter::FrequencySpecter(const FrequencySpecter &origin)
{
	width = origin.width;
	height = origin.height;
	this->specter = new float[width*height];
	memcpy(specter, origin.specter, sizeof(float) * width*height);
}

FrequencySpecter & FrequencySpecter::operator=(const FrequencySpecter & original)
{
	if (specter)
	{
		delete[] specter;
		specter = nullptr;
	}

	width = original.width;
	height = original.height;
	if (original.specter)
	{
		specter = new float[height*width];
		memcpy(specter, original.specter, sizeof(float) * width*height);
	}

	return *this;
}

unsigned int FrequencySpecter::getWidth()
{
	return width;
}

unsigned int FrequencySpecter::getHeight()
{
	return height;
}

FrequencySpecter::~FrequencySpecter()
{
	if (specter)
	{
		delete[] specter;
		specter = nullptr;
	}
}

//LowPass ideal filter with the specified threshold
void FrequencySpecter::filterIdealLowPass(float threshold)
{
	float threshSquare = threshold * threshold;
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			if (distanceSquared((int)i, (int)j) > (unsigned int)threshSquare)
			{
				specter[i*width + j] = 0;
			}
		}
	}
}

void FrequencySpecter::filterIdealHighPass(float threshold)
{
	float threshSquare = threshold * threshold;
	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			if (distanceSquared((int)i, (int)j) < (unsigned int)threshSquare)
			{
				specter[i*width + j] = 0;
			}
		}
	}
}

void FrequencySpecter::filterButterworthHighPass(float threshold, unsigned int order)
{
	if (specter)
	{
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				float h = 1 / (1 + (sqrt(2) - 1)*pow((threshold / sqrt(distanceSquared(i, j))), 2 * order));
				specter[i * width + j] *= h;
			}
		}
	}
}

void FrequencySpecter::filterButterworthLowPass(float threshold, unsigned int order)
{
	if (specter)
	{
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				float h = 1 / (1 + (sqrt(2) - 1) * pow(sqrt(distanceSquared(i, j)) / threshold, 2 * order));
				specter[i*width + j] *= h;
			}
		}
	}
}

float * FrequencySpecter::getSpecter()
{
	return specter;
}

PlainImage & FrequencySpecter::makeFourierTransform()
{
	PlainImage *spatialImage;
	float *img = new float[width*height];
	Byte *image = new Byte[width*height];
	cv::Mat src(height, width, CV_32F, specter), dst(height, width, CV_32F, img);
	cv::dft(src, dst, cv::DFT_REAL_OUTPUT + cv::DFT_INVERSE + cv::DFT_SCALE);
	for (unsigned int i = 0; i < width*height; ++i)
	{
		if (img[i] < 0)
			img[i] = -1 * img[i];
		image[i] = (Byte)img[i];
	}
	spatialImage = new PlainImage(width, height, image);
	delete[] img;
	delete[] image;
	return *spatialImage;
}
