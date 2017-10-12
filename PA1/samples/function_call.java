void foo(){
    return 5;
}

void main() {
    int x;
    foo(x = 5);
    foo1(5);
    foo2(5, x = 10, 2);
    return x;
}
