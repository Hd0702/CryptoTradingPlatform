#pragma once
#include <chrono>
#include <type_traits>

template <typename _Tp>
struct is_chrono_duration : std::false_type {};

template <class _Rep, class _Period>
struct is_chrono_duration<std::chrono::duration<_Rep, _Period>> : std::true_type {};

template<typename _Tp>
concept chrono_duration = is_chrono_duration<_Tp>::value;