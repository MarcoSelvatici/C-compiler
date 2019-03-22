int default_stat() {
  int a = 4;
  int b = 5;
  switch (a < b)
  {
    case 0:
      b = 1;
      break;
    case 2:
      b = 1;
      break;
    
    default:
      b = b * 2;
      break;
  }
  return b;
}