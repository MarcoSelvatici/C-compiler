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
