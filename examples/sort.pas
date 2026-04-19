begin
   integer a;
   integer b;
   integer c;
   integer t;
   read(a);
   read(b);
   read(c);
   if a > b then
      begin t := a; a := b; b := t end
      else t := 0;
   if b > c then
      begin t := b; b := c; c := t end
      else t := 0;
   if a > b then
      begin t := a; a := b; b := t end
      else t := 0;
   write(a);
   write(b);
   write(c)
end
