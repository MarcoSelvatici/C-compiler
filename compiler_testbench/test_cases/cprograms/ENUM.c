enum en {a, b = 4, c, d = 2 * 3};

int f() {
  return a + b + c + d;
}