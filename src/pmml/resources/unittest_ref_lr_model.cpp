void myTestFunc(double *param, double *res)
{
  ////////////////////////////// 
  //
  // File used by unit test
  // PMMLBasicsTest1::testExportLinearRegressionCpp
  //
  ////////////////////////////// 

  // Intercept
  double y = 3.83737;

  // Attribute : x6
  y += param[0]*0.475913;

  // Attribute : x8
  y += param[1]*0.142884;

  // Attribute : x6x8
  y += param[2]*-0.022019;

  // Attribute : x6x6x8
  y += param[3]*0.000536256;

  // Return the value
  res[0] = y;
}
