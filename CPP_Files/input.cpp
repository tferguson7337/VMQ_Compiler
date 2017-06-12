#include <iostream>

int x;

int test(int y)
{
	int local;

	cout << "Enter in three return-separated integer values: ";

	cin >> local >> x >> y;

	cout << "TEST: local = " << local << endl;
	cout << "TEST: x = " << x << endl;
	cout << "TEST: y = " << y << endl;

	cout << "Function test is returning local variable" << endl;

	return local;
}

int main()
{
	int y;

	cout << "Enter two return-separated integer values: ";
	
	cin >> x >> y;

	cout << "MAIN: x = " << x << endl;

	cout << "MAIN: y = " << y << endl;

	cout << endl << "Calling test, passing y, assigning to x" << endl;

	x = test(y);

	cout << "Returned from test, x = " << x << " | y = " << y << endl;	

	return 0;	

}
