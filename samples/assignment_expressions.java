public class AssignmentExpr {
    int i;
    boolean b;

    void foo() {
      int i2;
      boolean b2;

      i = 1;
      i = i2;

      b = true;
      b2 = b;
    }    
}
