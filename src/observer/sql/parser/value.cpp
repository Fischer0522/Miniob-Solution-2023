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
// Created by WangYunlai on 2023/06/28.
//

#include <sstream>
#include "sql/parser/value.h"
#include "storage/field/field.h"
#include "common/log/log.h"
#include "common/lang/comparator.h"
#include "common/lang/string.h"
#include "sql/expr/tuple.h"

const char *ATTR_TYPE_NAME[] = {"undefined", "chars", "ints", "floats", "texts","dates", "booleans"};

const char *attr_type_to_string(AttrType type)
{
  if (type >= UNDEFINED && type <= DATES) {
    return ATTR_TYPE_NAME[type];
  }
  return "unknown";
}
AttrType attr_type_from_string(const char *s)
{
  for (unsigned int i = 0; i < sizeof(ATTR_TYPE_NAME) / sizeof(ATTR_TYPE_NAME[0]); i++) {
    if (0 == strcmp(ATTR_TYPE_NAME[i], s)) {
      return (AttrType)i;
    }
  }
  return UNDEFINED;
}

Value::Value(int val) { set_int(val); }

Value::Value(float val) { set_float(val); }

Value::Value(bool val) { set_boolean(val); }

Value::Value(const char *s, int len /*= 0*/) { set_string(s, len); }

void Value::set_data(char *data, int length)
{
  switch (attr_type_) {
    case CHARS: {
      set_string(data, length);
    } break;
    case INTS: {
      num_value_.int_value_ = *(int *)data;
      length_               = length;
    } break;
    case FLOATS: {
      num_value_.float_value_ = *(float *)data;
      length_                 = length;
    } break;
    case BOOLEANS: {
      num_value_.bool_value_ = *(int *)data != 0;
      length_                = length;
    } break;
    case DATES: {
      int date_value        = *(int *)data;
      date_value_           = Date(date_value);
      num_value_.int_value_ = date_value;
      length_               = length;
    }
    case TEXTS: {
      set_text(data,length);
    } break;
    default: {
      LOG_WARN("unknown data type: %d", attr_type_);
    } break;
  }
}
void Value::set_int(int val)
{
  attr_type_            = INTS;
  num_value_.int_value_ = val;
  length_               = sizeof(val);
}

void Value::set_float(float val)
{
  attr_type_              = FLOATS;
  num_value_.float_value_ = val;
  length_                 = sizeof(val);
}
void Value::set_boolean(bool val)
{
  attr_type_             = BOOLEANS;
  num_value_.bool_value_ = val;
  length_                = sizeof(val);
}

void Value::set_string(const char *s, int len /*= 0*/)
{
  attr_type_ = CHARS;
  if (len > 0) {
    len = strnlen(s, len);
    str_value_.assign(s, len);
  } else {
    str_value_.assign(s);
  }
  length_ = str_value_.length();
  float f;
  int   i;
  common::str_to_val(s, f);
  common::str_to_val(s, i);
  num_value_.int_value_   = i;
  num_value_.float_value_ = f;
}
void Value::set_text(const char *s, int len) {
  // TODO fix this magic number later
  int text_page_length = 9;
  for (int i = 0;i < text_page_length;i++) {
    int decodeValue;
    memcpy(&decodeValue,s + i * 4,4);
    if (decodeValue == BP_INVALID_PAGE_NUM) {
      return;
    }
    this->text_page_numbers_.push_back(decodeValue);
  }
}
void Value::set_date(Date d)
{
  attr_type_            = DATES;
  length_               = sizeof(int);
  date_value_           = d;
  num_value_.int_value_ = d.to_int();
}
void Value::set_value(const Value &value)
{
  switch (value.attr_type_) {
    case INTS: {
      set_int(value.get_int());
    } break;
    case FLOATS: {
      set_float(value.get_float());
    } break;
    case CHARS: {
      set_string(value.get_string().c_str());
    } break;
    case BOOLEANS: {
      set_boolean(value.get_boolean());
    } break;
    case DATES: {
      set_date(value.get_date());
    }
    case UNDEFINED: {
      ASSERT(false, "got an invalid value type");
    } break;
  }
}

