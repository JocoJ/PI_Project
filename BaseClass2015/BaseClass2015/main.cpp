#include "PlainImage.h"
#include "Computations.h"
#include <Windows.h>
#include <iostream>

#define SZ 16

int main()
{
	PlainImage img, hist;
	try
	{
		PlainImage mean = makeAverageElement("myFilenames.txt", "Images/");
		mean.writeImage("meanFace.bmp");
	}
	catch (logic_error &e)
	{
		cout << e.what() << '\n';
	}
	return 0;
}