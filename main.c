#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>

int i, j, n=4;

char* loadPng(const char* filename, int* width, int* height) {
  
  unsigned char* image = NULL;

  int error = lodepng_decode32_file(&image, width, height, filename);
  if(error)
  {
    printf("error %u: %s\n", error, lodepng_error_text(error));
  }
  
  return (image);
}

void writePng(const char* filename, const unsigned char* image, unsigned width, unsigned height){
    unsigned error;
    unsigned char* png;  size_t pngsize;
    LodePNGState state;
    lodepng_state_init(&state);
    error = lodepng_encode(&png, &pngsize, image, width, height, &state);
    if(!error) lodepng_save_file(png, pngsize, filename);
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
    lodepng_state_cleanup(&state);
    free(png);
}

void preparation(unsigned char *od, int ih, int iw){
    for (i=0;i<ih;i++)
        for (j=0;j<iw;j++){
            if (od[iw*i+j]<70) od[iw*i+j]=0;
            if (od[iw*i+j]>200) od[iw*i+j]=255;
        }
    return;
}

void FGauss(unsigned char *od, unsigned char *dc, int ih, int iw){
    int t=0;
    for (i=2;i<ih-1;i++)
        for (j=2;j<iw-1;j++)
            for (t=0;t<n;t++)
                dc[4*iw*i+j*4+t]=0.12*(od[iw*i*4+j*4+t] + od[iw*(4*i-4)+4*j+t] + od[iw*(4*i-4)+(4*j-4)+t] + od[iw*(4*i-4)+(4*j+4)+t] + od[iw*i*4+(4*j-4)+t] + od[iw*4*i+(4*4+4)+t] + od[iw*(4*i+4)+(4*j-4)+t] + od[iw*(4*i+4)+(4*j+4)+t] + od[iw*(4*i+4)+4*j+t]);
    return;
}

void colouring(unsigned char *dc, unsigned char *mcod,int ih, int iw, int n){
  for (i=0;i<iw*ih;i++){
    mcod[i*n]=20+dc[i]+0.5*dc[i-1];
    mcod[i*n+1]=45+dc[i];
    mcod[i*n+2]=170+dc[i];
    mcod[i*n+3]=255;
  }
  return;
}

int main() {
    char *input = "skull.png";
    int iw, ih, k=0;
    unsigned char *idata=loadPng(input,&iw,&ih);
    unsigned char *odata=malloc(ih*iw*sizeof(unsigned char));
    unsigned char *datacopy=malloc(ih*iw*sizeof(unsigned char));
    unsigned char *mcodata=malloc(ih*iw*n*sizeof(unsigned char));
    
    for(i=0;i<ih*iw;i++){
        odata[k]=0.299*idata[4*i]+0.587*idata[4*i+1]+0.114*idata[4*i+2];
        k++;
    }
    
    preparation(odata, ih, iw);
    FGauss(odata, datacopy, ih, iw);
    colouring(datacopy, mcodata, ih, iw, n);
    
    char *output = "result.png";
    writePng(output, mcodata, iw, ih);
    return 0;
}
