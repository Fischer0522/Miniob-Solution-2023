/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai 2023/6/27
//

#pragma once

#include <span>
#include <string>
#include <complex>
#include "date.h"
#include "common/rc.h"
#include "storage/buffer/page.h"

/**
 * @brief 属性的类型
 *
 */
enum AttrType
{
  UNDEFINED,
  CHARS,   ///< 字符串类型
  INTS,    ///< 整数类型(4字节)
  FLOATS,  ///< 浮点数类型(4字节)
  TEXTS,   ///< TEXT文本类型，最大64KB
  DATES,  /// date类型，在程序中使用Date类型来表达，输出时格式化为yyyy-mm-dd,存储时使用yyyymmdd的int类型(4字节)
  BOOLEANS,   ///< boolean类型，当前不是由parser解析出来的，是程序内部使用的
  TUPLE_SET,  ///< 集合类型，当前不是由parser解析出来的，是程序内部使用的
};

const char *attr_type_to_string(AttrType type);
AttrType    attr_type_from_string(const char *s);

/**
 * @brief 属性的值
 *
 */
class Value
{
public:
  Value() = default;

  Value(AttrType attr_type, char *data, int length = 4) : attr_type_(attr_type) { this->set_data(data, length); }

  explicit Value(int val);
  explicit Value(float val);
  explicit Value(bool val);
  explicit Value(const char *s, int len = 0);

  static Value Null()
  {
    Value val;
    val.is_null_ = true;
    return val;
  }

  bool is_null() const;

  Value(const Value &other)            = default;
  Value &operator=(const Value &other) = default;

  RC cast_to(AttrType in_type);

  void set_type(AttrType type) { this->attr_type_ = type; }
  void set_data(char *data, int length);
  void set_data(const char *data, int length) { this->set_data(const_cast<char *>(data), length); }
  void set_int(int val);
  void set_float(float val);
  void set_boolean(bool val);
  void set_string(const char *s, int len = 0);
  void set_text_string(std::string s);
  void set_text(const char *s,int len = 0);
  void set_text_page_numbers(std::vector<PageNum> text_page_numbers);
  void set_date(Date d);
  void set_value(const Value &value);
  void set_null() { is_null_ = true; }
  void set_null(AttrType attr_type);
  void set_tuple_set(Value *p_tuple_set, std::size_t length);
  std::vector<PageNum>& get_text_page_numbers();
  RC   apply_length_func()
  {
    if (attr_type_ != AttrType::CHARS) {
      return RC::ILLEGAL_FUNCTION;
    }
    set_int((int)get_string().size());
    return RC::SUCCESS;
  }

  RC apply_round_func(int decimals = 0)
  {
    let customRound = [](double value) {
      double floorValue = std::floor(value);
      double diff       = value - floorValue;
      if (diff <= 0.5) {
        return floorValue;
      } else {
        return floorValue + 1.0;
      }
    };
    if (attr_type() == AttrType::FLOATS) {
      double factor = std::pow(10.0, decimals);
      set_float((float)(customRound(get_float() * factor) / factor));
    } else {
      return RC::ILLEGAL_FUNCTION;
    }
    return RC::SUCCESS;
  }

  RC apply_date_format_func(const std::string &format)
  {
    if (attr_type() == AttrType::DATES) {
      let s = get_date().format(format);
      set_string(s.c_str(), (int)s.size());
    } else {
      return RC::ILLEGAL_FUNCTION;
    }
    return RC::SUCCESS;
  }

  std::string to_string() const;

  int compare(const Value &other) const;

  const char *data() const;
  int         length() const { return length_; }

  AttrType attr_type() const { return attr_type_; }

  bool contains(const Value &value) const;

public:
  /**
   * 获取对应的值
   * 如果当前的类型与期望获取的类型不符，就会执行转换操作
   */
  int         get_int() const;
  float       get_float() const;
  std::string get_string() const;
  Date        get_date() const;
  bool        get_boolean() const;

  void clear_denominator()
  {
    if (denominator != 0) {
      switch (attr_type_) {
        case FLOATS: num_value_.float_value_ /= (float)denominator; break;
        case INTS:
          num_value_.float_value_ = (float)num_value_.int_value_;
          attr_type_              = FLOATS;
          num_value_.float_value_ /= (float)denominator;
          break;
        default: break;
      }
      denominator = 0;
    }
  }

  void increase_denominator() { denominator++; }

  Value &operator+=(const Value &rhs)
  {
    switch (attr_type_) {

      case FLOATS: num_value_.float_value_ += rhs.get_float(); break;
      case INTS: num_value_.int_value_ += rhs.get_int(); break;
      default: break;
    }
    return *this;
  }

  void set_name(std::string name) { raw_name = std::move(name); }

  bool is_number_type()
  {
    return attr_type_ == AttrType::FLOATS || attr_type_ == AttrType::INTS || attr_type_ == AttrType::BOOLEANS;
  }

  std::string raw_name;

private:
  AttrType attr_type_ = UNDEFINED;
  int      length_    = 0;

  union
  {
    int   int_value_;
    float float_value_;
    bool  bool_value_;
  } num_value_;
  std::string      str_value_;
  std::span<Value> tuple_set_;
  Date             date_value_;
  std::vector<PageNum> text_page_numbers_;

  int  denominator = 0;  ///< 分母，用于计算avg
  bool is_null_    = false;
};

/** Implements std::hash on Value */
namespace std {

template <>
struct hash<Value>
{
  auto operator()(const Value &val) const -> std::size_t {
    if (val.is_null()) {
      return 1;
    }
    switch (val.attr_type()) {
      case UNDEFINED: return 0;
      case CHARS: return std::hash<std::string>()(val.get_string());
      case INTS: return std::hash<int>()(val.get_int());
      case FLOATS: return std::hash<float>()(val.get_float());
      case DATES: return std::hash<std::string>()(val.get_date().to_string());
      case BOOLEANS: return std::hash<int>()(val.get_boolean());
      case TUPLE_SET: {
        // not supported!
        return -1;
      }
    }

    return -1;
  }
};
}  // namespace std