public class ChainedExpr {
    void main() {
        
        boolean b = ((i + 4) / (k * 2) - d) == (d * k);
        b = ((i + 4) / (k * 2) - d) != (d * k);
        b += ((i + 4) / (k-- * 2) - d) >= (++d * k);

    }    
}
