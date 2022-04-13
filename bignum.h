#pragma once

// here you can include whatever you want :)
#include <string>
#include <stdint.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <exception>


#define SUPPORT_DIVISION 0 // define as 1 when you have implemented the division
#define SUPPORT_IFSTREAM 0 // define as 1 when you have implemented the input >>

// if you do not plan to implement bonus, just delete those lines
class BigNum final {
public:

    // constructors
    BigNum() {
        negative = false;
        representation.push_back(0);
    }

    BigNum(int64_t n) {
        if (n == 0) {
            negative = false;
            representation.push_back(0);
            return;
        }
        negative = n < 0;
        n = abs(n);
        while (n > 0) {
            representation.push_back(n % 10);
            n /= 10;
        }
        std::reverse(representation.begin(), representation.end());
    }

    explicit BigNum(const std::string& str) {

        std::string local = str;
        size_t startingIndex = 0;

        negative = local[0] == '-';

        if (!isdigit(local[0]) && ((local[0] == '+' || local[0] == '-') && local.size() == 1)) {    // if not a number or there is just the sign
            throw std::runtime_error("Invalid input.");
            return;
        }

        if (local[0] == '-' || local[0] == '+') {
            startingIndex++;
        }

        if (!std::all_of(local.begin() + startingIndex, local.end(), ::isdigit)) {
            
            throw std::runtime_error("Invalid input.");
            return;
        }

        if (local.empty() || (local.size() == 1 && local[0] == '0')) {
            negative = false;
            representation.push_back(0);
            return;
        }

        bool start = true;

        for (size_t i = startingIndex; i < local.size(); i++) {

            if(start && local[i] == '0') {
                continue;
            }
            start = false;

            if (std::isdigit(local[i])) {
                representation.push_back(toInt(local[i]));
            }
            else {
                representation.clear();
                negative = false;
                throw std::runtime_error("Invalid input.");
                return;
            }

        }

        if (representation.empty()) {
            representation.push_back(0);
            negative = false;
        }

    }

    // copy
    BigNum(const BigNum& other) = default;
    BigNum& operator=(const BigNum& rhs) = default;

    // unary operators

    const BigNum& operator+() const { 
        return *this;
    }

    BigNum operator-() const {
        BigNum other{ *this };
        if (other != BigNum{ 0 }) other.negative = !this->negative;
        return other;
    }

    // binary arithmetics operators
    BigNum& operator+=(const BigNum& rhs) {

       (*this) = (*this) + rhs;
        return (*this);

    }

    BigNum& operator-=(const BigNum& rhs) {

        (*this) = (*this) - rhs;
        return (*this);

    }

    BigNum& operator*=(const BigNum& rhs) {

        (*this) = (*this) * rhs;
        return (*this);
    
    }

#if SUPPORT_DIVISION == 1
    BigNum& operator/=(const BigNum& rhs); // bonus
    BigNum& operator%=(const BigNum& rhs); // bonus
#endif

private:
    // here you can add private data and members, but do not add stuff to 
    // public interface, also you can declare friends here if you want

    bool negative = false;
    std::vector<uint8_t> representation;

    uint8_t toInt(char c) {
        return  c - '0';
    }

    friend BigNum operator+(BigNum lhs, const BigNum& rhs);
    friend BigNum operator-(BigNum lhs, const BigNum& rhs);
    friend BigNum operator*(BigNum lhs, const BigNum& rhs);

    friend bool operator==(const BigNum& lhs, const BigNum& rhs);
    friend bool operator!=(const BigNum& lhs, const BigNum& rhs);
    friend bool operator<(const BigNum& lhs, const BigNum& rhs);
    friend bool operator>(const BigNum& lhs, const BigNum& rhs);
    friend bool operator<=(const BigNum& lhs, const BigNum& rhs);
    friend bool operator>=(const BigNum& lhs, const BigNum& rhs);

