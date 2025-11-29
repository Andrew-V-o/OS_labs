#include "Vector.h"
#include <cmath>

using namespace std;

namespace MyMath
{
    Vector Vector::operator+(const Vector& other) const
    {
        return Vector(Number(x_ + other.x_), Number(y_ + other.y_));
    }

    Number Vector::getPolarRadius() const
    {
        return Number(sqrt(x_ * x_ + y_ * y_));
    }

    Number Vector::getPolarAngle() const
    {
        return Number(atan2(y_, x_));
    }

    const Vector Vector::ZERO_VECTOR = Vector(ZERO, ZERO);
    const Vector Vector::ONE_VECTOR = Vector(ONE, ONE);

    ostream& operator<<(ostream& os, const Vector& v)
    {
        os << "(" << v.getX() << ", " << v.getY() << ")";
        return os;
    }
}