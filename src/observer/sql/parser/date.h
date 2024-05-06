//
// Created by fischer on 23-10-14.
//
#include <string>
#include <sstream>
#include <iomanip>
class Date
{
public:
  Date() = default;
  Date(const char *str);
  Date(int data);
  // Date(int year,int month,int day);

  bool        isValid() const { return valid; }
  bool        operator<(const Date &that) const;
  bool        operator==(const Date &that) const;
  int         compare(const Date &that) const;
  std::string to_string() const;
  int         to_int() const;
  void        parse_date_int(int date);
  bool        isDateValid() const;
  bool        valid{};
  int         year{};
  int         month{};
  int         day{};
  std::string format(const std::string &formatStr) const {
    std::ostringstream oss;
    std::vector<std::string> monthNames = {"January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"};

    for (size_t i = 0; i < formatStr.size(); i++) {
      if (formatStr[i] == '%' && i + 1 < formatStr.size()) {
        switch (formatStr[i + 1]) {
          case 'Y': oss << year; break;
          case 'y': oss << std::setw(2) << std::setfill('0') << (year % 100); break;
          case 'M': oss << monthNames[month - 1]; break;
          case 'm': oss << std::setw(2) << std::setfill('0') << month; break;
          case 'D':
            oss << day;
            if(day == 1 || day == 21 || day == 31) oss << "st";
            else if(day == 2 || day == 22) oss << "nd";
            else if(day == 3 || day == 23) oss << "rd";
            else oss << "th";
            break;
          case 'd': oss << std::setw(2) << std::setfill('0') << day; break;
          default: oss << formatStr[i + 1]; break;
        }
        i++;
      } else {
        oss << formatStr[i];
      }
    }
    return oss.str();
  }
};
