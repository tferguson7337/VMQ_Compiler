#include <iostream>

int main()
{
    int x;

    cout << "Enter a value between 1 and 10 (inclusive): x = ";
    cin >> x;

    if(x >= 1 && x <= 10)
    {
	if(x == 1 || x == 2)
	{
	    if(x == 1)
		cout << "x == 1" << endl;
	    else
		cout << "x == 2" << endl;
	}
	else
	{
	    if(x == 3 || x == 4)
	    {
		if(x == 3)
		    cout << "x == 3" << endl;
		else
		    cout << "x == 4" << endl;
	    }
	    else
	    {
		if(x == 5 || x == 6)
		{
		    if(x == 5)
			cout << "x == 5" << endl;
		    else
			cout << "x == 6" << endl;
		}
		else
		{
		    if(x == 7 || x == 8)
		    {
			if(x == 7)
			    cout << "x == 7" << endl;
			else
			    cout << "x == 8" << endl;
		    }
		    else
		    {
			if(x == 9)
			    cout << "x == 9" << endl;
			else
			    cout << "x == 10" << endl;
		    }
		}
	    }
	}
    }
    else
	cout << "x is out of range [1, 10]" << endl;

    return 0;
}
