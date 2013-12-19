      SUBROUTINE myTestFunc(P0, P1, P2, P3, RES)
C --- *********************************************
C --- 
C ---  File used by unit test
C ---  PMMLBasicsTest1::testExportLinearRegressionFortran
C --- 
C --- *********************************************

      IMPLICIT DOUBLE PRECISION (P)
      DOUBLE PRECISION RES
      DOUBLE PRECISION Y

C --- Intercept
      Y = 3.83737;

C --- Attribute : x6
      Y += P[0]*0.475913;

C --- Attribute : x8
      Y += P[1]*0.142884;

C --- Attribute : x6x8
      Y += P[2]*-0.022019;

C --- Attribute : x6x6x8
      Y += P[3]*0.000536256;

C --- Return the value
      RES = Y 
      RETURN
      END
