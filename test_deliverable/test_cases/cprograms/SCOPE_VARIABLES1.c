int scope_variables() {
  int a = 1, b = 2, c = 3, d, e, f[2];
  while (a < 10){
    int b = 10;
    a++;
    d = b;
    f[0] = 3;
    f[1] = a;
  }
  for (int c = 0; c < 10; c++){
    int a = 15;
    e = c;
    int f[2];
    f[0] = 7;
    f[1] = a;
  }

  return a + b + c + d + e + f[0] + f[1];
}