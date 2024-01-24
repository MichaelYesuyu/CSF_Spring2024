#include <cassert>
#include "bigint.h"

BigInt::BigInt(){
  isNegative = false;
  values.push_back(0);
}

BigInt::BigInt(uint64_t val, bool negative) : isNegative(negative){
  values.push_back(val);
}

BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative) : isNegative(negative){
  for(auto it = vals.begin(); it != vals.end(); it++){
    values.push_back(*it);
  }
}

BigInt::BigInt(const BigInt &other)
  // TODO: initialize member variables
{
}

BigInt::~BigInt()
{
}

BigInt &BigInt::operator=(const BigInt &rhs)
{
  // TODO: implement
}

bool BigInt::is_negative() const {
  return isNegative;
}

const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  return values;
}

uint64_t BigInt::get_bits(unsigned index) const
{
  if(index >= values.size()){
    return 0;
  }else{
    return values[index];
  }
}

BigInt BigInt::operator+(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement
  // Hint: a - b could be computed as a + -b
}

BigInt BigInt::operator-() const
{
  // TODO: implement
}

bool BigInt::is_bit_set(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator<<(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
}

int BigInt::compare(const BigInt &rhs) const
{
  // TODO: implement
}

std::string BigInt::to_hex() const
{
  // TODO: implement
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}