    friend std::ostream& operator<<(std::ostream& lhs, const BigNum& rhs);

};

BigNum operator+(BigNum lhs, const BigNum& rhs) {

    BigNum solution{ 0 };
    bool sameSigned = rhs.negative == lhs.negative;

    size_t lhsSize = lhs.representation.size();
    size_t rhsSize = rhs.representation.size();

    if (lhs == solution || rhs == solution) {
        if (lhs == solution && rhs != solution) return rhs;
        if (lhs != solution && rhs == solution) return lhs;
        if (lhs == solution && rhs == solution) return solution;
    }

    BigNum shorter;
    BigNum longer;

    if (lhsSize == rhsSize) {
        shorter = lhs;
        longer = rhs;
    }
    else {
        shorter = lhsSize > rhsSize ? rhs : lhs;
        longer = lhsSize < rhsSize ? rhs : lhs;
    }

    if (sameSigned) {

        while (shorter.representation.size() < longer.representation.size()) {
            shorter.representation.insert(shorter.representation.begin(), 0);
        }

        solution.negative = lhs.negative;
        solution.representation.resize(longer.representation.size());

        int8_t number = 0;
        int8_t leftOver = 0;

        size_t i = longer.representation.size() - 1;

        for (auto it = longer.representation.rbegin(); it != longer.representation.rend(); ++it, i--) {
            number = shorter.representation[i] + longer.representation[i] + leftOver;
            leftOver = number >= 10 ? 1 : 0;
            number %= 10;
            solution.representation[i] = number;
        }

        if (leftOver) {
            solution.representation.insert(solution.representation.begin(), leftOver);
        }

    }
    else {

        BigNum absoluteLeft;
        BigNum absoluteRight;

        if (lhs < 0) {
            absoluteLeft = lhs ;
            absoluteRight =  rhs ;
        }
        else {
            absoluteLeft = lhs ;
            absoluteRight = rhs ;
        }

        absoluteLeft.negative = false;
        absoluteRight.negative = false;

        if (absoluteLeft == absoluteRight) {
            return BigNum{ 0 };
        }

        BigNum& bigger = absoluteLeft < absoluteRight ? absoluteRight : absoluteLeft;
        BigNum& smaller = absoluteLeft > absoluteRight ? absoluteRight : absoluteLeft;

        solution.negative = absoluteLeft < absoluteRight ? rhs.negative : lhs.negative;
        solution.representation.resize(bigger.representation.size());

        while (smaller.representation.size() < bigger.representation.size()) {
            smaller.representation.insert(smaller.representation.begin(), 0);
        }

        int8_t number = 0;
        int8_t leftOver = 0;

        size_t i = longer.representation.size() - 1;

        for (auto it = bigger.representation.rbegin(); it != bigger.representation.rend(); ++it, i--) {

            int8_t top = bigger.representation[i];
            int8_t bot = smaller.representation[i] + leftOver;

            if (top < bot) {
                top += 10;
                leftOver = 1;
            }
            else {
                leftOver = 0;
            }

            number = top - bot;
            solution.representation[i] = number;

        }
        i = 0;
        if (solution.representation.size() > 1) {
            size_t first = 0;
            for (i = 0; i < solution.representation.size(); i++) {
                if (solution.representation[i] != 0) break;
                first++;
            }
        }
        solution.representation.erase(solution.representation.begin(), solution.representation.begin() + i);

    }

    return solution == BigNum{ 0 } ? BigNum{ 0 } : solution;

}

BigNum operator-(BigNum lhs, const BigNum& rhs) {
    
    BigNum right = rhs;
    right.negative = !right.negative;

    return lhs + right;

}

