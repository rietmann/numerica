#include<stdio.h>
#include<math.h>

extern "C" {
  void print(double Val) {
    printf("Print: %g\n",Val);
  }
  void printi(int Val) {
    printf("Print: %d\n",Val);
  }
  
  double* makeVectorFloat(int length) {
    return new double[length];
  }
  
  double* makeEnumeratedVectorFloat(double start, double step, double end) {
    int i;
    int count = floor((end-start)/step)+1;
    double* vector = new double[count+1];
    vector[0] = count;
    for(i=0;i<count;i++) {
      vector[i+1]=start+i*step;
    }
    return vector;
  }

  int* makeEnumeratedVectorInteger(int start, int step, int end) {
    int i;
    int count = floor((end-start)/step)+1;
    int* vector = new int[count+1];
    vector[0] = count;
    for(i=0;i<count;i++) {
      vector[i+1]=start+i*step;
    }
    return vector;
  }
}
