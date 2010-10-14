extern "C" {
  void print(double Val);
  void printi(int Val);
  int lengthf(double* vec);
  
  double* makeVectorFloat(int length);
  double* makeEnumeratedVectorFloat(double start, double step, double end);
  int* makeEnumeratedVectorInteger(int start, int step, int end);
  void addDoubleDoubleVectors(double** c, double* a, double* b);
  void addIntDoubleVectors(double** c, int* a, double* b);
  void addDoubleIntVectors(double** c, int* a, double* b);
  void addIntIntVectors(int** c, int* a, int* b);
  void aMinusBDoubleDoubleVectors(double** c_ptr, double* a, double* b);
  void aMinusBIntDoubleVectors(double** c_ptr, int* a, double* b);
  void aMinusBDoubleIntVectors(double** c_ptr, double* a, int* b);
  void aMinusBIntIntVectors(int** c_ptr, int* a, int* b);
  void mulDoubleDoubleVectors(double** c_ptr, double* a, double* b);
  void mulIntDoubleVectors(double** c_ptr, int* a, double* b);
  void mulDoubleIntVectors(double** c_ptr, double* a, int* b);
  void mulIntIntVectors(int** c_ptr, int* a, int* b);
  void aDivBDoubleDoubleVectors(double** c_ptr, double* a, double* b);
  void aDivBIntDoubleVectors(double** c_ptr, int* a, double* b);
  void aDivBDoubleIntVectors(double** c_ptr, double* a, int* b);
  void aDivBIntIntVectors(double** c_ptr, int* a, int* b);
  
  void makeEmptyVector(int** a);
}
