begin
   integer a;
   integer b;
   integer t;
   read(a);
   read(b);
   while b > 0 do
      begin
         t := a - (a div b) * b;
         a := b;
         b := t
      end;
   write(a)
end
