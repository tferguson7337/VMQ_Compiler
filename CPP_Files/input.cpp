#include <iostream>

int x[2];

int test(int y)
{
	int local;

	cout << "Enter in three return-separated integer values: ";

	cin >> local >> x[0] >> y;

	cout << "TEST: local = " << local << endl;
	cout << "TEST: x[0] = " << x[0] << endl;
	cout << "TEST: y = " << y << endl;

	cout << "Function test is returning local variable" << endl;

	return local;
}

int main()
{
	int y;

	cout << "Enter two return-separated integer values: ";
	
	cin >> x[0] >> y;

	cout << "MAIN: x[0] = " << x[0] << endl;

	cout << "MAIN: y = " << y << endl;

	cout << endl << "Calling test, passing y, assigning to x[0]" << endl;

	x[1] = test(y);

	cout << "Returned from test:\t";
	cout << "x[0] = " << x[0] <<  " | x[1] = " << x[1] << " | y = " << y << endl;	

	return 0;	

}
