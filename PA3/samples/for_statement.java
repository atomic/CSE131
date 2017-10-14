void foo() {
  int f = 4;
  boolean b = false;

  for (f = 1; b != true; f++)
    b = false;

  for (f = 1; b != true; f++) {
    int x = 1;
    b = false;
    i++;
  }
}