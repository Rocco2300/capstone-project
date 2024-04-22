#pragma once

#include <fmt/core.h>

#ifndef NDEBUG
#define massert(expr, fmt, ...) massert_impl(#expr, expr, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define massert(expr, fmt, ...) ;
#endif

template <typename... Args>
void massert_impl(std::string_view expressionStr,
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
    fmt::print(stderr, "Assert failed at {}: {}\nExpected: {}\n{}", file, line, expressionStr,
               message);
    abort();
}