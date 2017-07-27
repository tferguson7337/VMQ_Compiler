#include <iostream>

int main()
{
	int a, b, c, d, e, f, g, x;

	a=b=c=d=e=f=g=x=0;
while(x < 3)
{
	while(a < 2)
	{
		cout << a << endl;
		while(b < 2)
		{
			cout << "\t" << b << endl;
			while(c < 2)
			{
				cout << "\t\t" << c << endl;
				while(d < 2)
				{
					cout << "\t\t\t" << d << endl;
					while(e < 2)
					{
						cout << "\t\t\t\t" << e << endl;
						while(f < 2)
						{
							cout << "\t\t\t\t\t" << f << endl;
							while(g < 2)
							{
								cout << "\t\t\t\t\t\t" << g << endl;
								g += 1;
							}
							f += 1;
						}
						e += 1;
					}
					d += 1;
				}
				c += 1;
			}
			b += 1;
		}
		a += 1;
	}
	a=b=c=d=e=f=g=0;
	x += 1;
}	
	cout << "Exit loop" << endl;

	return 0;
}
