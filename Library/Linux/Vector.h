#pragma once

#ifdef VECTOR_EXPORTS
#define VECTOR_API __declspec(dllexport)
#else
#define VECTOR_API __declspec(dllimport)
#endif

#include "Number.h" 
#include <cmath>

namespace MyMath
{
    class VECTOR_API Vector
    {
    private:
        double x_;
        double y_;

    public:
        Vector(const Number& x = Number(0.0), const Number& y = Number(0.0))
            : x_(x.getValue()), y_(y.getValue()) {
        }

        double getX() const { return x_; }
        double getY() const { return y_; }

        Vector operator+(const Vector& other) const;

        Number getPolarRadius() const;
        Number getPolarAngle() const;

        static const Vector ZERO_VECTOR;
        static const Vector ONE_VECTOR;
    };

    VECTOR_API std::ostream& operator<<(std::ostream& os, const Vector& v);
}