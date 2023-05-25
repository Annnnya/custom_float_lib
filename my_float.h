#ifndef MY_FLOAT_H
#define MY_FLOAT_H

#include <cmath>

template <int MantissaBits, int ExponentBits>
class my_float {
private:
    bool sign;
    unsigned long long mantissa;
    int exponent;
    static constexpr int mantissa_bits = MantissaBits;
    static constexpr int exponent_bits = ExponentBits;
    static constexpr unsigned long long mantissa_mask = (1ULL << mantissa_bits) - 1;
    static constexpr int exponent_bias = (1 << (exponent_bits - 1)) - 1;

public:
    my_float(double value = 0.0) {
        sign = std::signbit(value);

        double abs_value = std::abs(value);

        if (abs_value != 0) {
            exponent = std::floor(std::log2(abs_value));
            double normalized_value = abs_value / std::pow(2, exponent);
            mantissa = std::llround((normalized_value - 1.0) * std::pow(2, mantissa_bits));
            exponent += exponent_bias;
        } else {
            mantissa = 0;
            exponent = 0;
        }
    }

    double to_double() const {
        double fraction = static_cast<double>(mantissa) / std::pow(2, mantissa_bits);
        double value = (1.0 + fraction) * std::pow(2, exponent - exponent_bias);

        return sign ? -value : value;
    }

    my_float operator+(const my_float& other) const {
        // Check for special cases: zero and NaN
        if (exponent == 0 && mantissa == 0) {
            return other;
        } else if (other.exponent == 0 && other.mantissa == 0) {
            return *this;
        }

        int this_exponent = exponent - exponent_bias;
        int other_exponent = other.exponent - other.exponent_bias;

        if (this_exponent < other_exponent) {
            return other + *this;
        }

        unsigned long long this_mantissa = mantissa | (1ULL << mantissa_bits);
        unsigned long long other_mantissa = other.mantissa | (1ULL << mantissa_bits);

        int shift = this_exponent - other_exponent;
        if (shift > mantissa_bits) {
            return *this;
        }

        bool carry = false;
        unsigned long long sum_mantissa = this_mantissa;
        if (shift > 0) {
            unsigned long long shift_mantissa = other_mantissa >> shift;
            carry = ((shift_mantissa & 1) == 1) && ((sum_mantissa & ((1ULL << shift) - 1)) != 0);
            sum_mantissa += shift_mantissa;
        } else {
            carry = ((other_mantissa & 1) == 1) && ((sum_mantissa & 1) == 1);
            sum_mantissa += other_mantissa;
        }

        int new_exponent = this_exponent;
        if (carry) {
            sum_mantissa >>= 1;
            new_exponent++;
        }

        my_float result;
        result.sign = sign;
        result.exponent = new_exponent + result.exponent_bias;
        result.mantissa = sum_mantissa & mantissa_mask;

        return result;
    }



    my_float operator-(const my_float& other) const {
        if (exponent == 0 && mantissa == 0) {
            return -other;
        } else if (other.exponent == 0 && other.mantissa == 0) {
            return *this;
        }

        int this_exponent = exponent - exponent_bias;
        int other_exponent = other.exponent - other.exponent_bias;

        if (this_exponent < other_exponent) {
            return -other + *this;
        }

        unsigned long long this_mantissa = mantissa | (1ULL << mantissa_bits);
        unsigned long long other_mantissa = other.mantissa | (1ULL << mantissa_bits);

        int shift = this_exponent - other_exponent;
        if (shift > mantissa_bits) {
            return *this;
        }

        bool borrow = false;
        unsigned long long diff_mantissa = this_mantissa;
        if (shift > 0) {
            unsigned long long shift_mantissa = other_mantissa >> shift;
            borrow = ((shift_mantissa & 1) == 1) && ((diff_mantissa & ((1ULL << shift) - 1)) != 0);
            diff_mantissa -= shift_mantissa;
        } else {
            borrow = ((other_mantissa & 1) == 1) && ((diff_mantissa & 1) == 1);
            diff_mantissa -= other_mantissa;
        }

        int new_exponent = this_exponent;
        if (borrow) {
            diff_mantissa >>= 1;
            new_exponent--;
        }

        my_float result;
        result.sign = sign;
        result.exponent = new_exponent + result.exponent_bias;
        result.mantissa = diff_mantissa & mantissa_mask;

        return result;
    }

