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
  uint64_t outputvec[len];
  for(int i = 0; i < len; i++){
    outputvec[i] = lhs.get_bits(i) + rhs.get_bits(i) + overflow;
    if(outputvec[i] <= lhs.get_bits(i)){ //overflow happens!
      overflow = 1;
    }else{
      overflow = 0;
    } 
  }
  BigInt output = BigInt(*outputvec, negativity);
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
  uint64_t outputvec[len];
  //negativity equal to the negativity of the BigInt with larger magnitude
  //len equal to the len of the BigInt with larger magnitude
  if(BigInt::compare_magnitudes(lhs, rhs) == 1){
    negativity = lhs.is_negative();
    for(int i = 0; i < len; i++){
      outputvec[i] = lhs.get_bits(i) - rhs.get_bits(i) - borrow;
      if(lhs.get_bits(i)>(rhs.get_bits(i) + borrow)){ //if the lhs is smaller, need to borrow
        borrow = 0;
      }else{
        borrow = 1;
      }
    }
  }else{
    negativity = rhs.is_negative();
    for(int i = 0; i < len; i++){
      outputvec[i] = rhs.get_bits(i) - lhs.get_bits(i) - borrow;
      if(rhs.get_bits(i)>(lhs.get_bits(i) + borrow)){ //if the lhs is smaller, need to borrow
        borrow = 0;
      }else{
        borrow = 1;
      }
    }
  }
  BigInt output = BigInt(*outputvec, negativity);
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
