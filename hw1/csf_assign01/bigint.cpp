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
  //copy member fields of the rhs object into this
  values = rhs.values;
  isNegative = rhs.isNegative;
  return *this;
}

//return the negativity
bool BigInt::is_negative() const {
  return isNegative;
}

//return the value vector
const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  return values;
}

//get the bit at the desired position
uint64_t BigInt::get_bits(unsigned index) const
{
  //check validity of index
  if(index >= values.size()){
    return 0;
  }else{
    return values[index];
  }
  return 0;
}

//get the length of the value vector, for static functions without access to the private field
uint64_t BigInt::get_len() const{
  return this->values.size();
}

//overload +, delegate to either add magnitude or subtract magnitude according to negativity
BigInt BigInt::operator+(const BigInt &rhs) const
{
  // first remove leading zeros with cleanData()
  BigInt left = this->cleanData();
  BigInt right = rhs.cleanData();
  BigInt output;
  //a + b
  if(left.is_negative() == right.is_negative()){
    output = add_magnitudes(left, right);
  //a - (-b)
  }else{
   output = subtract_magnitudes(left, right);
  }
  return output;
}

//delegate to the overloaded "+"
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
  //create a new BigInt instance, and then reverse negativity
  BigInt negate = BigInt(*this);
  if(!negate.is_zero()){
    negate.isNegative = !negate.isNegative;
  }
  return negate;
}

//check if the n th bit is 1
bool BigInt::is_bit_set(unsigned n) const
{
  unsigned vecIndex = n / 64;
  unsigned bitPosition = n % 64;
  //Bit not set if the request bit is larger than the value of the BigInt
  if(vecIndex >= values.size()){
    return false;
  }
  uint64_t curVal = values[vecIndex];
  curVal = curVal >> (bitPosition);

  return ((curVal % 2) == 1);
}

//overload <<
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

//overload *
BigInt BigInt::operator*(const BigInt &rhs) const
{
  //get the size of rhs, then multiply each digit of rhs with the whole of lhs
  unsigned numBits = rhs.values.size() * 64;
  BigInt answer = BigInt();
  //If either value is zero, just return zero
  if(this->is_zero() || rhs.is_zero()){
    return answer;
  }
  //If only one of the values is negative, answer will be negative, otherwise answer is positive
  if(this->isNegative ^ rhs.is_negative()){
    answer.isNegative = true;
  }else{
    answer.isNegative = false;
  }
  //multiply each valid bit with this, then add up
  for(unsigned i=0; i<numBits; i++){
    if(rhs.is_bit_set(i)){
      answer = answer + (*this << i);
    }
  }
  return answer;
}

//overload /
BigInt BigInt::operator/(const BigInt &rhs) const
{
  //Cannot divide by zero
  if(rhs.is_zero()){
    throw std::invalid_argument("Division by zero");
  }
  //if RHS is larger than LHS, integer division defaults to 0
  if(compare_magnitudes(*this, rhs) < 0){
    return BigInt();
  }
  //Determine sign of final answer
  bool result_isNeg;
  if(this->isNegative ^ rhs.isNegative){
    result_isNeg = true;
  }else{
    result_isNeg = false;
  }
  
  //rhs is the larger number
  BigInt upperBound = BigInt(*this);
  BigInt lowerBound = BigInt();
  BigInt dividend = BigInt(*this); 
  BigInt divisor = BigInt(rhs);
  BigInt result = BigInt();
  //Make all numbers positive as sign has already been accounted for
  upperBound.isNegative = false;
  lowerBound.isNegative = false;
  dividend.isNegative = false;
  divisor.isNegative = false;
  result.isNegative = false;

  result = dividend.division_search(lowerBound, upperBound, dividend, divisor);
  result.isNegative = result_isNeg;
  return result;
}

BigInt BigInt::division_search(BigInt lowerBound, BigInt upperBound, BigInt dividend, BigInt divisor){
  BigInt one = BigInt(1);
  BigInt mid = (upperBound + lowerBound).div_by_2();  
  BigInt result = mid * divisor;
  BigInt result_plus_one = (mid + 1) * divisor;
  //If result is less than or equal to dividend, but result + 1 is larger than dividend, then we have found our answer
  if((compare_magnitudes(result, dividend) != 1) && compare_magnitudes(result_plus_one, dividend) == 1){
    return mid;
  }
  //Result is larger than the dividend, divisor is too big
  else if(compare_magnitudes(result, dividend) > 0){
    return result.division_search(lowerBound, mid - 1, dividend, divisor);
  } 
  //Result is smaller than the dividend, divisor is too small
  else{
    return result.division_search(mid + 1, upperBound, dividend, divisor);
  }
}

