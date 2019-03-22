int while_assign() {
  int a = 10;
  int b;
  while (b = 1){
    a++;
    if(a == 20)
      break;
  }
  while (b = 0)
    a = 0;

  return a;
}