int pow(int base, int exponent) {
    int i;

    int result = 1;
    for (i = 0; i < exponent; i++) {
        result = result * base;
    }

    return result;
}

void main() {
    int a = 1 + pow(2, 3) * 5;
}