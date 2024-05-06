//
// Created by code4love on 23-10-10.
//

#ifndef MINIDB_PCH_H
#define MINIDB_PCH_H

#include <vector>
#include <algorithm>
#include <deque>
#include <queue>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <string>

//extern std::vector<std::string> call_stack;

//#define new_call_stack call_stack.resize(0)
#define new_call_stack
#define push_call_stack call_stack.push_back(std::string("") + __func__ + ":" + std::to_string(__LINE__))
//#define return_rc(rc) { let r = (rc); if (r != RC::SUCCESS) { push_call_stack; } return r; }
#define return_rc(rc) return rc;

//#define SAME_TYPE(T1, T2) constexpr (std::is_same_v<T1, T2>)

#define container_of(ptr, type, member) \
  ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

template <typename T>
using ref = const std::remove_cvref_t<T> &;

template <typename T>
using ref_mut = std::remove_cvref_t<T> &;

#define let const auto
#define let_ref const auto &
#define let_mut auto
#define let_mut_ref auto &

#define DEFER(NAME, DO) auto defer_##NAME = Z_defer_constructor([&](auto*) { DO });

template <typename T>
auto Z_defer_constructor(T f)
{
//  struct Defer
//  {
//    T &&f;
//    ~Defer() { f(); }
//  };
//  return Defer{std::forward<decltype(f)>(f)};
  return std::unique_ptr<T, T>(&f, f);
}

//static_assert(std::is_same_v<ref<int>, const int &>);
//static_assert(std::is_same_v<ref<int &>, const int &>);
//static_assert(std::is_same_v<ref<int &&>, const int &>);
//static_assert(std::is_same_v<ref<const int>, const int &>);
//static_assert(std::is_same_v<ref<const int &>, const int &>);
//static_assert(std::is_same_v<ref<const int &&>, const int &>);
//
//static_assert(std::is_same_v<ref_mut<int>, int &>);
//static_assert(std::is_same_v<ref_mut<int &>, int &>);
//static_assert(std::is_same_v<ref_mut<int &&>, int &>);
//static_assert(std::is_same_v<ref_mut<const int>, int &>);
//static_assert(std::is_same_v<ref_mut<const int &>, int &>);
//static_assert(std::is_same_v<ref_mut<const int &&>, int &>);

#endif  // MINIDB_PCH_H
