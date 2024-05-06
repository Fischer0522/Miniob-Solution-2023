//
// Created by fischer on 23-10-14.
//

#include "date.h"
#include <sstream>

bool Date::isDateValid() const
{
  static int m[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
  bool leap = (year%400==0 || year%100 && year%4==0);
  return year > 0
         && (month > 0) && (month <= 12)
         && (day > 0) && (day <= ((month == 2 && leap)?1:0) + m[month]);
}
bool Date::operator<(const Date &that) const {
  if (this->year < that.year) return true;
  if (this->year == that.year) {
    if (this->month < that.month) return true;
    if (this->month == that.month) {
      if(this->day < that.day) return true;
    }
  }
  return false;
}
bool Date::operator==(const Date &that) const {
  return this->year == that.year && this->month == that.month && this->day == that.day;
}
std::string Date::to_string() const {
  std::ostringstream oss;
  oss << year << '-';
  oss <<(month < 10 ? "0":"") << month << '-';
  oss <<(day < 10? "0":"") << day;
  return oss.str();
}
int Date::to_int() const {
  return (year * 10000) + (month * 100) + day;
}
void Date::parse_date_int(int date) {
  this->year = date /10000;
  date %=10000;
  this->month = date / 100;
  this->day = date % 100;
}
Date::Date(const char *str) {

  std::istringstream  ss(str);
  char delimiter;
  ss >> this->year;
  ss >> delimiter;
  ss >> this->month;
  ss >> delimiter;
  ss >> this->day;
  valid = isDateValid();
}
Date::Date(int date) {
  this->year = date /10000;
  date %=10000;
  this->month = date / 100;
  this->day = date % 100;
}
int Date::compare(const Date &that) const {
  if (*this < that) {
    return -1;
  } else if(*this == that) {
    return 0;
  } else {
    return 1;
  }
}
