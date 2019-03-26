int constants(){
    int a = 0xAA;
    int b = 077;
    int c = 0b11;
    int d = a + 0X1F;
    int e = b + 023;
    int f = c + 0b101101;
    return a + b + c + d + e + f;
}