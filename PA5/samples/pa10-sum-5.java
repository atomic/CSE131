int sum(int a, int b, int c, int d, int e) {
    int z = a + b + c + d + e;
    return z;
}

void main() {

    int p = 1;
    int q = 2;
    int r = 3;
    int s = 4;
    int t = 5;
    t = sum(p,q,r,s,t);

    printInt(t);
}
