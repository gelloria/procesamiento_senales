#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kiss_fftr.h"

#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

//number of coefficients
#define SAMPLES 64
//file size
#define FILE_LENGTH get_file_length()

/*S t r u c t u r e s*/

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

/*F u n c t i o n s*/

//Macro function to get input file length
int get_file_length(){
    FILE *file;
    file = fopen("input.txt", "r");
    size_t pos = ftell(file);    // Current position
    fseek(file, 0, SEEK_END);    // Go to end
    int length = ftell(file); // read the position which is the size
    fseek(file, pos, SEEK_SET);  // restore original position
    fclose(file);
    return length;
}

void TestFftReal(const char* title, const kiss_fft_scalar in[SAMPLES], kiss_fft_cpx out[SAMPLES / 2 + 1])
{
  kiss_fftr_cfg cfg;

  printf("%s\n", title);

  if ((cfg = kiss_fftr_alloc(SAMPLES, 0/*is_inverse_fft*/, NULL, NULL)) != NULL)
  {
    size_t i;

    kiss_fftr(cfg, in, out);
    free(cfg);

    for (i = 0; i < SAMPLES; i++)
    {
      printf(" in[%2zu] = %+f    ", i, in[i]);
      if (i < SAMPLES / 2 + 1)
        printf("out[%2zu] = %+f , %+f", i, out[i].r, out[i].i);
      printf("\n");
    }
  }
  else
  {
    printf("not enough memory?\n");
    exit(-1);
  }
}

int main(void)
{
    // txt input file import
    FILE *myFile;
    myFile = fopen("input.txt", "r");

    if (myFile == NULL){
        printf("Error Reading File\n");
        exit (0);
    }
    //read file into array
    float numberArray[SAMPLES];
    kiss_fft_scalar in[SAMPLES];
    float maxNumber = 0;

    //put this in a function
    for (int i = 0; i < SAMPLES; i++){
        fscanf(myFile, "%f,", &numberArray[i] );
        in[i] = numberArray[i];
        if (numberArray[i]>maxNumber){
            maxNumber = numberArray[i];
        }


    }

    printf(" Max Number is = %f \n", maxNumber);

    fclose(myFile);



  return 0;
}
