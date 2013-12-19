      SUBROUTINE myTestFunc(rw,r,tu,tl,hu,hl,l,kw,yhat)
C --- *********************************************
C --- 
C ---  File used by unit test
C ---  PMMLBasicsTest1::testExportNeuralNetworkFortran
C --- 
C --- *********************************************
      IMPLICIT DOUBLE PRECISION (V)
      DOUBLE PRECISION rw
      DOUBLE PRECISION r
      DOUBLE PRECISION tu
      DOUBLE PRECISION tl
      DOUBLE PRECISION hu
      DOUBLE PRECISION hl
      DOUBLE PRECISION l
      DOUBLE PRECISION kw
      DOUBLE PRECISION yhat

C --- Preprocessing of the inputs
      VXNrw = ( rw - 0.099999D0 ) / 0.028899D0
      VXNr = ( r - 25048.9D0 ) / 14419.8D0
      VXNtu = ( tu - 89334.9D0 ) / 15180.8D0
      VXNtl = ( tl - 89.5523D0 ) / 15.2866D0
      VXNhu = ( hu - 1050D0 ) / 34.6793D0
      VXNhl = ( hl - 760.001D0 ) / 34.6718D0
      VXNl = ( l - 1400.02D0 ) / 161.826D0
      VXNkw = ( kw - 10950D0 ) / 632.913D0

C --- Values of the weights
      VW1 = -1.74548
      VW2 = 6.96551
      VW3 = -1.26357
      VW4 = 0.753663
      VW5 = 0.00165366
      VW6 = 0.004725
      VW7 = 0.00996979
      VW8 = 0.178798
      VW9 = -0.180981
      VW10 = -0.173569
      VW11 = 0.0855967

C --- hidden neural number 1
      VAct1 = VW3
     1      + VW4 * VXNrw
     1      + VW5 * VXNr
     1      + VW6 * VXNtu
     1      + VW7 * VXNtl
     1      + VW8 * VXNhu
     1      + VW9 * VXNhl
     1      + VW10 * VXNl
     1      + VW11 * VXNkw

      VPot1 = 1.D0 / (1.D0 + DEXP(-1.D0 * VAct1))

C --- Output
      VOut = VW1
     1    + VW2 * VPot1

C --- Pretraitment of the output
      yhat = 77.8117D0 + 45.7061D0 * VOut;

C --- 
      RETURN
      END
