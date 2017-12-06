int sum(int a, int b, int c, int d, int e) {
    return a + b + c + d + e;
}

void main() {
    int v = readIntFromSTDIN();
    int x = readIntFromSTDIN();
    int w = readIntFromSTDIN();
    int y = readIntFromSTDIN();
    int z = readIntFromSTDIN();

    int total = sum(v, x, w, y, z);

    printInt(total);
}
