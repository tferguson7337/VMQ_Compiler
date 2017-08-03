#include <iostream>

int x, y, z;

int add(int a, int b) { return a + b; }

int sub(int a, int b) { return a - b; }

int mul(int a, int b) { return a * b; }

// div(int, int) is already defined as part of cstdlib
int divide(int a, int b) { return a / b; }
	
int mod(int a, int b) { return a % b; }

int print(int zval) { cout << "z == " << zval << endl << endl; }

int main()
{
    x = 8; y = 3;

    cout << "START:  x = 8 | y = 3" << endl << endl;

    cout << "z <- sub(7, 2)" << endl;
    z = sub(7, 2);
    print(z);
	
    cout << "z <- add(x, y)" << endl;
    z = add(x, y);
    print(z);

    cout << "z <- sub(x, y)" << endl;
    z = sub(x, y);
    print(z);

    cout << "z <- mul(x, y)" << endl;
    z = mul(x, y);
    print(z);

    cout << "z <- divide(x, y)" << endl;
    z = divide(x, y);
    print(z);

    cout << "z <- mod(x, y)" << endl;
    z = mod(x, y);
    print(z);

    cout << "z <- ((add(x+y, y*5) + add(x-z, x*z)) * mod(x%y, (z*x)-y))" << endl;
    z = (add(x+y, y*5) + add(x-z, x*z)) * mod(x%y, (z*x)-y);
    print(z);

    cout << "z <- divide(mul(add(x, y), sub(z, y*y*x)), mod(x*x*x - (x*y), z))" << endl;
    z = divide(mul(add(x, y), sub(z, y*y*x)), mod(x*x*x - (x*y), z));
    print(z);

    return 0;
}
