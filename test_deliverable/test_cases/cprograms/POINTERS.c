int pointers() {
  int a = 10;
  int* b = &a;
  int c = a;
  int d = *b;
  *b = 5;
  return a + c + d;
}