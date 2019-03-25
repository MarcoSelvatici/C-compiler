int f1(int a);
int f2(int b, int c);
int f3(int d, int e, int f);
int nested_function_calls();

int nested_function_calls() {

	return f1(f2(f3(1,1,1),f2(1,1)));


}

int f1(int a){
	return 3 * a;
}

int f2(int b, int c) {
	return b + 2 * c;
}


int f3(int d, int e, int f){
	return d + e + f;
}
