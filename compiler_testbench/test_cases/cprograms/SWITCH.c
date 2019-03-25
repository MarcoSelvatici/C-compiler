int switch_() {
  int a = 3;
  int b = 0;
  switch (a) {
    case 1:
      b = 1;
      break;
    case 2:
      b = 2;
      break;
    case 3:
      b = 3;
      break;

    default:
      return 0;
  }
  return b;
}
