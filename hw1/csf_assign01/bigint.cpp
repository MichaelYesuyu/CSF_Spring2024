#include <cassert>
#include <sstream>
#include <iomanip>
#include <iostream>
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

uint64_t BigInt::get_len() const{
  return this->values.size();
}

BigInt BigInt::operator+(const BigInt &rhs) const
{
  // first remove leading zeros
  BigInt left = this->cleanData();
  BigInt right = rhs.cleanData();
  BigInt output;
  if(left.is_negative() == right.is_negative()){
    output = add_magnitudes(left, right);
  }else{
   output = subtract_magnitudes(left, right);
  }
  return output;
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // first remove leading zeros
  BigInt left = this->cleanData();
  BigInt right = rhs.cleanData();
  BigInt output = left + (-right);
  return output;
}

//unary minus
BigInt BigInt::operator-() const
{
  //create a new BigInt instance
  BigInt negate = BigInt(*this);
  if(!negate.is_zero()){
    negate.isNegative = !negate.isNegative;
  }
  return negate;
}

bool BigInt::is_bit_set(unsigned n) const
{
  // TODO: implement
  unsigned vecIndex = n / 64;
  unsigned bitPosition = n % 64;
  //Bit not set if the request bit is larger than the value of the BigInt
  if(vecIndex >= values.size()){
    return false;
  }
  uint64_t curVal = values[vecIndex];
  if(bitPosition != 0){
    //Get the lower bitPosition number of bits from the current value
    curVal = curVal & ~(~0UL << (bitPosition + 1));
    //Leaves curVal with all 0s except the bit that we want to check
    curVal = curVal >> (bitPosition);
  }
  return (curVal & 1) == 1;
}

