#include <iostream>

//integer array
int int_arr[5];

//float array
float flt_arr[8];

int main()
{
    int_arr[0] = 0;
    int_arr[1] = 1;
    int_arr[2] = 2;
    int_arr[3] = 3;
    int_arr[4] = 4.0;

    cout << "int_arr[0] == " << int_arr[0] << endl;
    cout << "int_arr[1] == " << int_arr[1] << endl;
    cout << "int_arr[2] == " << int_arr[2] << endl;
    cout << "int_arr[3] == " << int_arr[3] << endl;
    cout << "int_arr[4] == " << int_arr[4] << endl;
    cout << endl;

    flt_arr[0] = 3.14;
    flt_arr[1] = 2.62;
    flt_arr[2] = 5.50;
    flt_arr[3] = 0.0;
    flt_arr[4] = 5;
    flt_arr[5] = 10.0;
    flt_arr[6] = 7.25;
    flt_arr[7] = 1516.19258;

    cout << "flt_arr[0] == " << flt_arr[0] << endl;
    cout << "flt_arr[1] == " << flt_arr[1] << endl;
    cout << "flt_arr[2] == " << flt_arr[2] << endl;
    cout << "flt_arr[3] == " << flt_arr[3] << endl;
    cout << "flt_arr[4] == " << flt_arr[4] << endl;
    cout << "flt_arr[5] == " << flt_arr[5] << endl;
    cout << "flt_arr[6] == " << flt_arr[6] << endl;
    cout << "flt_arr[7] == " << flt_arr[7] << endl;

    cout << "flt_arr[int_arr[2]] == " << flt_arr[int_arr[2]] << endl;

    cout << endl;

    return 0;
}