//delegate to compare_magnitude
int BigInt::compare(const BigInt &rhs) const
{
  if((rhs.is_zero())&&(this->is_zero())){
    return 0;
  }else if(rhs.is_zero()){
    if(this->is_negative()){
      return -1;
    }else{
      return 1;
    }
  }else if(this->is_zero()){
    if(rhs.is_negative()){
      return 1;
    }else{
      return -1;
    }
  }
  //if they have different negativity
  if(rhs.is_negative() != this->is_negative()){
    if(rhs.is_negative()){
      return 1; //rhs is negative, while lhs is positive
    }else{
      return -1; //lhs is negative, rhs is positive
    }
  }
  // if has same negativity, call the compare_magnitude helper function
  int compare = compare_magnitudes(*this, rhs);
  if(rhs.is_negative()){
    if(compare == 1){
      return -1;
    }else if(compare == -1){
      return 1;
    }else{
      return 0;
    }
  }else{
    return compare;
  }
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

//convert to decimal form
std::string BigInt::to_dec() const
{
  //remove leading 0s
  BigInt data = this->cleanData();
  std::stringstream ss;
  //load the sign (if needed) first
  if(data.isNegative){
    ss << "-";
  }
  //create a bigint with 100
  uint64_t a = 100;
  BigInt hundred = BigInt(a);
  //store the 2-digit numbers
  std::vector<int> intVec;
  //divide this by 100 each time
  BigInt cur = BigInt(*this);
  while(!cur.is_zero()){
    BigInt result = cur - ((cur / hundred)* hundred);
    cur = cur/hundred;
    intVec.push_back(result.get_bits(0));
  }
  //used to check if we need to add zero to the 2-digit number
  int checkZero = 0;
  //iterate through the 2-digit numbers
  int len = intVec.size();
  for(int i = 0; i<len; i++){
    if((intVec[len - i - 1] < 10)&&(checkZero == 1)){ //add a zero if the 2-digit number is actually 1-digit long AND it is not the first 2-digit number
      ss << "0";
    }
    ss << intVec[len - i - 1];
    checkZero = 1;
  }
  if((ss.str()).empty()){
    ss << "0";
  }
  return ss.str();
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
  while ((out.values.back() == 0)&&(out.values.size() != 1)){
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
    len = (lhs.get_len() + 1);
  }else{
    len = (rhs.get_len() + 1);
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
  //if(overflow == 1){
  //  output.values.push_back(1);
  //}
  return output;
}

//use this method with cleaned data
BigInt BigInt::subtract_magnitudes(const BigInt &lhs, const BigInt &rhs){
  //If lhs and rhs have the same magnitude, return 0
  if(compare_magnitudes(lhs, rhs) == 0){
    return BigInt();
  }
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
      if(lhs.get_bits(i)>=(rhs.get_bits(i) + borrow)){ //if the lhs is smaller, need to borrow
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
      if(rhs.get_bits(i)>=(lhs.get_bits(i) + borrow)){ //if the lhs is smaller, need to borrow
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

//use this method with cleaned data
int BigInt::compare_magnitudes(const BigInt &lhs, const BigInt &rhs){
  BigInt left = lhs.cleanData();
  BigInt right = rhs.cleanData();
  int leftlen = left.get_len();
  int rightlen = right.get_len();
  if(leftlen > rightlen){
    return 1;
  }else if(leftlen < rightlen){
    return -1;
  }else{
    for(int i = (leftlen - 1); i >= 0; i--){
      if(left.get_bits(i) > right.get_bits(i)){
        return 1;
      }else if(left.get_bits(i) < (right.get_bits(i))){
        return -1;
      }
    }
    return 0; //is they are the same, return 0;
  }
}

BigInt BigInt::div_by_2() const{
  if(this->values.size() == 0){
    return *this;
  }
  BigInt newBigInt = BigInt(*this);
  for(auto it = newBigInt.values.begin(); it != newBigInt.values.end(); ++it){
    //Right shift the current element by 2
    *it = *it >> 1;
    //If not the last element, check to see if the more significant element has a 1 in the last bit (shifted down)
    if(((it+1) != newBigInt.values.end()) && *(it+1) % 2 == 1){
      //Set the most significant bit of the lower value to 1
      (*it) |= (1UL << 63);
    }
  }
  if(newBigInt.is_zero()){
    newBigInt.isNegative = false;
  }
  return newBigInt;
}
