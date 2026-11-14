#include "PurestSaturationEngine.h"
void PurestSaturationEngine::reset(double sampleRate) { rate=sampleRate;

	A = 0.1;
	B = 1.0;

	fpdL = 0x12345678u;
	fpdR = 0x87654321u;
	//this is reset: values being initialized only once. Startup values, whatever they are.
	
    }
void PurestSaturationEngine::setParameter(int index,float value) { value=std::clamp(value,0.f,1.f); switch(index) {
case 0: A=value; break;
case 1: B=value; break;
default: break; } }
void PurestSaturationEngine::processReplacing(float **inputs, float **outputs, int sampleFrames) 
{
    float* in1  =  inputs[0];
    float* in2  =  inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];
	
	long double inputGain = A*10.0;
	long double outputGain = B;
	
    while (--sampleFrames >= 0)
    {
		long double inputSampleL = *in1;
		long double inputSampleR = *in2;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		if (inputGain != 1.0) {
			inputSampleL *= inputGain;
			inputSampleR *= inputGain;
		}		
		
		inputSampleL = fmin(fmax(inputSampleL,-2.032610446872596),2.032610446872596);
		long double X = inputSampleL * inputSampleL;
		long double temp = inputSampleL * X;
		inputSampleL -= (temp*0.125); temp *= X;
		inputSampleL += (temp*0.0078125); temp *= X;
		inputSampleL -= (temp*0.000244140625); temp *= X;
		inputSampleL += (temp*0.000003814697265625); temp *= X;
		inputSampleL -= (temp*0.0000000298023223876953125); temp *= X;
		//purestsaturation: sine, except all the corrections
		//retain mantissa of a long double increasing power function
		inputSampleR = fmin(fmax(inputSampleR,-2.032610446872596),2.032610446872596);
		X = inputSampleR * inputSampleR;
		temp = inputSampleR * X;
		inputSampleR -= (temp*0.125); temp *= X;
		inputSampleR += (temp*0.0078125); temp *= X;
		inputSampleR -= (temp*0.000244140625); temp *= X;
		inputSampleR += (temp*0.000003814697265625); temp *= X;
		inputSampleR -= (temp*0.0000000298023223876953125); temp *= X;
		//purestsaturation: sine, except all the corrections
		//retain mantissa of a long double increasing power function
		
		if (outputGain < 1.0) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
		}
		
		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit stereo floating point dither
		
		*out1 = inputSampleL;
		*out2 = inputSampleR;

		in1++;
		in2++;
		out1++;
		out2++;
    }
}

void PurestSaturationEngine::processDoubleReplacing(double **inputs, double **outputs, int sampleFrames) 
{
    double* in1  =  inputs[0];
    double* in2  =  inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];
	
	long double inputGain = A*10.0;
	long double outputGain = B;
	
    while (--sampleFrames >= 0)
    {
		long double inputSampleL = *in1;
		long double inputSampleR = *in2;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		
		if (inputGain != 1.0) {
			inputSampleL *= inputGain;
			inputSampleR *= inputGain;
		}		
		
		inputSampleL = fmin(fmax(inputSampleL,-2.032610446872596),2.032610446872596);
		long double X = inputSampleL * inputSampleL;
		long double temp = inputSampleL * X;
		inputSampleL -= (temp*0.125); temp *= X;
		inputSampleL += (temp*0.0078125); temp *= X;
		inputSampleL -= (temp*0.000244140625); temp *= X;
		inputSampleL += (temp*0.000003814697265625); temp *= X;
		inputSampleL -= (temp*0.0000000298023223876953125); temp *= X;
		//purestsaturation: sine, except all the corrections
		//retain mantissa of a long double increasing power function
		inputSampleR = fmin(fmax(inputSampleR,-2.032610446872596),2.032610446872596);
		X = inputSampleR * inputSampleR;
		temp = inputSampleR * X;
		inputSampleR -= (temp*0.125); temp *= X;
		inputSampleR += (temp*0.0078125); temp *= X;
		inputSampleR -= (temp*0.000244140625); temp *= X;
		inputSampleR += (temp*0.000003814697265625); temp *= X;
		inputSampleR -= (temp*0.0000000298023223876953125); temp *= X;
		//purestsaturation: sine, except all the corrections
		//retain mantissa of a long double increasing power function
		
		if (outputGain < 1.0) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
		}
		
		//begin 64 bit stereo floating point dither
		int expon; frexp((double)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
		frexp((double)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
		//end 64 bit stereo floating point dither
		
		*out1 = inputSampleL;
		*out2 = inputSampleR;

		in1++;
		in2++;
		out1++;
		out2++;
    }
}
