begin
   integer x;
   integer n;
   integer result;
   integer i;
   read(x);
   read(n);
   result := 1;
   i := 0;
   while i < n do
      begin
         result := result * x;
         i := i + 1
      end;
   write(result)
end
