#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <optional>
#include <sstream>
#include <string>

std::pair<std::string_view, std::optional<std::string_view>> SplitTwoStrict(std::string_view s, std::string_view delimiter = " ");

std::pair<std::string_view, std::string_view> SplitTwo(std::string_view s, std::string_view delimiter = " ");

std::string_view ReadToken(std::string_view& s, std::string_view delimiter = " ");

int ConvertToInt(std::string_view str);

template <typename Number>
void ValidateBounds(Number number_to_check, Number min_value, Number max_value);

#endif // STRINGHELPER_H
