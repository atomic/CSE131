void main() {
    // 2) Allow user to input 2 integer values, then print out the larger value
    a = readIntFromSTDIN();
    int b = readIntFromSTDIN();

    // max(a, b)
    if (a > b) {
       printInt(a);
    } else {
       printInt(b);
    }
}
