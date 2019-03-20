int address_op() {
  int a;
  int b;
  int c = &a;
  int d = &b;
  if (c > d){
      return 0;
  }
  return 1;
}