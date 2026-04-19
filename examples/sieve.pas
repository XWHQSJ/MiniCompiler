begin
   integer n;
   integer i;
   integer j;
   integer is_prime;
   integer has_factor;
   read(n);
   i := 2;
   while i <= n do
      begin
         j := 2;
         has_factor := 0;
         while j * j <= i do
            begin
               if i - (i div j) * j = 0
                  then has_factor := 1
                  else has_factor := has_factor + 0;
               j := j + 1
            end;
         if has_factor = 0
            then write(i)
            else is_prime := 0;
         i := i + 1
      end
end
