#include <iostream>

int add(int x, int y) { return x + y; }
int sub(int x, int y) { return x - y; }
int mul(int x, int y) { return x * y; }
int div(int x, int y) { return x / y; }
int mod(int x, int y) { return x % y; }

int main()
{
    float f_arr[5];

    f_arr[0] = 1.5;
    f_arr[1] = 7.34;
    f_arr[2] = 19.5;
    f_arr[3] = -0.05;
    f_arr[4] = -127.502;

    cout << "f_arr[0] == " << f_arr[0] << endl;
    cout << "f_arr[1] == " << f_arr[1] << endl;
    cout << "f_arr[2] == " << f_arr[2] << endl;
    cout << "f_arr[3] == " << f_arr[3] << endl;
    cout << "f_arr[4] == " << f_arr[4] << endl;

    cout << endl;

    cout << "f_arr[mod(sub(add(f_arr[1], f_arr[2]), div(f_arr[2], f_arr[1])), 5)] == "
	 <<  f_arr[mod(sub(add(f_arr[1], f_arr[2]), div(f_arr[2], f_arr[1])), 5)] << endl;
}
