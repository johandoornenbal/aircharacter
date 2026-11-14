#pragma once
// Airwindows Density3 (MIT). Processing bodies unchanged.
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <algorithm>
class Density3Engine {
public:
 Density3Engine() { reset(44100.); }
 void reset(double sampleRate);
 void setParameter(int index,float value);
 void processReplacing(float**,float**,int);
 void processDoubleReplacing(double**,double**,int);
 double getSampleRate() const { return rate; }
private:
 double rate=44100.;

    
    float A;
    float B;
    float C;
    float D;

	double iirSampleL;
	double iirSampleR;

	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff
};
