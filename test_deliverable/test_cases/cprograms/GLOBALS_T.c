int a;
int b = 3;

int f() {
  a = 2 * b + 3 + b - -b + ~b;
  return a + b;
}