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
		if (lena.isPGM())
		{
			lena.convertPGM2BMP();
		}
		FrequencySpecter specter = lena.makeFourierTransform();
		specter.filterIdealHighPass(10);
		filteredLena = specter.makeFourierTransform();
		filteredLena.writeImage("filteredYale10.bmp");
	}
	catch (exception &e)
	{
		cout << e.what() << '\n';
	}
	return 0;
}