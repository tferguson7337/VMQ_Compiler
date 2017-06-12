#include <iostream>

int main()
{

	int a, b, c, d, e, f, g;

	a=b=c=d=e=f=g=0;

	a = 5 * 2;

	b = 3 - 4;

	c = 8 / 5 * 3;

	d = (3 + 5) / 2 - 1;

	e = 15 % 7 * (15 - 2) / 2;

	f = 30 - c - (b * a);

	cout << "a = 5 * 2 = " << a << endl;

	cout << "b = 3 - 4 = " << b << endl;

	cout << "c = 8 / 5 * 3 = " << c << endl;

	cout << "d = (3 + 5) / 2 - 1 = " << d << endl;
	
	cout << "e = 15 % 7 * (15 - 2) / 2 = " << e << endl;

	cout << "f = 30 - c - (b * a) = " << f << endl;

	g = 5;

	cout << "g = " << g << endl;

	g = g * c * e * ((a - (-b)) / 2);
	
	cout << "g = g * c * e * ((a - (-b)) / 2) = " << g << endl;

	f += (-25);

	cout << "f += (-25) = " << f << endl;

	g -= (g/7) + f * (a + g / e);

	cout << "g -= (g/7) + f * (a + g / e) = " << g << endl;

	return 0;
}
