int empty_cases() {
  int a = 3;
  switch (a) {
    default:
    case 1:
    case 2:
    case 3:
      a = 2;
  }
  return a;
}