const char *Value::data() const
{
  switch (attr_type_) {
    case CHARS: {
      return str_value_.c_str();
    } break;
    case TEXTS: {
      return str_value_.c_str();
    }
    case DATES: {
      // 在value当中存储了一份int类型的date，返回这个即可
      return (const char *)&num_value_;
    }
    default: {
      return (const char *)&num_value_;
    } break;
  }
}

std::string Value::to_string() const
{
  std::stringstream os;
  if (is_null()) {
    os << "NULL";
    return os.str();
  }
  switch (attr_type_) {
    case INTS: {
      os << num_value_.int_value_;
    } break;
    case FLOATS: {
      os << common::double_to_str(num_value_.float_value_);
    } break;
    case BOOLEANS: {
      os << num_value_.bool_value_;
    } break;
    case CHARS: {
      os << str_value_;
    } break;
    case DATES: {
      os << date_value_.to_string();
    } break;
    case TEXTS: {
      os << str_value_;
    }
    default: {
      LOG_WARN("unsupported attr type: %d", attr_type_);
    } break;
  }
  return os.str();
}

int Value::compare(const Value &other) const
{
  if (is_null()) {
    if (other.is_null()) {
      return 0;
    } else {
      return -1;
    }
  }
  if (this->attr_type_ == other.attr_type_) {
    switch (this->attr_type_) {
      case INTS: {
        return common::compare_int((void *)&this->num_value_.int_value_, (void *)&other.num_value_.int_value_);
      } break;
      case FLOATS: {
        return common::compare_float((void *)&this->num_value_.float_value_, (void *)&other.num_value_.float_value_);
      } break;
      case CHARS: {
        return common::compare_string((void *)this->str_value_.c_str(),
            this->str_value_.length(),
            (void *)other.str_value_.c_str(),
            other.str_value_.length());
      } break;
      case BOOLEANS: {
        return common::compare_int((void *)&this->num_value_.bool_value_, (void *)&other.num_value_.bool_value_);
      }
      case DATES: {
        return this->date_value_.compare(other.date_value_);
      }
      default: {
        LOG_WARN("unsupported type: %d", this->attr_type_);
      }
    }
  } else if (this->attr_type_ == INTS && other.attr_type_ == FLOATS) {
    float this_data = this->num_value_.int_value_;
    return common::compare_float((void *)&this_data, (void *)&other.num_value_.float_value_);
  } else if (this->attr_type_ == FLOATS && other.attr_type_ == INTS) {
    float other_data = other.num_value_.int_value_;
    return common::compare_float((void *)&this->num_value_.float_value_, (void *)&other_data);
  }
  if (this->attr_type_ == CHARS) {
    if (other.attr_type_ == INTS) {
      int int_val;
      common::str_to_val(this->str_value_, int_val);
      return common::compare_int((void *)&int_val, (void *)&other.num_value_.int_value_);
    } else if (other.attr_type_ == FLOATS) {
      float float_val;
      common::str_to_val(this->str_value_, float_val);
      return common::compare_float((void *)&float_val, (void *)&other.num_value_.float_value_);
    }
  } else if (other.attr_type_ == CHARS) {
    if (this->attr_type_ == INTS) {
      int int_val;
      common::str_to_val(other.str_value_, int_val);
      return common::compare_int((void *)&this->num_value_.int_value_, (void *)&int_val);
    } else if (this->attr_type_ == FLOATS) {
      float float_val;
      common::str_to_val(other.str_value_, float_val);
      return common::compare_float((void *)&this->num_value_.float_value_, (void *)&float_val);
    }
  }
  LOG_WARN("not supported");
  return -1;  // TODO return rc?
}

