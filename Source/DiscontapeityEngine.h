#pragma once
// Airwindows Discontapeity (MIT). Processing bodies unchanged.
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <algorithm>
class DiscontapeityEngine {
public:
 DiscontapeityEngine() { reset(44100.); }
 void reset(double sampleRate);
 void setParameter(int index,float value);
 void processReplacing(float**,float**,int);
 void processDoubleReplacing(double**,double**,int);
 double getSampleRate() const { return rate; }
private:
 using VstInt32=int;
 static constexpr int dscBuf=256;
 double rate=44100.;

    
    float A;
	
	double dBaL[dscBuf+5];
	double dBaPosL;
	int dBaXL;
		
	double dBaR[dscBuf+5];
	double dBaPosR;
	int dBaXR;
		
	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff
};
