#include <iostream>

int main()
{

	int x;

	cout << "Enter non-negative integer between 0 and 10: ";

	cin >> x;

	if(x > 5)
		cout << x << " is greater than 5" << endl;
	else
		cout << x << " is less than or equal to 5" << endl;
	
	return 0;
}
