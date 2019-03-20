int* a, *p = 1;
int c = 10;

int f() {
  int b = 2;
  a = b;
  a = &b;
  *a = 7;
  p = &c;
  return b + *a + *p; 
}