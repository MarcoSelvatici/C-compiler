int assigns() {
  int a = 2, b = 0, c = 2, d = 17, e = 32, f = 11;
  b += a;
  c *= a;
  a -= d;
  d %= a;
  e >>= 2;
  e &= 100;
  d |= 11;
  a ^= 31;
  b /= b;
  f <<= 1;
  return a + b + c + d + e + f;
}