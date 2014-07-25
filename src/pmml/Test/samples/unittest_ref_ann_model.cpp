#define ActivationFunction(sum) ( 1.0 / ( 1.0 + exp( -1.0 * sum )) )
void myTestFunc(double *param, double *res)
{
  ////////////////////////////// 
  //
  // File used by unit test
  // PMMLBasicsTest1::testExportNeuralNetworkCpp
  //
  ////////////////////////////// 

  int nInput   = 8;
  int nOutput   = 1;
  int nHidden  = 1;
  const int nNeurones  = 10;
  double myTestFunc_act[nNeurones];

  // --- Preprocessing of the inputs and outputs
  double myTestFunc_minInput[] = {
  0.099999, 25048.9, 89334.9, 89.5523, 1050, 
  760.001, 1400.02, 10950, 
  };
  double myTestFunc_minOutput[] = {
  77.8117,   };
  double myTestFunc_maxInput[] = {
  0.028899, 14419.8, 15180.8, 15.2866, 34.6793, 
  34.6718, 161.826, 632.913, 
  };
  double myTestFunc_maxOutput[] = {
  45.7061,   };

  // --- Values of the weights
  double myTestFunc_valW[] = {
  -1.74548, 6.96551, -1.26357, 0.753663, 0.00165366, 
  0.004725, 0.00996979, 0.178798, -0.180981, -0.173569, 
  0.0855967, 
  };
  // --- Constants
  int indNeurone = 0;
  int CrtW;
  double sum;

  // --- Input Layers
  for(int i = 0; i < nInput; i++) {
     myTestFunc_act[indNeurone++] = ( param[i] - myTestFunc_minInput[i] ) / myTestFunc_maxInput[i];
  }

  // --- Hidden Layers
  for (int member = 0; member < nHidden; member++) {
     int CrtW = member * ( nInput + 2) + 2;
     sum = myTestFunc_valW[CrtW++];
     for (int source = 0; source < nInput; source++) {
         sum += myTestFunc_act[source] * myTestFunc_valW[CrtW++];
       }
       myTestFunc_act[indNeurone++] = ActivationFunction(sum);
  }

  // --- Output
  for (int member = 0; member < nOutput; member++) {
    sum = myTestFunc_valW[0];
    for (int source = 0; source < nHidden; source++) {
      CrtW = source * ( nInput + 2) + 1;
      sum += myTestFunc_act[nInput+source] * myTestFunc_valW[CrtW];
    }
    myTestFunc_act[indNeurone++] = sum;
    res[member] = myTestFunc_minOutput[member] + myTestFunc_maxOutput[member] * sum;
  }
}
