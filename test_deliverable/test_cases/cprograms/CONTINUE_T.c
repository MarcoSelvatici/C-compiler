int continues(){
    int a = 0;
    int b = 0;
    while (a < 100){
        a++;
        if ((a % 2) == 0)
            continue;
        b++;
    }

    return b;
}