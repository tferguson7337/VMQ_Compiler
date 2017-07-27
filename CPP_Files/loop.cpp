#include <iostream>

int print(int x)
{
	cout << "x = " << x << endl;
}

int main()
{
	int x;
	x = 0;

	while(x < 10)
	{
		print(x);
		x += 2;
	}
	
	cout << "Loop exit." << endl;

	while(x != 0)
	{
		print(x);
		x -= 1;
	}

	cout << "Loop exit." << endl;

	while(x <= 10)
	{
		print(x);
		x += 1;
	}
		
	cout << "Loop exit." << endl;

	while(x >= 0)
	{
		print(x);
		x-=1;
	}

	cout << "Loop exit." << endl;
	
	while(x == (-1))
	{
		print(x);
		x = x * x;
		print(x);
	}

	cout << "Loop exit." << endl;

	return 0;
}
