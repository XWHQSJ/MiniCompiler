
begin
     integer k;
	 integer k;
     integer function f(n);
        begin
           integer  n;
            if n<=0  then  f:=1
            else   f:=n*f(n-1)
         end;
      read(m);
      k=f(m);
      write(k)
end