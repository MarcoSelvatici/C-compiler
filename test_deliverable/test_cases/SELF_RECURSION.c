int self_recursion(int a) {
  if (a == 0) { return 42; }
  return self_recursion(a - 1);
}