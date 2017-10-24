// good
void foo() {
	int i;
}

// good
void bar() {
	int i;
	return;
}

// good
boolean baz() {
	int i;
	return true;
}

// bad - missing return type
int fubar() {
	int i;
}