int main (){
  int a = 10;
  if (a == 10){
    a = a - 1;
    if (a < 10){
      a = a - 1;
      if ( a % 2 == 0)
        if ( a % 2 == 1)
          return -1;
      else 
        return a;  
    }
  }
  return  2 * a;
}