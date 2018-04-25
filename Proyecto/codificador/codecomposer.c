#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <dsplib.h>

float getMaxValue(char *coeff)
{
	char coeff_rev[4];
    char coeff_shift[2];

	coeff_rev[2] = 0x0000;
	coeff_rev[3] = 0x2696;

	coeff_shift[0] = coeff[3] << 8 | coeff[2];
	coeff_shift[1] = coeff[1] << 8 | coeff[0];

	float fnum = * (float*) &coeff_shift[0];

	return fnum;
}

char uquant(float max, int bits, char qNum)
{
	int intMax = trunc(max);
	int intBits = round(log2(intMax));
	int intMaxTmp = pow(2,intBits);

	if(intMaxTmp < intMax)
	{
		intBits += 1;
	}

	int decBits = bits - intBits - 1;

	int tmpNum = (int)qNum;

	float num;
	num = tmpNum/pow(2,decBits);

	return num;
}

struct qComplex
{
    char real;
    char imag;
};

struct uComplex
{
    float real;
    float imag;
};

void main()
{
   FILE *fid;
   fid = fopen("Wav_max4.qwv", "r");
   int file_size;
   fseek(fid, 0, SEEK_END); // seek to end of file
   file_size = ftell(fid); // get current file pointer
   fseek(fid, 0, SEEK_SET); // seek back to beginning of file

   //Defining masks for coeff encoded with 4 and 2 bits
   char high4_mask = 240;
   char low4_mask = 15;
   char high2_mask = 12;
   char low2_mask = 3;

   int map[4] = {8,8,4,2};

   int qBits1 = map[0];
   int qBits2 = map[1];
   int qBits3 = map[2];
   int qBits4 = map[3];

   float coeffMax;
   char coeff[4];
   fread(&coeff,1,4,fid);
   coeffMax = getMaxValue(coeff);

   //------------------------
   struct qComplex XM[33];
   //struct qComplex XM1[1];
   //struct qComplex XM2[28];
   //struct qComplex XM3[2];
   //struct qComplex XM4[2];

   //------------------------
   struct uComplex uXM[33];
   struct uComplex uXM1[1];
   struct uComplex uXM2[28];
   struct uComplex uXM3[2];
   struct uComplex uXM4[2];

   //int iyn = 0;
   //float yn[file_size];

   int n;
   for (n = 0; n < file_size; n += 60)
   {
	   char coeffBlock[60];
   	   fread(&coeffBlock,1,60,fid);

   	   int bytesParsed = 0;
   	   int numByte = 0;
   	   int numTempByte = 0;
   	   while(bytesParsed < 60)
   	   {
		   if(numByte < 1)
		   {
			   //Real using 1 byte
			   char real = coeffBlock[numTempByte];
			   char imag = 0;
			   XM[numByte].real = real;
			   XM[numByte].imag = imag;
			   bytesParsed += 1;
			   numByte += 1;
			   numTempByte += 1;
		   }
		   else if(numByte < 29)
		   {
			   //Complex using 1 byte real + 1 byte imag
			   char real = coeffBlock[numTempByte];
			   char imag = coeffBlock[numTempByte + 1];
			   XM[numByte].real = real;
			   XM[numByte].imag = imag;
			   bytesParsed += 2;
			   numByte += 1;
			   numTempByte += 2;
		   }
		   else if (numByte < 31)
		   {
			   //Complex using upper half byte real + lower half byte imag
			   char tempReal = coeffBlock[numTempByte] & high4_mask;
			   char real = tempReal >> 4;
			   char imag = coeffBlock[numTempByte] & low4_mask;
			   XM[numByte].real = real;
			   XM[numByte].imag = imag;
			   bytesParsed += 1;
			   numByte += 1;
			   numTempByte += 1;
		   }
		   else
		   {
			   //Complex using upper quarter byte real + lower quarter byte imag
			   char tempReal = coeffBlock[numTempByte] & high2_mask;
			   char real = tempReal >> 2;
			   char imag = coeffBlock[numTempByte] & low2_mask;
			   XM[numByte].real = real;
			   XM[numByte].imag = imag;
			   bytesParsed += 1;
			   numByte += 1;
			   numTempByte += 1;
		   }
   	   }

   	   //Building unquantized bands according map
   	   int f0 = 0;
   	   int f1 = 0;
   	   int f2 = 0;
   	   int f3 = 0;
   	   int f4 = 0;

   	   int fm;
   	   for (fm = 0; fm < 33; fm++)
   	   {
   		   if(fm < 1)
		   {
			   //Real using 1 byte
			   uXM1[f1].real = uquant(coeffMax,qBits1, XM[fm].real);
			   uXM1[f1].imag = uquant(coeffMax,qBits1, XM[fm].imag);
			   f1 += 1;
		   }
		   else if(fm < 29)
		   {
			   //Complex using 1 byte real + 1 byte imag
			   uXM2[f2].real = uquant(coeffMax,qBits2, XM[fm].real);
			   uXM2[f2].imag = uquant(coeffMax,qBits2, XM[fm].imag);
			   f2 += 1;
		   }
		   else if (fm < 31)
		   {
			   //Complex using upper half byte real + lower half byte imag
			   uXM3[f3].real = uquant(coeffMax,qBits3, XM[fm].real);
			   uXM3[f3].imag = uquant(coeffMax,qBits3, XM[fm].imag);
			   f3 += 1;
		   }
		   else
		   {
			   //Complex using upper quarter byte real + lower quarter byte imag
			   uXM4[f4].real = uquant(coeffMax,qBits4, XM[fm].real);
			   uXM4[f4].imag = uquant(coeffMax,qBits4, XM[fm].imag);
			   f4 += 1;
		   }
   		   f0 += 1;
   	   }

   	   //Building uXM with results of reverting
   	   f0 = 0;
   	   f1 = 0;
   	   f2 = 0;
   	   f3 = 0;
   	   f4 = 0;

   	   for (fm = 0; fm < 33; fm++)
   	   {
		   if(fm < 1)
		   {
			   //Real using 1 byte
			   uXM[fm].real = uXM1[f1].real;
			   uXM[fm].imag = uXM1[f1].imag;
			   f1 += 1;
		   }
		   else if(fm < 29)
		   {
			   //Complex using 1 byte real + 1 byte imag
			   uXM[fm].real = uXM2[f2].real;
			   uXM[fm].imag = uXM2[f2].imag;
			   f2 += 1;
		   }
		   else if (fm < 31)
		   {
			   //Complex using upper half byte real + lower half byte imag
			   uXM[fm].real = uXM3[f3].real;
			   uXM[fm].imag = uXM3[f3].imag;
			   f3 += 1;
		   }
		   else
		   {
			   //Complex using upper quarter byte real + lower quarter byte imag
			   uXM[fm].real = uXM4[f4].real;
			   uXM[fm].imag = uXM4[f4].imag;
			   f4 += 1;
		   }
		   f0 += 1;
	   }


   	   //Take the irfft of uXM
   	   //float ym[64];
   	   //ym = irfft(uXM, M);


   	   //Add block to output
   	   /*
   	   int m;
   	   for(m = 0; m < 64; m++)
   	   {
   		   yn[iyn] = ym[m];
   		   iyn += 1;
   	   }
   	   */



   //----------------------------------------------------------------------------------------------------
   }

   fclose(fid);
}
