int collatz_recursive(int n) {
  if (n == 1) {
    return n;
  }

  if (n % 2 == 0) {
    return n + collatz_recursive(n / 2);
  } else {
    return n + collatz_recursive(n * 3 + 1);
  }
}

int collatz_iterative(int n) {
  int res = 1;
  while (n != 1) {
    res = res + n;
    if (n % 2 == 0) n = n / 2;
    else n = n * 3 + 1;
  }

  return res;
}

int main() {
  int tmp = 20 % 3;
  int n = 5 * 3 + (20 - tmp);
  return collatz_recursive(n) == collatz_iterative(n);
}