int switch_test_changing() {
  int a = 3;
  int b = 0;
  switch (a){
    case 1:
      b = 1;
    default:
      break;
    case 3:
      b += 3;
      a = 2;
    case 2 : 
      b = 100;
  }
  return b;
}