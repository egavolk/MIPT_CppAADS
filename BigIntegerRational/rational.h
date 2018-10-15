#pragma once
#include <vector>
#include <string>
#include <iostream>

void ReverseString(std::string &s)
{
	std::string s1 = s;
	s = "";
	for (int i = 1; i <= (int)s1.size(); ++i)
		s += s1[s1.size() - i];
}

const std::string tostr(int x, const int k)
{
	std::string cur_s;
	for (int j = 0; (j < k) || (x > 0); j++)
	{
		int rem = x % 10;
		cur_s += ('0' + rem);
		x /= 10;
	}
	ReverseString(cur_s);
	return cur_s;
}

class BigInteger
{
private:
	const int K = 9;
	const int INF = 1e9;
	std::vector < int > a;
	bool minus;

	int min(int x, int y)
	{
		if (x < y)
			return x;
		return y;
	}

	int max(int x, int y)
	{
		if (x > y)
			return x;
		return y;
	}

	void ReverseVector(std::vector < int > &s)
	{
		std::vector < int > s1 = s;
		for (int i = 1; i <= (int)s1.size(); ++i)
			s[i - 1] = s1[s1.size() - i];
	}

	BigInteger &sum(BigInteger &x, const BigInteger &y, char sign)
	{
		bool ans_minus = false;
		BigInteger _y = y;
		_y.minus = !y.minus;
		if (((sign == '-') && (x < y)) || ((sign == '+') && (x < _y)))
			ans_minus = true;
		int z = 0;
		for (int i = 0; i < max(x.a.size(), y.a.size()); ++i)
		{
			int cur = 0, x1 = 0, x2 = 0;
			if (i < (int)x.a.size())
				x1 = (x.minus ? -1 : 1) * x.a[i];
			if (i < (int)y.a.size())
				x2 = (y.minus ? -1 : 1) * y.a[i];
			cur = z + x1 + (sign == '-' ? -x2 : x2);
			if (i < (int)x.a.size())
				x.a[i] = cur % INF;
			else
				x.a.push_back(cur % INF);
			z = cur / INF;
		}
		if (z != 0)
			x.a.push_back(z);
		x.minus = ans_minus;
		if (x.minus)
			for (int i = 0; i < (int)x.a.size(); ++i)
			{
				if (x.a[i] > 0)
				{
					x.a[i] -= INF;
					x.a[i + 1] += 1;
				}
			}
		else
			for (int i = 0; i < (int)x.a.size(); ++i)
			{
				if (x.a[i] < 0)
				{
					x.a[i] += INF;
					x.a[i + 1] -= 1;
				}
			}
		for (int i = 0; i < (int)x.a.size(); ++i)
			if (x.a[i] < 0)
				x.a[i] = -x.a[i];
		while ((x.a.size() > 1) && (x.a.back() == 0))
			a.pop_back();
		return x;
	}

	BigInteger mult(const BigInteger &x, const int &y, int n)
	{
		long long z = 0;
		BigInteger res;
		res.a.clear();
		for (int i = 0; i < n; ++i)
			res.a.push_back(0);
		for (int i = 0; i < (int)x.a.size(); ++i)
		{
			long long cur = z + 1ll * x.a[i] * y;
			res.a.push_back(cur % INF);
			z = cur / INF;
		}
		if (z != 0)
			res.a.push_back(z);
		while ((res.a.size() > 1) && (res.a.back() == 0))
			res.a.pop_back();
		return res;
	}

	std::string mult(std::string s, int x)
	{
		std::string res;
		int rem = 0;
		for (int i = s.size() - 1; i >= 0; --i)
		{
			int k = (s[i] - '0') * x + rem;
			res += (k % 10 + '0');
			rem = k / 10;
		}
		if (rem)
			res += (rem + '0');
		ReverseString(res);
		return res;
	}

	bool less(std::string &x, std::string &y)
	{
		if (x.size() < y.size())
			return true;
		if (x.size() > y.size())
			return false;
		for (int i = 0; i < (int)x.size(); ++i)
		{
			if (x[i] < y[i])
				return true;
			if (x[i] > y[i])
				return false;
		}
		return false;
	}

public:

	BigInteger(const BigInteger &other)
		: a(other.a)
		, minus(other.minus)
	{}

	BigInteger(int x = 0)
	{
		minus = false;
		a.clear();
		if (!x)
			a.push_back(0);
		if (x < 0)
		{
			minus = true;
			x *= -1;
		}
		for (; x > 0; x /= INF)
			a.push_back(x % INF);
	}

	BigInteger(const std::string &s)
	{
		int k = 0;
		a.clear();
		a.push_back(0);
		minus = false;
		if (s[0] == '-')
		{
			minus = true;
			k++;
		}
		int j = -1, step = 1;
		for (int i = 0; i < (int)s.size() - k; ++i)
		{
			if (!(i % 9))
			{
				step = 1;
				j++;
			}
			if (j == (int)a.size())
				a.push_back(0);
			a[j] = step * (s[s.size() - i - 1] - '0') + a[j];
			step *= 10;
		}
	}

