int scope_variables() {
  int b = 1;
  int c;
  {
    int b = 2;
    c = b;
  }
  int d = b;
  return c + d;
}