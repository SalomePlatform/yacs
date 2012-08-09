       double precision function prdscl(A,B,n)
       integer i,n
       double precision A(n), B(n), S
C
       S = 0.0D0
       do i=1, n
          S = S + A(i) * B(i)
       enddo
C
       prdscl = S
       return
       end