	const std::string toString() const
	{
		std::string s;
		if (minus)
			s += '-';
		s += tostr(a.back(), 0);
		for (int i = a.size() - 2; i >= 0; --i)
		{
			s += tostr(a[i], K);
		}
		if (s.empty())
			s = "0";
		return s;
	}

	BigInteger& operator=(const BigInteger &b) {
		if (this == &b) {
			return *this;
		}
		a = b.a;
		minus = b.minus;
		return *this;
	}

	bool operator==(const BigInteger &b) const
	{
		return (a == b.a) && (minus == b.minus);
	}

	bool operator!=(const BigInteger &b) const
	{
		return !(*this == b);
	}

	bool operator<(const BigInteger &b) const
	{
		if ((!minus) && (b.minus))
			return false;
		if ((!b.minus) && (minus))
			return true;
		bool f = false;
		if ((minus) && (b.minus))
			f = true;
		if (a.size() < b.a.size())
			return !f;
		if (a.size() > b.a.size())
			return f;
		for (int i = a.size() - 1; i >= 0; --i)
		{
			if (a[i] < b.a[i])
				return !f;
			if (a[i] > b.a[i])
				return f;
		}
		return false;
	}

	bool operator<=(const BigInteger &b) const
	{
		return (*this == b) || (*this < b);
	}

	bool operator>(const BigInteger &b) const
	{
		return !(*this <= b);
	}

	bool operator>=(const BigInteger &b) const
	{
		return !(*this < b);
	}


	explicit operator bool() const 
	{
		return (a.size() != 1) || (a.back() != 0);
	}

	explicit operator double() const
	{
		double res = 0;
		for (int i = a.size() - 1; i >= 0; --i)
		{
			res *= INF;
			res += a[i];
		}
		if (minus)
			res *= -1;
		return res;
	}

	const BigInteger operator-() const
	{
		if ((a.size() == 1) && (a[0] == 0))
			return *this;
		BigInteger res = *this;
		if (minus)
			res.minus = false;
		else
			res.minus = true;
		return res;
	}

	const BigInteger operator +() const {
		return *this;
	}

	BigInteger &operator+=(const BigInteger &b)
	{
		*this = sum(*this, b, '+');
		return *this;
	}

	BigInteger &operator-=(const BigInteger &b)
	{
		*this = sum(*this, b, '-');
		return *this;
	}

	BigInteger &operator/=(const BigInteger &b)
	{
		BigInteger c = *this, d = b;
		c.minus = false, d.minus = false;
		if (c < d)
		{
			*this = 0;
			return *this;
		}
		std::string x = c.toString();
		std::string y = d.toString();
		std::string ans;
		for (int k = (int)x.size() - (int)y.size(); k >= 0; --k)
		{
			std::string z;
			for (int j = 0; j < (int)x.size() - k; ++j)
				if ((!z.empty()) || (x[j] != '0'))
					z += x[j];
			if (z.empty())
				z = "0";
			if (less(z, y))
			{
				if (!ans.empty())
					ans += '0';
			}
			else
			{
				int cur = 9;
				bool f = true;
				std::string y_cur;
				while (f)
				{
					y_cur = mult(y, cur);
					if (less(y_cur, z) || (y_cur == z))
						f = false;
					else
						cur--;
				}
				ans += ('0' + cur);
				BigInteger cur_d = BigInteger(z);
				cur_d -= BigInteger(y_cur);
				z = cur_d.toString();
				if (z == "0")
					z = "";
				for (int j = (int)x.size() - k; j < (int)x.size(); ++j)
					if (((int)z.size() != 0) || (x[j] != '0'))
						z += x[j];
				x = z;
				if (x.empty())
					x = "0";
			}
		}
		if (ans.empty())
			ans = "0";
		if ((minus != b.minus) && (ans != "0"))
			ans = "-" + ans;
		BigInteger res = ans;
		*this = res;
		return *this;
	}

	BigInteger &operator*=(const BigInteger &b)
	{
		BigInteger res;
		for (int i = 0; i < (int)b.a.size(); ++i)
		{
			BigInteger cur = mult(*this, b.a[i], i);
			res += cur;
		}
		if (minus != b.minus)
			res.minus = true;
		if (((int)res.a.size() == 1) && (res.a[0] == 0))
			minus = false;
		*this = res;
		return *this;
	}

	BigInteger &operator%=(const BigInteger &b)
	{
		BigInteger cur = *this;
		cur /= b;
		cur *= b;
		*this -= cur;
		return *this;
	}

	BigInteger &operator++()
	{
		return *this += 1;
	}

	BigInteger &operator--()
	{
		return *this -= 1;
	}

	BigInteger operator++(int)
	{
		BigInteger old = *this;
		*this += 1;
		return old;
	}

	BigInteger operator--(int)
	{
		BigInteger old = *this;
		*this -= 1;
		return old;
	}
};

std::istream &operator>>(std::istream &is, BigInteger &b)
{
	std::string s;
	is >> s;
	b = s;
	return is;
}

