#pragma once

#ifndef BIGINT_H
#define BIGINT_H

#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cctype>


class BigInt {
private:
    std::string value; // 存储数字的绝对值，字符串形式，高位在前
    bool isNegative;   // 符号位，true 表示负数

    // 移除字符串表示中高位的冗余零（“00123” -> “123”），并处理“-0”和“0”的情况。
    void normalize() {
        // 去除高位的零
        size_t nonZeroPos = value.find_first_not_of('0');
        if (nonZeroPos == std::string::npos) { // 全为零
            value = "0";
            isNegative = false; // 强制将0设为正数
            return;
        }
        value = value.substr(nonZeroPos);
        // 如果值是"0"，确保符号为正
        if (value == "0") {
            isNegative = false;
        }
    }

    // 核心静态工具函数
    // 比较两个正数字符串的大小 (a, b 均为无符号数字字符串)
    static bool _absLess(const std::string& a, const std::string& b) {
        if (a.length() != b.length()) return a.length() < b.length();
        return a < b;
    }
    static bool _absLessOrEqual(const std::string& a, const std::string& b) {
        return !_absLess(b, a); // a <= b 等价于 !(b < a)
    }

    // 无符号加法 (a, b 均为无符号数字字符串)
    static std::string _unsignedAdd(const std::string& s1, const std::string& s2) {
        int n = s1.size(), m = s2.size();
        std::vector<int> a(std::max(n, m) + 1, 0), b(a.size(), 0), res(a.size() + 1, 0);

        for(int i = 0; i < n; i++) a[i] = s1[n - 1 - i] - '0';
        for(int i = 0; i < m; i++) b[i] = s2[m - 1 - i] - '0';
        int len = std::max(n, m) + 1;
        for(int i = 0; i < len; i++) {
            res[i] += a[i] + b[i];
            if(res[i] >= 10) {
                res[i+1] += res[i] / 10;
                res[i] %= 10;
            }
        }
        int j = len - 1;
        while(j > 0 && res[j] == 0) j--;
        std::string result = "";
        for(; j>=0; j--) result += static_cast<char>(res[j] + '0');
        return result;
    }

    // 无符号减法 (a >= b, a, b 均为无符号数字字符串)
    static std::string _unsignedSub(const std::string& s1, const std::string& s2) {
        int n = s1.size(), m = s2.size();
        std::vector<int> a(std::max(n, m), 0), b(a.size(), 0), res(a.size(), 0);

        for(int i = 0; i < n; i++) a[i] = s1[n - 1 - i] - '0';
        for(int i = 0; i < m; i++) b[i] = s2[m - 1 - i] - '0';
        int len = std::max(n, m);
        for(int i = 0; i < len; i++) {
            res[i] += a[i] - b[i];
            if(res[i] < 0) {
                res[i + 1]--;
                res[i] += 10;
            }
        }
        int j = len - 1;
        while(j > 0 && res[j] == 0) j--;
        std::string result = "";
        for(; j>=0; j--) result += static_cast<char>(res[j] + '0');
        return result;
    }

    // 无符号乘法
    static std::string _unsignedMul(const std::string& s1, const std::string& s2) {
        int n = s1.size(), m = s2.size();
        std::vector<int> a(n, 0), b(m, 0), res(n + m, 0);

        for(int i = 0; i < n; i++) a[i] = s1[n - 1 - i] - '0';
        for(int i = 0; i < m; i++) b[i] = s2[m - 1 - i] - '0';

        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                res[i + j] += a[i] * b[j];
            }
        }
        int len = n + m;
        for(int i = 0; i < len; i++) {
            res[i+1] += res[i] / 10;
            res[i] %= 10;
        }
        int idx = len - 1;
        while(idx > 0 && res[idx] == 0) idx--;
        std::string result = "";
        for(; idx>=0; idx--) result += static_cast<char>(res[idx] + '0');
        return result;
    }

    // 无符号除法 (返回商)，基于原代码的减法模拟除法
    static std::string _unsignedDiv(std::string s1, const std::string& s2) {
        std::string quotient = "", current = "";
        bool hasQuotientDigit = false;
        int n = s1.size();
        for(int i = 0; i < n; i++) {
            current += s1[i];
            int count = 0;
            // 当 current >= s2 时，不断减去 s2
            while (_absLessOrEqual(s2, current)) { // 即 !_absLess(current, s2)
                count++;
                current = _unsignedSub(current, s2);
            }
            if (count > 0) {
                hasQuotientDigit = true;
                quotient += static_cast<char>(count + '0');
            } else if (hasQuotientDigit) {
                quotient += '0';
            }
        }
        if (quotient.empty()) quotient = "0";
        return quotient;
    }
    // 获取无符号除法的余数
    static std::string _unsignedMod(std::string s1, const std::string& s2) {
        std::string current = "";
        int n = s1.size();
        for(int i = 0; i < n; i++) {
            current += s1[i];
            while (_absLessOrEqual(s2, current)) { // 即 current >= s2
                current = _unsignedSub(current, s2);
            }
        }
        // 规范余数，去除高位的零
        size_t pos = current.find_first_not_of('0');
        if (pos == std::string::npos) return "0";
        return current.substr(pos);
    }

