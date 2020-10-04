#include <cmath>

class Complex {
private:
    double re, im;

public:
    Complex(double number): re(number) {}
    Complex(double re_, double im_): re(re_), im(im_) {}

    bool operator==(const Complex& other) const {
        return (re == other.re) && (im == other.im);
    }

    bool operator!=(const Complex& other) const {
        return !(*this == other);
    }

    Complex operator+() const {
        return *this;
    }

    Complex operator-() const {
        return {-re, -im};
    }


    Complex operator+(const double number) const {
        return {re + number, im};
    }

    Complex operator-(const double number) const {
        return {re - number, im};
    }

    Complex operator/(const double number) const {
        return {re / number, im / number};
    }

    Complex operator*(const double number) const {
        return {re * number, im * number};
    }


    Complex operator+(const Complex& other) const {
        return {re + other.re, im + other.im};
    }

    Complex operator-(const Complex& other) const {
        return {re - other.re, im - other.im};
    }

    Complex operator*(const Complex& other) const {
        return {re * other.re - im * other.im, re * other.im + im * other.re};
    }

    Complex operator/(const Complex& other) const {
        return (*this * Complex(other.re, -other.im)) / (other.re * other.re + other.im * other.im);
    }


    double Re() const {
        return re;
    }

    double Im() const {
        return im;
    }
};

Complex operator+(const double lhs, const Complex& rhs) {
    return {lhs + rhs.Re(), rhs.Im()};
}

Complex operator-(const double lhs, const Complex& rhs) {
    return {lhs - rhs.Re(), rhs.Im()};
}

Complex operator*(const double lhs, const Complex& rhs) {
    return {lhs * rhs.Re(), lhs * rhs.Im()};
}

Complex operator/(const double lhs, const Complex& rhs) {
    return Complex(lhs) / rhs;
}

double abs(const Complex& z) {
    return std::sqrt(z.Re() * z.Re() + z.Im() * z.Im());
}
