#include<stdio.h>
#include<math.h>

extern "C" {
  void print(double Val) {
    printf("Print: %g\n",Val);
  }
  void printi(int Val) {
    printf("Print: %d\n",Val);
  }

  int lengthf(double* vec) {
    long int N = lround(vec[0]);
    return N;
  }

  int lengthi(int* vec) {
    return vec[0];
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

  void addDoubleDoubleVectors(double** c_ptr, double* a, double* b) {
    // the length of the array is stored in the first slot
    long int N = lround(a[0]);
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = a[i] + b[i];
    }
  }
  
  void addIntDoubleVectors(double** c_ptr, int* a, double* b) {
    // the length of the array is stored in the first slot
    long int N = a[0];
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = a[i] + b[i];
    }
  }

  void addDoubleIntVectors(double** c_ptr, double* a, int* b) {
    addIntDoubleVectors(c_ptr,b,a);
  }
  
  void addIntIntVectors(int** c_ptr, int* a, int* b) {
    // the length of the array is stored in the first slot
    int N = a[0];
    *c_ptr = new int[N+1];
    int* c = *c_ptr;
    c[0] = N;
    for(int i=1;i<=N;i++) {
      c[i] = a[i] + b[i];
    }
  }

  void aMinusBDoubleDoubleVectors(double** c_ptr, double* a, double* b) {
    // the length of the array is stored in the first slot
    long int N = lround(a[0]);
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = a[i] - b[i];
    }
  }
  
  void aMinusBIntDoubleVectors(double** c_ptr, int* a, double* b) {
    // the length of the array is stored in the first slot
    long int N = a[0];
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = a[i] - b[i];
    }
  }

  void aMinusBDoubleIntVectors(double** c_ptr, double* a, int* b) {
    // the length of the array is stored in the first slot
    long int N = a[0];
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = a[i] - b[i];
    }
  }
  
  
  void aMinusBIntIntVectors(int** c_ptr, int* a, int* b) {
    // the length of the array is stored in the first slot
    int N = a[0];
    *c_ptr = new int[N+1];
    int* c = *c_ptr;
    c[0] = N;
    for(int i=1;i<=N;i++) {
      c[i] = a[i] - b[i];
    }
  }

  void mulDoubleDoubleVectors(double** c_ptr, double* a, double* b) {
    // the length of the array is stored in the first slot
    long int N = lround(a[0]);
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = a[i] * b[i];
    }
  }
  
  void mulIntDoubleVectors(double** c_ptr, int* a, double* b) {
    // the length of the array is stored in the first slot
    long int N = a[0];
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = a[i] * b[i];
    }
  }

  void mulDoubleIntVectors(double** c_ptr, double* a, int* b) {
    mulIntDoubleVectors(c_ptr,b,a);
  }
  
  void mulIntIntVectors(int** c_ptr, int* a, int* b) {
    // the length of the array is stored in the first slot
    int N = a[0];
    *c_ptr = new int[N+1];
    int* c = *c_ptr;
    c[0] = N;
    for(int i=1;i<=N;i++) {
      c[i] = a[i] * b[i];
    }
  }
  
void aDivBDoubleDoubleVectors(double** c_ptr, double* a, double* b) {
    // the length of the array is stored in the first slot
    long int N = lround(a[0]);
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = a[i] / b[i];
    }
  }
  
  void aDivBIntDoubleVectors(double** c_ptr, int* a, double* b) {
    // the length of the array is stored in the first slot
    long int N = a[0];
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = ((double)a[i]) / b[i];
    }
  }

  void aDivBDoubleIntVectors(double** c_ptr, double* a, int* b) {
    // the length of the array is stored in the first slot
    long int N = a[0];
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = (double)N;
    for(long int i=1;i<=N;i++) {
      c[i] = a[i] / ((double)b[i]);
    }
  }
  
  
  void aDivBIntIntVectors(double** c_ptr, int* a, int* b) {
    // the length of the array is stored in the first slot
    int N = a[0];
    *c_ptr = new double[N+1];
    double* c = *c_ptr;
    c[0] = N;
    for(int i=1;i<=N;i++) {
      c[i] = ((double)a[i]) / ((double)b[i]);
    }
  }


  void makeEmptyVector(int** a) {
    *a = new int[10];
  }
  
}
