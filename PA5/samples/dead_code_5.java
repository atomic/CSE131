void main() {
    int a = 1;
    int b_not_used = 0;
    int c_not_used = 0;
    int d_used = 0;
    int e_used = 0;

    if (a < 2) {
        a++;
    } else {
        a = a * 3;
    }

    return d_used + e_used;
}
