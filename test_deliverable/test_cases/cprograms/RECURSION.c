int rec(int n) {
  if (n <= 0) {
    return 0;
  }
  
  if (n % 2 != 0) {
    return 1 + rec(n-1);
  } else {
    //return rec(n - 2); // NON FAIL
    return 1 + rec(n - 2); // FAIL
  }
}