#include "PlainImage.h"
#include <Windows.h>
#include <iostream>

int main()
{
	PlainImage img;
	try
	{
		img.readImage("constructorTest.bmp");
		PlainImage hist;
		int len;
		int *data = img.makeHistogram(len);
		for (int i = 0; i < len; ++i)
		{
			std::cout << data[i] << ' ';
		}
		cout << len << '\n';
		hist.drawHistogram(data, len);
		hist.writeImage("histogram.bmp");

		img.writeImage("out.bmp");
	}
	catch (exception &e)
	{
		cout << e.what() << '\n';
	}
	return 0;
}