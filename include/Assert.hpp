#pragma once

#include <fmt/core.h>

#include <iostream>

#ifndef NDEBUG
#define massert(expr, fmt, ...) massert_impl(#expr, expr, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define massert(expr, msg) ;
#endif

template <typename... Args>
void massert_impl(std::string_view expression_str,
                  bool expression,
                  std::string_view file,
                  int line,
                  fmt::string_view fmt,
                  Args&&... args) {
    if (expression) {
        return;
    }

    std::string message = fmt.data();
    if (sizeof...(Args) > 0) {// If we have args
        message = fmt::vformat(fmt, fmt::make_format_args(args...));
    }
    std::cerr << "Assert failed at " << file << ':' << line << '\n'
              << "Expected: " << expression_str << '\n'
              << message << '\n';
    abort();
}