public:
    BigInt() : value("0"), isNegative(false) {}
    BigInt(const std::string& str) {
        fromString(str);
    }
    BigInt(const char* str) {
        fromString(std::string(str));
    }
    BigInt(int num) {
        *this = std::to_string(num);
    }
    BigInt(long long num) {
        *this = std::to_string(num);
    }
    // 从字符串解析
    void fromString(const std::string& str) {
        std::string s = str;
        isNegative = false;
        if (!s.empty()) {
            if (s[0] == '-') {
                isNegative = true;
                s = s.substr(1);
            } else if (s[0] == '+') {
                s = s.substr(1);
            }
        }
        
        for (char c : s) {
            if (!std::isdigit(c)) {
                throw std::invalid_argument("Invalid character in BigInt string");
            }
        }
        
        value = s;
        normalize();
    }

    /*
    std::string to_string() const {
        return (isNegative && value != "0") ? ("-" + value) : value;
    }
    */
   
    operator std::string() const {
        return (isNegative && value != "0") ? ("-" + value) : value;
    }

    // 算术运算符重载
    BigInt operator-() const {
        BigInt result = *this;
        if (result.value != "0") {
            result.isNegative = !result.isNegative;
        }
        return result;
    }

    BigInt& operator+=(const BigInt& rhs) {
        *this = *this + rhs;
        return *this;
    }
    BigInt& operator-=(const BigInt& rhs) {
        *this = *this - rhs;
        return *this;
    }
    BigInt& operator*=(const BigInt& rhs) {
        *this = *this * rhs;
        return *this;
    }
    BigInt& operator/=(const BigInt& rhs) {
        *this = *this / rhs;
        return *this;
    }

    friend BigInt operator+(BigInt lhs, const BigInt& rhs);
    friend BigInt operator-(BigInt lhs, const BigInt& rhs);
    friend BigInt operator*(BigInt lhs, const BigInt& rhs);
    friend BigInt operator/(BigInt lhs, const BigInt& rhs);
    friend BigInt operator%(BigInt lhs, const BigInt& rhs);

    // 比较运算符重载
    bool operator<(const BigInt& rhs) const {
        if (isNegative != rhs.isNegative) {
            return isNegative; // 负数 < 正数
        }
        if (isNegative) { // 都是负数
            return _absLess(rhs.value, value); // |-a| < |-b| 则 a > b，所以需要反向
        } else { // 都是正数
            return _absLess(value, rhs.value);
        }
    }
    bool operator>(const BigInt& rhs) const { return rhs < *this; }
    bool operator<=(const BigInt& rhs) const { return !(*this > rhs); }
    bool operator>=(const BigInt& rhs) const { return !(*this < rhs); }
    bool operator==(const BigInt& rhs) const {
        return (isNegative == rhs.isNegative) && (value == rhs.value);
    }
    bool operator!=(const BigInt& rhs) const { return !(*this == rhs); }

    // 流输出
    friend std::ostream& operator<<(std::ostream& os, const BigInt& num) {
        os << static_cast<std::string>(num);
        return os;
    }
    friend std::istream& operator>>(std::istream& is, BigInt& num) {
        std::string s;
        is >> s;
        num.fromString(s);
        return is;
    }

    // 阶乘
    static BigInt factorial(const BigInt& n) {
        if (n.isNegative) {
            throw std::domain_error("Factorial is not defined for negative numbers.");
        }
        if (n == BigInt(0) || n == BigInt(1)) {
            return BigInt(1);
        }
        return n * factorial(n - BigInt(1));
    }
};

BigInt operator+(BigInt lhs, const BigInt& rhs) {
    BigInt result;
    if (lhs.isNegative == rhs.isNegative) {
        result.value = BigInt::_unsignedAdd(lhs.value, rhs.value);
        result.isNegative = lhs.isNegative;
    } else if (!lhs.isNegative && rhs.isNegative) { // a + (-b) = a - b
        if (BigInt::_absLess(lhs.value, rhs.value)) { // |a| < |b|
            result.value = BigInt::_unsignedSub(rhs.value, lhs.value);
            result.isNegative = true; // 结果为负
        } else {
            result.value = BigInt::_unsignedSub(lhs.value, rhs.value);
            result.isNegative = false;
        }
    } else { // lhs 负，rhs 正: (-a) + b = b - a
        if (BigInt::_absLess(rhs.value, lhs.value)) { // |b| < |a|
            result.value = BigInt::_unsignedSub(lhs.value, rhs.value);
            result.isNegative = true;
        } else {
            result.value = BigInt::_unsignedSub(rhs.value, lhs.value);
            result.isNegative = false;
        }
    }
    result.normalize();
    return result;
}

BigInt operator-(BigInt lhs, const BigInt& rhs) {
    // a - b = a + (-b)
    return lhs + (-rhs);
}

BigInt operator*(BigInt lhs, const BigInt& rhs) {
    BigInt result;
    result.value = BigInt::_unsignedMul(lhs.value, rhs.value);
    // 同号为正，异号为负
    result.isNegative = (lhs.isNegative != rhs.isNegative);
    result.normalize();
    return result;
}

BigInt operator/(BigInt lhs, const BigInt& rhs) {
    if (rhs == BigInt(0)) {
        throw std::runtime_error("Division by zero");
    }
    BigInt result;
    result.value = BigInt::_unsignedDiv(lhs.value, rhs.value);
    result.isNegative = (lhs.isNegative != rhs.isNegative);
    result.normalize();
    return result;
}

BigInt operator%(BigInt lhs, const BigInt& rhs) {
    if (rhs == BigInt(0)) {
        throw std::runtime_error("Modulo by zero");
    }
    BigInt result;
    result.value = BigInt::_unsignedMod(lhs.value, rhs.value);
    // 余数的符号与被除数相同
    result.isNegative = lhs.isNegative;
    result.normalize();
    return result;
}

#endif // BIGINT_H