BigInt BigInt::operator<<(unsigned n) const
{   
    //Throw exception if attempting to left shift negative number
    if(this->is_negative()){
      throw std::invalid_argument("Cannot left shift negative number");
    }
    //Make a copy of the current BigInt
    BigInt newBigInt = BigInt(*this);
    //no shift needed when n=0 or vector is empty
    if(n == 0 || newBigInt.values.size() == 0){
      return newBigInt;
    }
    //Multiples of 64 shifts the entire vector and inserts 0 at the end
    int indexShifts = n / 64;
    //The remainder is the number of bits that needs to be shifted left
    int bitShifts = n % 64;
    //The number of bits that will overflow into the next element of the vector due to the bit shift
    int overflow = 64 - bitShifts;

    //Add 0s for the elements due to shifts by powers of 64
    newBigInt.values.insert(newBigInt.values.begin(), indexShifts, 0);

    uint64_t curVal = 0;
    uint64_t curVal_overflow = 0;
    uint64_t extraElement = 0;
    //Performs the shift with overflow handling
    if(bitShifts != 0){
      for(auto it = newBigInt.values.rbegin(); it != newBigInt.values.rend(); it++){
          curVal = *it;
          curVal_overflow = curVal >> overflow;
          *it = curVal << bitShifts;
          //Store the most significant element's overflow to add later
          if(it == newBigInt.values.rbegin()){
              extraElement = curVal_overflow;
          }
          //Edit the next element's bits, - because it's reverse iterator
          else{
            *(it - 1) |= curVal_overflow;
          }
      }
    }
    if(extraElement != 0){
        newBigInt.values.push_back(extraElement);
    }
    return newBigInt;
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
  //remove leading 0s
  BigInt data = this->cleanData();
  std::stringstream ss;
  //load the sign (if needed) first
  if(data.isNegative){
    ss << "-";
  }
  //iterate through the vector in reverse order 
  for(auto it = data.values.rbegin(); it != data.values.rend(); ++it){
    if(it == data.values.rbegin()){
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

//check if the number is zero
bool BigInt::is_zero() const{
  for (uint64_t value : values){
    if(value != 0){
      return false;
    }
  }
  return true;
}

//clear any leading bits that is 0
BigInt BigInt::cleanData() const {
  BigInt out = BigInt(*this);
  //if the biggest digit is 0 and that is not the only digit
  while (out.values.back() == 0 && out.values.size() != 1){
      out.values.pop_back();
  }
  return out;
}

//use this method with cleaned data
BigInt BigInt::add_magnitudes(const BigInt &lhs, const BigInt &rhs){
  bool negativity = lhs.is_negative();
  //use overload to track 
  int overflow = 0;
  //let len be the largest of the sizes of the two value vectors
  int len = 0;
  if(BigInt::compare_magnitudes(lhs, rhs) == 1){
    len = lhs.get_len();
  }else{
    len = rhs.get_len();
  }
  BigInt output = BigInt();
  output.isNegative = negativity;
  for(int i = 0; i < len; i++){
    uint64_t curSum = lhs.get_bits(i) + rhs.get_bits(i) + overflow;
    //Detects that overflow occurs
    if(curSum < lhs.get_bits(i)){
      overflow = 1;
    }else{
      overflow = 0;
    }
    //Don't add new element for first iteration as values already has one element by default
    if(i == 0){
      output.values[0] = curSum;
    }else{
      output.values.push_back(curSum);
    }
  }
  //Handles the case where an extra element is needed due to overflow
  if(overflow == 1){
    output.values.push_back(1);
  }
  return output;
}

//use this method with cleaned data
BigInt BigInt::subtract_magnitudes(const BigInt &lhs, const BigInt &rhs){
  bool negativity;
  int len = 0;
  int borrow = 0;
  if(lhs.get_len() > rhs.get_len()){
    len = lhs.get_len();
  }else{
    len = rhs.get_len();
  }
  BigInt output = BigInt();
  //negativity equal to the negativity of the BigInt with larger magnitude
  //len equal to the len of the BigInt with larger magnitude
  if(BigInt::compare_magnitudes(lhs, rhs) == 1){
    negativity = lhs.is_negative();
    for(int i = 0; i < len; i++){
      uint64_t curVal = lhs.get_bits(i) - rhs.get_bits(i) - borrow;
      if(lhs.get_bits(i)>(rhs.get_bits(i) - borrow)){ //if the lhs is smaller, need to borrow
        borrow = 0;
      }else{
        borrow = 1;
      }
      //Don't add element for first iteration as vector starts off with one element inside (0)
      if(i == 0){
        output.values[0] = curVal;
      }else{
        output.values.push_back(curVal);
      }
    }
  }else{
    negativity = rhs.is_negative();
    for(int i = 0; i < len; i++){
      uint64_t curVal = rhs.get_bits(i) - lhs.get_bits(i) - borrow;
      if(rhs.get_bits(i)>(lhs.get_bits(i) - borrow)){ //if the lhs is smaller, need to borrow
        borrow = 0;
      }else{
        borrow = 1;
      }
      //Don't add element for first iteration as vector starts off with one element inside (0)
      if(i == 0){
        output.values[0] = curVal;
      }else{
        output.values.push_back(curVal);
      }
    }
  }
  output.isNegative = negativity;
  return output;
  
}

//return 1 if left has larger magnitude, 0 if right has larger magnitude
//use this method with cleaned data
int BigInt::compare_magnitudes(const BigInt &lhs, const BigInt &rhs){
  int leftlen = lhs.get_len();
  int rightlen = rhs.get_len();
  if(leftlen > rightlen){
    return 1;
  }else if(leftlen < rightlen){
    return 0;
  }else{
    for(int i = (leftlen - 1); i >= 0; i--){
      if(lhs.get_bits(i) > rhs.get_bits(i)){
        return 1;
      }else if(lhs.get_bits(i) < (rhs.get_bits(i))){
        return 0;
      }
    }
    return 1; //is they are the same, return 1;
  }
  return -1; //error message
}
