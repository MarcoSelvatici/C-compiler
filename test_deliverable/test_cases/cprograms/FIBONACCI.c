int memo[10 * 10 + 900 + 1]; // Memo of 1001 positions.
int size = 10 * 10 + 900 + 1;
int modulo = 100007;

int rec(int n) {
  if (n == 0 || n == 1) {
    return 1;
  }
  if (memo[n] != -1) {
    return memo[n];
  }
  memo[n] = (rec(n - 1) % modulo) + (rec(n - 2) % modulo);
  return memo[n];
}

int not_really_fib(int n) {
  // Initialize memo.
  int i = 0;
  while (i < size) {
    memo[i] = -1;
    i++;
  }
  return rec(n);
}
