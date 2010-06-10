      real function f2(x)
      real x
C
      real a
      common / C / a
C
      f2 = x * a
      return
      end