int Value::get_int() const
{
  switch (attr_type_) {
    case CHARS: {
      try {
        return (int)(std::stol(str_value_));
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to number. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return 0;
      }
    }
    case INTS: {
      return num_value_.int_value_;
    }
    case FLOATS: {
      return (int)(num_value_.float_value_);
    }
    case BOOLEANS: {
      return (int)(num_value_.bool_value_);
    }
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

float Value::get_float() const
{
  switch (attr_type_) {
    case CHARS: {
      try {
        return std::stof(str_value_);
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return 0.0;
      }
    } break;
    case INTS: {
      return float(num_value_.int_value_);
    } break;
    case FLOATS: {
      return num_value_.float_value_;
    } break;
    case BOOLEANS: {
      return float(num_value_.bool_value_);
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

std::string Value::get_string() const { return this->to_string(); }
Date        Value::get_date() const { return this->date_value_; }

bool Value::get_boolean() const
{
  switch (attr_type_) {
    case CHARS: {
      try {
        float val = std::stof(str_value_);
        if (val >= EPSILON || val <= -EPSILON) {
          return true;
        }

        int int_val = std::stol(str_value_);
        if (int_val != 0) {
          return true;
        }

        return !str_value_.empty();
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float or integer. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return !str_value_.empty();
      }
    } break;
    case INTS: {
      return num_value_.int_value_ != 0;
    } break;
    case FLOATS: {
      float val = num_value_.float_value_;
      return val >= EPSILON || val <= -EPSILON;
    } break;
    case BOOLEANS: {
      return num_value_.bool_value_;
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return false;
    }
  }
  return false;
}

bool Value::contains(const Value &value) const
{
  if (attr_type_ != TUPLE_SET) {
    return attr_type_ == value.attr_type() && value.compare(*this) == 0;
  }

  return std::ranges::any_of(tuple_set_, [value](const Value &cell) { return cell.compare(value) == 0; });
}

void Value::set_tuple_set(Value *p_tuple_set, std::size_t length)
{
  if (length == 0) {
    set_null();
    return;
  }

  if (length == 1) {
    *this = *p_tuple_set;
    return;
  }

  attr_type_ = TUPLE_SET;
  tuple_set_ = {p_tuple_set, length};
}
bool Value::is_null() const
{
  return is_null_ || num_value_.int_value_ == std::numeric_limits<int>::max() ||
         num_value_.float_value_ == std::numeric_limits<float>::max();
}
void Value::set_null(AttrType attr_type)
{
  this->is_null_   = true;
  this->attr_type_ = attr_type;
  if (attr_type == INTS) {
    set_int(0);
  } else if (attr_type == FLOATS) {
    set_float(0);
  } else if (attr_type == DATES) {
    set_date({19700101});
  } else if (attr_type == CHARS) {
    set_string("");
  }
}

RC Value::cast_to(AttrType in_type)
{
  RC rc = RC::SUCCESS;
  if (attr_type_ == in_type) {
    return rc;
  }
  if (is_null()) {
    set_type(in_type);
    return RC::SUCCESS;
  }
  switch (in_type) {
    case UNDEFINED: {
      return_rc(RC::CAST_FAILURE);
    } break;
    case CHARS: {
      if (raw_name.empty()) {
        let s = to_string();
        set_string(s.c_str(), (int)s.size());
      } else {
        set_string(raw_name.c_str(), (int)raw_name.size());
      }
    } break;
    case INTS: {
      if (is_number_type()) {
        if (attr_type_ == FLOATS) {
          apply_round_func();
        }
        set_int(get_int());
      } else {
        return_rc(RC::CAST_FAILURE);
      }
    } break;
    case FLOATS: {
      if (is_number_type()) {
        set_float(get_float());
      } else {
        return_rc(RC::CAST_FAILURE);
      }
    } break;
    case DATES: {
      return_rc(RC::CAST_FAILURE);
    } break;
    case BOOLEANS: {
      if (is_number_type()) {
        set_boolean(get_boolean());
      } else {
        return_rc(RC::CAST_FAILURE);
      }
    } break;
    case TUPLE_SET: {
      return_rc(RC::CAST_FAILURE);
    } break;
  }

  return rc;
}
void Value::set_text_page_numbers(std::vector<PageNum> text_page_numbers) {
  this->text_page_numbers_ = std::move(text_page_numbers);
}
std::vector<PageNum> &Value::get_text_page_numbers() { return this->text_page_numbers_; }
void                  Value::set_text_string(std::string s) {
  this->str_value_ = std::move(s);
}
