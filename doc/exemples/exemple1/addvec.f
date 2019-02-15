       subroutine addvec(C,A,B,n)
       integer i,n
       double precision A(n), B(n), C(n)
C
       do i=1, n
          C(i) = A(i) + B(i)
       enddo
C
       return
       end
