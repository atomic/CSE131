void foo() {
  boolean b;
  int i;

  b = true;
  i = 0;

  while (b)
    b = false;

  while (i < 3) {
    i++;
    break;
  }

}
