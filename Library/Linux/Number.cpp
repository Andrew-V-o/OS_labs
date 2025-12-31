#include "Number.h"
#include <stdexcept>

using namespace std;

namespace MyMath
{
    Number Number::operator/(const Number& other) const
    {
        if (other.value_ == 0.0)
        {
            throw runtime_error("Division by zero in MyMath::Number");
        }
        return Number(value_ / other.value_);
    }
}