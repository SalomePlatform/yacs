      subroutine setcom(b)
      real b
C
      real a
      common / C / a
C
      a = b
C
      return
      end

      subroutine getcom(b)
      real b
C
      real a
      common / C / a
C
      b = a
C
      return
      end
