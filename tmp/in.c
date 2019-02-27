int f(int a) {
  if (a > 0)
    while (a > 0)
      a = a - 1;
  else
    return a;
  
  return a + 1;
}