int short_circuit(){
	int x = 2;
	(0 &&  x++);
	(1 &&  x++);
	(0 ||  x++);
	(1 ||  x++);
	return x;
}
