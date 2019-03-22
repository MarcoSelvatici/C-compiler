int pointers() {
  int* ptr_1, var_1 = 10;
  ptr_1 = &var_1;
  (*ptr_1)++;
  
  return var_1;
}