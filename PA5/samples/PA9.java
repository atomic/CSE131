void main() {

    // 1) Allow user to input an integer then print out that integer value
    int a = readIntFromSTDIN();
    printInt(a);

    // 2) Allow user to input 2 integer values, then print out the larger value
    a = readIntFromSTDIN();
    int b = readIntFromSTDIN();

    // max(a, b)
    if (a > b) {
       printInt(a);
    } else {
       printInt(b);
    }

    // 3) Allow user to input an integer value greater than 0, then output the
    // summation of that number.
    a = readIntFromSTDIN();

    int sum = 0;
    int i;
    for (i = 1; i <= a; i++) {
        sum += i;
    }

//    printInt(sum);

}