BigNum operator*(BigNum lhs, const BigNum& rhs) {

    BigNum top = lhs;
    BigNum bot = rhs;
    BigNum solution{ 0 };

    if (lhs == solution || rhs == solution) {
         return solution;
    }

    if (lhs.representation.size() > rhs.representation.size()) {
        top = lhs;
        bot = rhs;
    }
    else {
        top = rhs;
        bot = lhs;
    }

    solution.representation.resize(top.representation.size());

    std::vector<BigNum> intermediates;
    intermediates.resize(bot.representation.size());

    size_t k = 0;
    for (size_t i = bot.representation.size(); i-- > 0; ) {
        BigNum intermediate;
        intermediate.negative = false;
        intermediate.representation.resize(top.representation.size());
        uint8_t carryOver = 0;
        for (size_t j = top.representation.size(); j-- > 0; ) {
            uint8_t sum = top.representation[j] * bot.representation[i] + carryOver;
            carryOver = sum / 10;
            intermediate.representation[j] = (uint8_t)sum % 10;
        }
        if (carryOver) intermediate.representation.insert(intermediate.representation.begin(), carryOver);
        for (size_t l = 0; l < k; l++) {
            intermediate.representation.push_back(0);
        }
        intermediates[i] = intermediate;
        k++;
    }

    for (const BigNum& num : intermediates) {
        solution = solution + num;
    }

    solution.negative = lhs.negative == rhs.negative ? false : true;

    return solution == BigNum{ 0 } ? BigNum{ 0 } : solution;

}

#if SUPPORT_DIVISION == 1
BigNum operator/(BigNum lhs, const BigNum& rhs); // bonus
BigNum operator%(BigNum lhs, const BigNum& rhs); // bonus
#endif


bool operator==(const BigNum& lhs, const BigNum& rhs) {

    if (lhs.representation.size() != rhs.representation.size()) return false;

    if (lhs.negative != rhs.negative) return false;

    for (size_t i = 0; i < rhs.representation.size(); i++) {

        if (rhs.representation[i] != lhs.representation[i]) return false;

    }

    return true;

}

bool operator!=(const BigNum& lhs, const BigNum& rhs) {
    return !(lhs == rhs);
}

bool operator<(const BigNum& lhs, const BigNum& rhs) {

    if (lhs.negative && !rhs.negative) return true;

    if (!lhs.negative && rhs.negative) return false;

    bool bothNegative   = rhs.negative && lhs.negative;                                 // at this point both numbers are signed the same way
    bool sameDigitsNum  = lhs.representation.size() ==  rhs.representation.size();
    bool leftMoreDigits = lhs.representation.size() > rhs.representation.size();        // check if one has more digits

    if (!sameDigitsNum) {
        if (leftMoreDigits && bothNegative)    return true;
        if (!leftMoreDigits && !bothNegative)  return true;
        if (!leftMoreDigits && bothNegative)   return false;
        if (leftMoreDigits && !bothNegative)   return false;

    }

    if (lhs.representation.size() > rhs.representation.size()) return false;

    for (size_t i = 0; i < lhs.representation.size(); i++) {
        if (rhs.representation[i] > lhs.representation[i]) return bothNegative ? false : true;
        if (lhs.representation[i] > rhs.representation[i]) return bothNegative ? true  : false;
    }

    return false;

}

bool operator>(const BigNum& lhs, const BigNum& rhs) {

    if (lhs == rhs) return false;
    return !(lhs < rhs);

}

bool operator<=(const BigNum& lhs, const BigNum& rhs) {
    return (lhs == rhs) || (lhs < rhs);
}

bool operator>=(const BigNum& lhs, const BigNum& rhs) {
    return (lhs == rhs) || (lhs > rhs);
}


std::ostream& operator<<(std::ostream& lhs, const BigNum& rhs) {
    
    if (rhs.negative) lhs << '-';

    for (size_t i = 0; i < rhs.representation.size(); i++) {
        lhs << (char) (rhs.representation[i] + '0');
    }

    return lhs;

}

#if SUPPORT_IFSTREAM == 1
std::istream& operator>>(std::istream& lhs, BigNum& rhs); // bonus
#endif
