#include <iostream>

float castToFlt(int i) { float f; return f = i; }

int castToInt(float f) { int i; return i = f; }

int incrementIntV1(int i) { return i += 1; }

int incrementIntV2(int i) { return i += 1.0; }

float incrementFltV1(float f) { return f += 1.0; }

float incrementFltV2(float f) { return f += 1; }

int main()
{
    int i;
    float f;

    i = 15;
    f = 5.92;

    cout << "START: ";
    cout << "i == " << i;
    cout << " || ";
    cout << "f == " << f;
    cout << endl << endl;
    
    cout << "(float)i == " << castToFlt(i) << endl;
    cout << "(int)f == " << castToInt(f) << endl << endl;

    cout << "Incrementing i: " << incrementIntV1(i) << endl;
    cout << "Incrementing i again: " << incrementIntV2(i) << endl << endl;

    cout << "Incrementing f: " << incrementFltV1(f) << endl;
    cout << "Incrementing f again: " << incrementFltV2(f) << endl << endl;

    cout << "(float)i * f == " << castToFlt(i) * f << endl;
    cout << "i * (int)f == " << i * castToInt(f) << endl;
    cout << "(float)i * (int)f == " << castToFlt(i) * castToInt(f) << endl << endl;

    cout << "i == " << i << " || " << "f == " << f << endl;
    
    cout << "(((float)i - f) * (int)f) / (float)i == " << ((castToFlt(i) - f) * castToInt(f)) / castToFlt(i) << endl;
    
    return 0;
}
