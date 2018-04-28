// Define usado para que la FFT se haga en punto fijo
#define FIXED_POINT 32

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "kiss_fft.h"
#include "_kiss_fft_guts.h"

#define MAX_SAMPLES 25000
#define N_FFT 32

#pragma warning (disable : 4996)

int f1Idx;
int f2Idx;
int f3Idx;
int f4Idx;

int quant(float max, int bits, float in)
{

	int iFix;
	int iBits;
	int iMaxTmp;
	int iDecBits;
	int qOut;

	iFix = (int) max;
	iBits = (int)(roundf(log2f(iFix)));

	iMaxTmp = 1 << iBits;

	if (iMaxTmp < iFix)
		iBits += 1;

	iDecBits = bits - iBits - 1;

	qOut = (int)roundf(in * powf(2, iDecBits));

	return qOut;

}

void compute_fft(kiss_fft_cpx in[N_FFT], kiss_fft_cpx out[N_FFT])
{
  kiss_fft_cfg cfg;

  if ((cfg = kiss_fft_alloc(N_FFT, 1, NULL, NULL)) != NULL)
  {
    size_t i;

    kiss_fft(cfg, in, out);
    free(cfg);

    for (i = 0; i < N_FFT; i++)
    {
      printf(" in[%2zu] = %+d , %+d", i, in[i].r, in[i].i);
      printf("            out[%2zu] = %+d , %+d", i, out[i].r, out[i].i);
      printf("\n");
    }
  }
  else
  {
    printf("not enough memory?\n");
    exit(-1);
  }
}


int main(int argc, char ** argv)
{

	FILE *input_binary;
	float arrSamples[MAX_SAMPLES];
	float Max = 0.0;
	int intIdx = 0;
	int intSamples = 0;
	int intIsInvFFT = 1;
  kiss_fft_cpx input_fft[N_FFT];
  kiss_fft_cpx output_fft[N_FFT];
  int8_t frame_max_value = 0;
  int32_t real_part = 0;
  int32_t imaginary_part = 0;

	f1Idx = 1;
	f2Idx = 29;
	f3Idx = 31;
	f4Idx = 33;

	input_binary = fopen("Wav.qwv", "r");
  int file_size;
  fseek(input_binary, 0, SEEK_END);  // seek to end of file
  file_size = ftell(input_binary);   // get current file pointer
  fseek(input_binary, 0, SEEK_SET);  // seek back to beginning of file

  // Read the whole binary file
  char binary_file[file_size];
  fread(binary_file, 1, file_size, input_binary);

  // Recorrer el archivo completo. Cada frame es un conjunto de 32 elementos
  for (int frame = 0; frame < file_size; frame += N_FFT) {

    // Convert the int8_t to int32_t and prepare the input for the inverse FFT
  	for (int i = frame, fft_idx = 0; i < N_FFT * frame; i++, fft_idx++){

      // Coeficiente solo real
      if(i < f1Idx){
        real_part = (int32_t)binary_file[i];
        imaginary_part = (int32_t)0;
      }

      // Siguen 28 coeficientes con parte real e imaginaria
      else if(i < f2Idx){
        real_part = (int32_t)binary_file[i];
        imaginary_part = (int32_t)binary_file[i + 1];
        i++;
      }

      // Luego siguen 2 de 4 bits, pero en un solo byte cada uno
      else if(i < f3Idx){
        // La parte alta es la real
        real_part = (int32_t)(binary_file[i] & 0xF0);

        // La parte baja es la imaginaria
        imaginary_part = (int32_t)(binary_file[i] & 0x0F);
      }

      // Luego un numero compuesto de dos elementos
      else{
        // | 2  |  | 2  |  | 2  |  | 2  |
        // Real1 - imag1 - Real2 - imag2

        // --------------------------------------------------------------------
        // Primer numero
        // --------------------------------------------------------------------

        // Parte real del primer numero
        real_part = (int32_t)(binary_file[i] & 0xC0);

        // Parte imaginaria del primer numero
        imaginary_part = (int32_t)(binary_file[i] & 0x30);

        // Si alguno de los numeros es negativo se el aplica un enmascaramiento
        if(real_part < 0)
          real_part = 0xFFFFFF00 | real_part;
        if(imaginary_part < 0)
          imaginary_part = 0xFFFFFF00 | imaginary_part;

        input_fft[fft_idx].r = real_part;
        input_fft[fft_idx].i = imaginary_part;

        fft_idx ++;

        // --------------------------------------------------------------------
        // Segundo numero
        // --------------------------------------------------------------------

        // Parte real del segundo numero
        real_part = (int32_t)(binary_file[i] & 0xC);

        // Parte imaginaria del segundo numero
        real_part = (int32_t)(binary_file[i] & 0x3);
      }

      // Si alguno de los numeros es negativo se el aplica un enmascaramiento
      if(real_part < 0)
        real_part = 0xFFFFFF00 | real_part;
      if(imaginary_part < 0)
        imaginary_part = 0xFFFFFF00 | imaginary_part;

      input_fft[fft_idx].r = real_part;
      input_fft[fft_idx].i = imaginary_part;

      printf("REAL PART %d\n", input_fft[i].r);
      printf("IMG PART %d\n", input_fft[i].i);
  	}

    // Compute the inverse FFT
    compute_fft(input_fft, output_fft);
  }

	fclose(input_binary);

	return 0;
}
