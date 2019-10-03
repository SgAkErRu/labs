#include "bitstring.h"
#include <string>
#include <stdint.h>

bool BitString::CheckForBinary(const std::string &str) const noexcept // проверка содержимого битовой строки, чтобы оно было двоичным
{
    uint8_t len = static_cast<uint8_t>(str.length());
    for (uint8_t i = 0; i < len; i++)
    {
        if (str[i] != '0' && str[i] != '1') return false;
    }
    return true;
}

void BitString::CallCheck(const std::string &str) const // проверка содержимого битовой строки, чтобы оно было двоичным
{
    if (str.length() > 128) throw overflow_exception();
    if (!CheckForBinary(str)) throw not_binary_exception();
}

uint64_t BitString::BinaryString_toInt(const std::string &str) // перевожу битовую строку в 64-битное десятичное число
{
    uint64_t res = 0;
    uint64_t m = 1;
    uint8_t len = static_cast<uint8_t>(str.length());
    for(uint8_t i = 0; i < len; i++)
    {
        res += (str[i] == '1') ? (m << (len - i - 1)) : 0;
    }
    return res;
}

std::string BitString::GetOptimizedBinary(const std::string &str) const
{
    bool mask = false;
    uint64_t i = 0;
    uint64_t len = str.length();
    std::string temp = str;
    while (!mask && i < len)
    {
        if (str[i] == '1') {mask = true; temp.erase(0,i);}
        i++;
    }
    if (i == len) temp = "0";
    return temp;
}

std::string BitString::toString() const noexcept // перевожу десятичные числа из полей в строку с двоичным кодом
{
    std::stringstream ss;
    uint64_t m = 1;
    for (int i = 64; i > 0; --i)
    {
        ss << ((F1 & (m << (i-1))) ? 1 : 0);
    }
    for (int i = 64; i > 0; --i)
    {
        ss << ((F2 & (m << (i-1))) ? 1 : 0);
    }
    std::string str = GetOptimizedBinary(ss.str());
    return str;
}

BitString& BitString::operator&=(const BitString &b)
{
    F1 &= b.F1;
    F2 &= b.F2;
    return *this;
}

BitString operator&(const BitString &a, const BitString &b) // побитовое and
{
    BitString t = a;
    t &= b;
    return t;
}

BitString& BitString::operator|=(const BitString &b)
{
    F1 |= b.F1;
    F2 |= b.F2;
    return *this;
}

BitString operator|(const BitString &a, const BitString &b) // побитовое or
{
    BitString t = a;
    t |= b;
    return t;
}

BitString& BitString::operator^=(const BitString &b)
{
    F1 ^= b.F1;
    F2 ^= b.F2;
    return *this;
}

BitString operator^(const BitString &a, const BitString &b) // побитовое xor
{
    BitString t = a;
    t ^= b;
    return t;
}

BitString BitString::operator~() // побитовое not
{
    F1 = ~F1;
    F2 = ~F2;
    return *this;
}

BitString& BitString::operator<<=(const uint8_t &i)
{
    std::string str = toString();
    uint8_t len = static_cast<uint8_t>(str.length());
    std::string strnew;
    if (len+i > 128)
    {
        strnew.assign(len,'0');
        strnew.replace(0,len-i,str.substr(i,len));
    } else {
        strnew.assign(len+i,'0');
        strnew.replace(0,len,str);
    }
    *this = strnew;
    return *this;
}

BitString BitString::operator<<(const uint8_t &i) // побитовый сдвиг влево
{
    BitString t = *this;
    t <<= i;
    return t;
}

BitString& BitString::operator>>=(const uint8_t &i)
{
    std::string str = toString();
    uint8_t len = static_cast<uint8_t>(str.length());
    std::string strnew;
    if (len+i > 128)
    {
        strnew.assign(len,'0');
        strnew.replace(i,len-i,str.substr(0,len-i));
    } else {
        strnew.assign(len+i,'0');
        strnew.replace(i,len,str);
    }

    *this = strnew;
    return *this;
}

BitString BitString::operator>>(const uint8_t &i) // побитовый сдвиг вправо
{
    BitString t = *this;
    t >>= i;
    return t;
}

int BitString::count_of_SingleBit() const noexcept // перевожу десятичные числа из полей в строку с двоичным кодом
{
    std::string str = toString();
    uint8_t res = 0;
    res = std::count(str.begin(), str.end(), '1');
    return res;
}

bool operator==(const BitString &a, const BitString &b)
{
    return (a.count_of_SingleBit() == b.count_of_SingleBit());
}
bool operator!=(const BitString &a, const BitString &b)
{
    return !(a == b);
}
bool operator<(const BitString &a, const BitString &b)
{
    return (a.count_of_SingleBit() < b.count_of_SingleBit());
}
bool operator>=(const BitString &a, const BitString &b)
{
    return !(a<b);
}

bool operator>(const BitString &a, const BitString &b)
{
    return (a.count_of_SingleBit() > b.count_of_SingleBit());
}

bool operator<=(const BitString &a, const BitString &b)
{
    return !(a>b);
}

bool is_included(const BitString &a, const BitString &b) // включен ли a в b
{
    std::string s1, s2;
    s1 = a.toString();
    s2 = b.toString();
    return (s2.find(s1) != std::string::npos);
}

std::ostream& operator<<(std::ostream& t, const BitString &r)
{
    std::string s = r.toString();
    return (t << s);
}
std::istream& operator>>(std::istream& t, BitString &r)
{
    std::string str;
    t >> str;
    r = str;
    return t;
}