    my_float operator*(const my_float& other) const {
        bool sign = this->sign ^ other.sign; // XOR the signs

        unsigned long long mantissa_a = this->mantissa;
        unsigned long long mantissa_b = other.mantissa;
        int exponent_a = this->exponent - this->exponent_bias;  // Subtract the exponent bias
        int exponent_b = other.exponent - other.exponent_bias; // Subtract the exponent bias

        unsigned long long mantissa_result = mantissa_a * mantissa_b;
        int exponent_result = exponent_a + exponent_b + this->exponent_bias; // Add the exponent bias back

        if ((mantissa_result >> MantissaBits) != 0) {
            mantissa_result >>= 1;
            exponent_result += 1;
        }

        unsigned long long rounded_mantissa = (mantissa_result + (1ULL << (MantissaBits - 1))) >> MantissaBits;

        if ((rounded_mantissa >> MantissaBits) != 0) {
            rounded_mantissa >>= 1;
            exponent_result += 1;
        }

        bool result_sign = sign;

        my_float result;
        result.sign = result_sign;
        result.mantissa = rounded_mantissa & this->mantissa_mask;
        result.exponent = exponent_result;

        return result;
    }

    my_float operator/(const my_float& other) const {
        if (other.exponent == 0 && other.mantissa == 0) {
            throw std::runtime_error("Division by zero");
        }

        int this_exponent = exponent - exponent_bias;
        int other_exponent = other.exponent - other.exponent_bias;

        unsigned long long this_mantissa = mantissa | (1ULL << mantissa_bits);
        unsigned long long other_mantissa = other.mantissa | (1ULL << mantissa_bits);

        bool quotient_sign = (sign != other.sign);

        int shift = this_exponent - other_exponent;
        this_mantissa <<= mantissa_bits;
        other_mantissa <<= mantissa_bits;

        if (shift > 0) {
            other_mantissa >>= shift;
        } else {
            this_mantissa >>= -shift;
        }

        unsigned long long quotient_mantissa = this_mantissa / other_mantissa;

        int quotient_exponent = this_exponent - other_exponent;

        my_float result;
        result.sign = quotient_sign;
        result.exponent = quotient_exponent + result.exponent_bias;
        result.mantissa = quotient_mantissa & mantissa_mask;

        return result;
    }


    my_float operator+=(const my_float& other) {
        *this = *this + other;
        return *this;
    }

    my_float operator-=(const my_float& other) {
        *this = *this - other;
        return *this;
    }

    my_float operator*=(const my_float& other) {
        *this = *this * other;
        return *this;
    }

    my_float operator/=(const my_float& other) {
        *this = *this / other;
        return *this;
    }

    bool operator<(const my_float& other) const {
        if (sign && !other.sign) {
            return true;
        } else if (!sign && other.sign) {
            return false;
        } else if (sign && other.sign) {
            return (-(*this) > -other);
        } else {
            if (exponent < other.exponent) {
                return true;
            } else if (exponent > other.exponent) {
                return false;
            } else {
                return (mantissa < other.mantissa);
            }
        }
    }

    bool operator>(const my_float& other) const {
        return (other < *this);
    }

    bool operator<=(const my_float& other) const {
        return !(*this > other);
    }

    bool operator>=(const my_float& other) const {
        return !(*this < other);
    }

    bool operator==(const my_float& other) const {
        return (sign == other.sign && mantissa == other.mantissa && exponent == other.exponent);
    }

    bool operator!=(const my_float& other) const {
        return !(*this == other);
    }

    my_float operator-() const {
        my_float result = *this;
        result.sign = !result.sign;
        return result;
    }

