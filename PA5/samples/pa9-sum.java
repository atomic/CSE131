void main() {
    // 3) Allow user to input an integer value greater than 0, then output the
    // summation of that number.
    a = readIntFromSTDIN();

    int sum = 0;
    int i;
    for (i = 1; i <= a; i++) {
        sum += i;
    }

    printInt(sum);
}
