int for_loop(){
  int a;
  int b = 0;
  for (a = 10; a > 5; a--){ 
    if (a == 7)
      continue;
    b = b + a;
  }
  for (int i = 0; i < 10 ; ++i)
    if (b == 7)
      break;
    else
      b = b + i;
      
  return b; 
}