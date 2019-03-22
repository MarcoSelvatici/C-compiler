int pointers() {
  int val_1 = 10, val_2 = 11, val_3;
  int* ptr_1 = &val_1;
  int* ptr_2 = ptr_1;
  int* ptr_3;
  ptr_3 = &val_3;
  *ptr_3 = val_2;
  *ptr_2 = val_1;
  *ptr_1 = val_3;

  return val_1 < val_2;
}