    my_float operator+(double other) const {
        return *this + my_float(other);
    }

    my_float operator-(double other) const {
        return *this - my_float(other);
    }

    my_float operator*(double other) const {
        return *this * my_float(other);
    }

    my_float operator/(double other) const {
        return *this / my_float(other);
    }

    my_float operator+=(double other) {
        *this = *this + other;
        return *this;
    }

    my_float operator-=(double other) {
        *this = *this - other;
        return *this;
    }

    my_float operator*=(double other) {
        *this = *this * other;
        return *this;
    }

    my_float operator/=(double other) {
        *this = *this / other;
        return *this;
    }

    my_float operator++(){
        *this = *this + my_float(1.0);
        return *this;
    }

    bool operator>(double other) const {
        return (my_float(other) < *this);
    }

    bool operator<(double other) const {
        return (my_float(other) > *this);
    }

    bool operator<=(double other) const {
        return !(*this > other);
    }

    bool operator>=(double other) const {
        return !(*this < other);
    }

    bool operator==(double other) const {
        return *this==my_float(other);
    }

    bool operator!=(double other) const {
        return *this != my_float(other);
    }

    my_float sin() const {
        if (is_zero()) {
            return my_float(0.0);
        }

        my_float x = *this;
        my_float y;

        y = x;
        my_float term = x;
        my_float sum = x;

        for (my_float i = my_float(1.0); i < 15.0; ++i) {
            term *= -(x * x) / ((i*2.0) * (i*2.0 + 1.0));
            sum += term;
        }

        return sum;
    }

    my_float cos() const {
        if (is_zero()) {
            return my_float(1.0);
        }

        my_float x = *this;
        my_float y;

        y = 1.0;
        my_float term = 1.0;
        my_float sum = 1.0;

        for (my_float i = my_float(1.0); i < 15.0; ++i) {
            term *= -(x * x) / ((i*2.0 - 1) * (i*2.0));
            sum += term;
        }

        return sum;
    }

    my_float tan() const {
        if (is_zero()) {
            return my_float(0.0);
        }

        my_float x = *this;
        return x.sin() / x.cos();
    }

    my_float pow(const my_float& expon) const {
        if (is_zero()) {
            return my_float(0.0);
        }

        my_float base = *this;
        my_float exp = expon;

        return exp * base.log().exp();
    }


    my_float abs() const {
        my_float result = *this;
        result.sign = false;
        return result;
    }

    my_float log() const {
        if (is_zero()) {
            return my_float(-std::numeric_limits<double>::infinity());
        }

        if (sign) {
            return my_float(std::numeric_limits<double>::quiet_NaN());
        }

        my_float x = *this;

        my_float sum = (x - 1.0) / (x + 1.0);
        my_float y = sum*sum;
        my_float term = y;

        for (double i = 3.0; i < 30.0; i+=2.0) {
            term *= y;
            sum += term / my_float(i);
        }

        return sum * 2.0;
    }

    my_float exp() const {
        if (is_zero()) {
            return my_float(1.0);
        }

        my_float x = *this;

        my_float term = 1.0;
        my_float sum = 1.0;

        for (double i = 1.0; i < 20.0 ; ++i) {
            term *= x / my_float(i);
            sum += term;
        }

        return sum;
    }

    bool is_zero() const {
        return mantissa == 0 && exponent == 0;
    }

};

template <char... Chars>
constexpr my_float<16, 8> operator""_mf() {
    constexpr int size = sizeof...(Chars);
    constexpr char charArray[size + 1] = {Chars..., '\0'};

    double value = 0.0;
    int decimalPos = -1;

    for (int i = 0; i < size; ++i) {
        char c = charArray[i];

        if (c == '.') {
            decimalPos = i;
        } else {
            int digit = c - '0';
            value = value * 10.0 + digit;
        }
    }

    if (decimalPos != -1) {
        int decimalPlaces = size - decimalPos - 1;
        value /= std::pow(10.0, decimalPlaces);
    }

    return my_float<16, 8>(value);
}


#endif
