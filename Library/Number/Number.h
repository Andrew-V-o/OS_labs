#pragma once
#include <iostream>
#include <stdexcept>

namespace MyMath
{
    class Number
    {
    private:
        double value_;

    public:
        explicit Number(double val = 0.0) : value_(val) {}

        static Number Create(double val) { return Number(val); }

        Number operator+(const Number& other) const { return Number(value_ + other.value_); }
        Number operator-(const Number& other) const { return Number(value_ - other.value_); }
        Number operator*(const Number& other) const { return Number(value_ * other.value_); }
        Number operator/(const Number& other) const;

        double getValue() const { return value_; }

        friend std::ostream& operator<<(std::ostream& os, const Number& n)
        {
            os << n.value_;
            return os;
        }
    };

    const Number ZERO = Number(0.0);
    const Number ONE = Number(1.0);
}