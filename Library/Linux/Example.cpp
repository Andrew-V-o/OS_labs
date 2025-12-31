#include <iostream>
#include <iomanip>
#include "Number.h"
#include "Vector.h"

using namespace std;
using namespace MyMath;

int main()
{
    cout << fixed << setprecision(2);

    cout << "1. Static Library (Number)" << endl;

    Number num_a = Number::Create(20.0);
    Number num_b = Number::Create(4.0);

    cout << "Number A: " << num_a << endl;
    cout << "Number B: " << num_b << endl;

    cout << "A + B = " << (num_a + num_b) << endl;
    cout << "A - B = " << (num_a - num_b) << endl;
    cout << "A * B = " << (num_a * num_b) << endl;
    cout << "A / B = " << (num_a / num_b) << endl;

    cout << "\n2. Dynamic Library (Vector)" << endl;

    Vector vec_p1 = Vector(Number(3.0), Number(4.0));
    Vector vec_p2 = Vector(Number(2.5), Number(1.5));
    const Vector vec_one = Vector::ONE_VECTOR;

    cout << "Vector P1: " << vec_p1 << endl;
    cout << "Vector P2: " << vec_p2 << endl;

    Vector vec_sum = vec_p1 + vec_p2;
    cout << "Sum: " << vec_sum << endl;

    Number radius = vec_p1.getPolarRadius();
    Number angle = vec_p1.getPolarAngle();

    cout << "\nPolar coordinates of P1:" << endl;
    cout << "Radius: " << radius << endl;
    cout << "Angle (Radians): " << angle << endl;

    return 0;
}