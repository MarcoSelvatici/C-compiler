int unary_op() { 
  int a = 5;
  int b = a--;
  int c = --a;
  ++a;
  a++;
  --a;
  a--;
  a--;
  return -(~a) + b + b + c + c + c;
}