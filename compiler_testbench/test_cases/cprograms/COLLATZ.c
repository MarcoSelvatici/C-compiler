int collatz() {
  int n = 33;
  int steps = 0;
  while (n > 1){
    steps++;
    if (n % 2 == 0) n /= 2;
    else if (n % 2 != 0) n = n*3 + 1;
  }
  return steps;
}