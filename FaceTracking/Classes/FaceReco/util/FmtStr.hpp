#ifndef FMTSTR_H_
#define FMTSTR_H_

#include <string>
#include <sstream>

namespace facereco {

/**
 * @brief  A python style string format function. i.e.
 *
 *           FormatString("{} world, PI is {}", "hello", 3.1415)
 *
 *         will give you
 *
 *           "hello world, PI is 3.1415"
 *
 *         If there are more values than placehoder(i.e. "{}"), remain
 *         values will be throwed away.
 *
 *           FormatString("{} world, PI is", "hello", 3.1415)
 *
 *         will give you
 *
 *           "hello world, PI is"
 *
 *         If there are more placehoder than values, placehoder will be
 *         preserved.
 *
 *           FormatString("{} world, PI is {}", "hello")
 *
 *         while give you
 *
 *           "hello world, PI is {}"
 *
 *         This function use c++11 new feature -- variadic template,
 *         so you will need c++11 compiler to use it
 *
 * @param fmt_spec format specification that use "{}" as placeholder
 * @param args values corresponding to "{}" in previous specification
 *
 * @returns new string that replace all "{}" with its corresponding value
 */
template <typename... Types>
const std::string FormatString(const std::string& fmt_spec,
                               const Types&... args);

void BuildFormatString(std::ostringstream& builder, const std::string& fmt_spec,
                       std::string::size_type idx) {
    auto count = fmt_spec.size() - idx;
    if (count <= 0) {
        return;
    }

    builder.write(fmt_spec.data() + idx, count);
}

template <typename T, typename... Types>
void BuildFormatString(std::ostringstream& builder, const std::string& fmt_spec,
                       std::string::size_type idx, const T& first,
                       const Types&... args) {
    auto pos = fmt_spec.find_first_of("{}", idx);
    if (pos == std::string::npos) {
        return BuildFormatString(builder, fmt_spec, idx);
    }

    builder.write(fmt_spec.data() + idx, pos - idx);
    builder << first;
    BuildFormatString(builder, fmt_spec, pos + 2, args...);
}

template <typename... Types>
const std::string FormatString(const std::string& fmt_spec,
                               const Types&... args) {
    std::ostringstream builder;
    BuildFormatString(builder, fmt_spec, 0, args...);
    return builder.str();
}

}  // FMTSTR_H

#endif /* end of include guard: FMTSTR_H_ */