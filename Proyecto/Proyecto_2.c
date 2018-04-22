#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kiss_fftr.h"

#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

#define N 64

int import_txt_file(kiss_fft_scalar in[N]){

    FILE *myFile;
    myFile = fopen("testA.txt", "r");

    //read file into array
    float numberArray[N];
    int i;

    if (myFile == NULL){
        printf("Error Reading File\n");
        exit (0);
    }

    for (i = 0; i < N; i++){
        fscanf(myFile, "%f,", &numberArray[i] );
        in[i] = numberArray[i];
    }

    fclose(myFile);

    return 0;
}

int export_txt_file(const kiss_fft_scalar in[N], kiss_fft_cpx out[N]){

    FILE *myFile;
    myFile = fopen("testA_out.txt", "w");

    int i;
    if (myFile == NULL){
        printf("Error Reading File\n");
        exit (0);
    }

    for (i = 0; i < N; i++)
    {
      fprintf(myFile, "%d, %f , ", i, in[i]);
      if (i < N / 2 + 1){
        fprintf(myFile, "%f , %f \n", out[i].r, out[i].i);
      }else{
        fprintf(myFile,"\n");
      }
    }

    fclose(myFile);

    return 0;
}

void TestFftReal(const char* title, const kiss_fft_scalar in[N], kiss_fft_cpx out[N / 2 + 1])
{
  kiss_fftr_cfg cfg;

  printf("%s\n", title);

  if ((cfg = kiss_fftr_alloc(N, 0/*is_inverse_fft*/, NULL, NULL)) != NULL)
  {
    size_t i;

    kiss_fftr(cfg, in, out);
    free(cfg);

    for (i = 0; i < N; i++)
    {
      printf(" in[%2zu] = %+f    ", i, in[i]);
      if (i < N / 2 + 1)
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
  kiss_fft_scalar in[N];
  kiss_fft_cpx out[N / 2 + 1];
  size_t i;

  import_txt_file(in);

  TestFftReal("PARAMETERS OF THE RECIEVED FILES", in, out);

  export_txt_file(in, out);

  return 0;
}
