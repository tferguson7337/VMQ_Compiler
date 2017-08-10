#include <iostream>

int main()
{
    int arr[3];
    float usr;

    arr[0] = 5;
    arr[1] = 8;
    arr[2] = 0;
    
    cout << "A value of ~0.0, or a value between ~5.0 and ~8.0, will evaluate to TRUE" << endl;
    
    cout << "Enter a value: ";
    cin >> usr;

    if((usr == arr[2]) || (usr > arr[0] && usr < arr[1]))
	cout << "TRUE" << endl;
    else
	cout << "FALSE" << endl;

    return 0;
}
