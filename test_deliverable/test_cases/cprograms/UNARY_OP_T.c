int unary_op() { 
  int a = 5;
  ++a;
  a++;
  --a;
  a--;
  a--;
  return -(~a);
}