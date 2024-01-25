#include <cassert>
#include <sstream>
#include <iomanip>
#include "bigint.h"

//default constructor, sets the sign to negative
BigInt::BigInt(){
  isNegative = false;
  values.push_back(0);
}

//constructor that takes in one value
BigInt::BigInt(uint64_t val, bool negative) : isNegative(negative){
  values.push_back(val);
}

//constructor that takes in multiple values
BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative) : isNegative(negative){
  //iterate through the list
  for(auto it = vals.begin(); it != vals.end(); it++){
    values.push_back(*it);
  }
}

//copy constructor
BigInt::BigInt(const BigInt &other) : values(other.values), isNegative(other.isNegative) {}

//no dynamically allocated memory, so there's no need for a destructor
BigInt::~BigInt()
{
}

//overload operator =
BigInt &BigInt::operator=(const BigInt &rhs)
{
  // TODO: implement
  values = rhs.values;
  isNegative = rhs.isNegative;
  return *this;
}

//return thr negativity
bool BigInt::is_negative() const {
  return isNegative;
}

//return thr value vector
const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  return values;
}

//ge the bit at the desired position
uint64_t BigInt::get_bits(unsigned index) const
{
  //check valdity of index
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

//unary minus
BigInt BigInt::operator-() const
{
  //create a new BigInt instance
  BigInt negate = BigInt(*this);
  negate.isNegative = !negate.isNegative;
  return negate;
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

//change the bigInt to hexadecimal represention (string)
std::string BigInt::to_hex() const
{
  std::stringstream ss;
  //load the sign (if needed) first
  if(isNegative){
    ss << "-";
  }
  //iterate through the vector in reverse order 
  for(auto it = values.rbegin(); it != values.rend(); ++it){
    if(it == values.rbegin()){
    //for the most significant bit, no need to setfill
    ss << std::hex << *it;
    }else{
      //for other digits, we should setfill
      ss << std::hex << std::setw(16) << std::setfill('0') << *it;
    }
  }
  return ss.str();
}

std::string BigInt::to_dec() const
{
  // TODO: implement
}

