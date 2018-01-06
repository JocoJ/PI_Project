#pragma once
#include "PlainImage.h"

//Computes an average face based on some given normalized faces
//In the file referred by @filename must contain the following:
//		- the number of images used for computing the mean face on the first line
//		- the paths to those faces, one path per line
//			-- the faces must be normalized and of the same size
//			-- BMP or PGM format only
//The function throws a @logic_error object in case of wrong file structure or no file found
//Returns a reference to the resulted mean face as a PlainImage object
PlainImage& makeAverageElement(const char* filename, const char* directory = nullptr);

//Calculates the general difference between the learning set and the mean
//Basically, it gives the dispersion
PlainImage& computeThreshold(PlainImage mean, const char* filename, const char* directory = nullptr);