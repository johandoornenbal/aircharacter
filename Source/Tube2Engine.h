#pragma once
// Airwindows Tube2 (MIT). Processing bodies unchanged.
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <algorithm>
class Tube2Engine {
public:
 Tube2Engine() { reset(44100.); }
 void reset(double sampleRate);
 void setParameter(int index,float value);
 void processReplacing(float**,float**,int);
 void processDoubleReplacing(double**,double**,int);
 double getSampleRate() const { return rate; }
private:
 double rate=44100.;

    
	double previousSampleA;
	double previousSampleB;
	double previousSampleC;
	double previousSampleD;
	double previousSampleE;
	double previousSampleF;
	
	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff

    float A;
    float B;
};
