int breaks(){
    int a = 0;
    while ( a < 11){
        while ( a < 12){
            if (a > 7)
               break;
            a++;
        }
        if (a > 8)
            break;
        a++;
    }
    return a;
}