std::ostream &operator<<(std::ostream &os, const BigInteger &b)
{
	os << (BigInteger(b)).toString();
	return os;
}

const BigInteger operator+(const BigInteger &x, const BigInteger &y)
{
	BigInteger res = x;
	res += y;
	return res;
}

const BigInteger operator-(const BigInteger &x, const BigInteger &y)
{
	BigInteger res = x;
	res -= y;
	return res;
}

const BigInteger operator*(const BigInteger &x, const BigInteger &y)
{
	BigInteger res = x;
	res *= y;
	return res;
}

const BigInteger operator/(const BigInteger &x, const BigInteger &y)
{
	BigInteger res = x;
	res /= y;
	return res;
}

const BigInteger operator%(const BigInteger &x, const BigInteger &y)
{
	BigInteger res = x;
	res %= y;
	return res;
}


class Rational
{
private:
	BigInteger numerator, denominator;
	
public:
	Rational(const Rational &other)
		: numerator(other.numerator)
		, denominator(other.denominator)
	{}

	Rational(const BigInteger &x)
	{
		numerator = x;
		denominator = 1;
	}

	Rational(const BigInteger &a, const BigInteger &b)
	{
		BigInteger zero = 0;
		BigInteger gcd;
		if ((a == zero) || (b == zero))
			gcd = a + b;
		else
		{
			BigInteger c = a < 0 ? -a : a;
			BigInteger d = b < 0 ? -b : b;
			while (d % c != 0)
			{
				d = d % c;
				BigInteger e = d;
				d = c;
				c = e;
			}
			gcd = c;
			if (b < 0)
				gcd *= -1;
		}
		numerator = a / gcd;
		denominator = b / gcd;
	}
	
	Rational(const int x = 0)
	{
		numerator = x;
		denominator = 1;
	}

	const std::string toString() const
	{
		std::string s = numerator.toString() + '/' + denominator.toString();
		if ((s.back() == '1') && (s[s.size() - 2] == '/'))
		{
			s.pop_back();
			s.pop_back();
		}
		return s;
	}

	Rational& operator=(const Rational &b) {
		if (this == &b) {
			return *this;
		}
		numerator = b.numerator;
		denominator = b.denominator;
		return *this;
	}

	bool operator==(const Rational &b) const
	{
		return (numerator == b.numerator) && (denominator == b.denominator);
	}

	bool operator!=(const Rational &b) const
	{
		return !(*this == b);
	}

	bool operator<(const Rational &b) const
	{
		return numerator * b.denominator < b.numerator * denominator;
	}

	bool operator<=(const Rational &b) const
	{
		return (*this == b) || (*this < b);
	}

	bool operator>(const Rational &b) const
	{
		return !(*this <= b);
	}

	bool operator>=(const Rational &b) const
	{
		return !(*this < b);
	}

	const Rational operator-() const
	{
		Rational res = *this;
		res.numerator *= (-1);
		return res;
	}

	Rational &operator+=(const Rational &b)
	{
		numerator *= b.denominator;
		numerator += (b.numerator * denominator);
		denominator *= b.denominator;
		*this = Rational(numerator, denominator);
		return *this;
	}

	Rational &operator-=(const Rational &b)
	{
		numerator *= b.denominator;
		numerator -= (b.numerator * denominator);
		denominator *= b.denominator;
		*this = Rational(numerator, denominator);
		return *this;
	}

	Rational &operator*=(const Rational &b)
	{
		numerator *= b.numerator;
		denominator *= b.denominator;
		*this = Rational(numerator, denominator);
		return *this;
	}

	Rational &operator/=(const Rational &b)
	{
		numerator *= b.denominator;
		denominator *= b.numerator;
		*this = Rational(numerator, denominator);
		return *this;
	}

	std::string asDecimal(size_t precision = 0) const
	{
		BigInteger a = numerator, b = denominator;
		std::string res = (a / b).toString();
		if (precision == 0)
			return res;
		if (((a < 0) != (b < 0)) && (res == "0"))
		{
			res = "-" + res;
		}
		res += '.';
		
		
		BigInteger rem = (a % b) * 10;
		for (int i = 0; i < (int)precision; ++i)
		{
			res += (rem / b).toString().back();
			rem = (rem % b) * 10;
		}
		return res;
	}

	explicit operator double() const
	{
		double ans = double(numerator / denominator);
		BigInteger rem = numerator % denominator;
		int d = 5;
		double div = 1;
		for (int i = 0; i < d; ++i)
		{
			rem *= 1e9;
			div *= 1e9;
		}
		ans += double(rem / denominator) / div;
		return ans;
	}
};

const Rational operator+(const Rational &x, const Rational &y)
{
	Rational res = x;
	res += y;
	return res;
}

const Rational operator-(const Rational &x, const Rational &y)
{
	Rational res = x;
	res -= y;
	return res;
}

const Rational operator*(const Rational &x, const Rational &y)
{
	Rational res = x;
	res *= y;
	return res;
}

const Rational operator/(const Rational &x, const Rational &y)
{
	Rational res = x;
	res /= y;
	return res;
}