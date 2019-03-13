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

int fib(int n) {
  int arr[1001];
  arr[0] = 0;
  arr[1] = 1;
  for (int i = 2; i < 1001; i++) {
    arr[i] = (arr[i-1] + arr[i-2]) % modulo;
  }
  return arr[n];
}

int not_really_fib(int n) {
  // Initialize memo.
  int i = 0;
  while (i < size) {
    memo[i] = -1;
    i++;
  }
  return rec(n) + fib(n);
}
