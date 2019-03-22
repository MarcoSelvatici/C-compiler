int switch_test_assign() {
  int a = 3;
  int b = 0;
  switch (b = 10){
    case 0:
      a = 10;
      break;
    case 1:
      a = 20;
      break;
    case 10: 
      a = 30;
      break;
    default:
      a = 40; 
  }
  return a;
}