int x = 2;

int scope_variables() {
  {
	   int x = 8;
  }
  return x;
}
