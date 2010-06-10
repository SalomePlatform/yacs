      real function f1(x)
      real x
C
      real a
      common / C / a
C
      f1 = x / a
      return
      end
