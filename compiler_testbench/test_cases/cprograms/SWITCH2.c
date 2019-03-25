int switch_(){
  int i;
  int a = 5;
  switch(a){
	  case 1:
		  for(i = 0; i < 100; i++){
				a++;
			}
			break;
		case 2:
			a = 10;
			break;

		default:
			a = 50;
	}
	return a;
}
