void main() {
	
    int i;
    int k = 1;
    int d = 2;

	boolean b = ((i + 4) / (k * 2) - d) == (d * k);
	b = ((i + 4) / (k * 2) - d) != (d * k);
	b += ((i + 4) / (k-- * 2) - d) >= (++d * k);

}