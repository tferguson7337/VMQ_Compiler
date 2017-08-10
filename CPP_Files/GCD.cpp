// Sample program for C++ subset
// Computes the GCD of two integers


/* include statements are needed for real C++,
    but are simply considered comments in our subset
*/
#include <iostream>

int x,y;    //The two values whose GCD is to be computed
            

            // In the subset, a and b are passed by reference, but in
            //  real C++, they are passed by value. It doesn't matter
            //   in this program.

int gcd(int a,int b) {
    if ( b == 0 ) return a;
    else return gcd ( b, a % b );
}

int main() {
    cout << "Enter two integers: ";
    cin >> x >> y; cout << "The GCD is " << gcd(x,y) << endl;
}
