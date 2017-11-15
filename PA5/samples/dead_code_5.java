void main() {
    int a_used = 1;
    int b_not_used = 0;
    int c_not_used = 0;
    int d_used = 0;
    int e_used = 0;

    if (a_used < 2) {
        a_used++;
    } else {
        a_used = a_used * 3;
    }

    int f_not_used = 0;
    int g_not_used = 0;
    return d_used + e_used;
}
