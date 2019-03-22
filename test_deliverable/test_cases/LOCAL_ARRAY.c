int local_array() {
  int arr[10];
  arr[0] = 1;
  arr[1] = 1;
  arr[2] = arr[0] + arr[1];
  arr[3] = arr[1] + arr[2];
  arr[4] = arr[2] + arr[3];
  arr[5] = arr[3] + arr[4];
  arr[6] = arr[4] + arr[5];
  arr[7] = arr[5] + arr[6];
  arr[8] = arr[6] + arr[7];
  arr[9] = arr[7] + arr[8];
  return arr[9];
}
