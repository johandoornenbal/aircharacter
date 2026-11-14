#pragma once
// Airwindows Sweeten (MIT). Processing bodies unchanged.
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <algorithm>
class SweetenEngine {
public:
 SweetenEngine() { reset(44100.); }
 void reset(double sampleRate);
 void setParameter(int index,float value);
 void processReplacing(float**,float**,int);
 void processDoubleReplacing(double**,double**,int);
 double getSampleRate() const { return rate; }
private:
 double rate=44100.;

    
	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff
	double savg[20];

    float A;
};
