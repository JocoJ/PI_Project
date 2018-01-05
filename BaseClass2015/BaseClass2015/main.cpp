#include "PlainImage.h"
#include <Windows.h>
#include <iostream>

#define SZ 16

int main()
{
	PlainImage img, hist;
	try
	{
		PlainImage lena;
		PlainImage filteredLena;
		lena.readImage("yale.pgm");
		lena.convertPGM2BMP();
		lena.writeImage("Images/img.bmp");
	}
	catch (logic_error &e)
	{
		cout << e.what() << '\n';
	}
	return 0;
}