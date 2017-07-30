#include <iostream>

int main()
{
	int x;

	x = 0;

	while(x < 15)
	{
		cout << "x is ";

		if(x % 2 == 0)
			cout << "even" << endl;
		else
			cout << "odd" << endl;

		x += 1;
	}

	return 0;
}
