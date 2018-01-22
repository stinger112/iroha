/**
 * Copyright Soramitsu Co., Ltd. 2018 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IROHA_RESULT_HPP
#define IROHA_RESULT_HPP

#include "visitor.hpp"

/**
 * Result is a type which represents value or an error, and values and errors
 * are template parametrized. Working with value wrapped in result is done using
 * match() function, which accepts 2 functions: for value and error cases. No
 * accessor functions are provided.
 */

namespace iroha {
  namespace expected {

    /**
     * Value and error types can be constructed from any value or error, if
     * underlying types are constructible. Example:
     *
     * @code
     * Value<std::string> v = Value<const char *>("hello");
     * @nocode
     */
    template <typename T>
    struct Value {
      T value;
      template <typename V>
      operator Value<V>() {
        return {value};
      }
    };

    template <typename E>
    struct Error {
      E error;
      template <typename V>
      operator Error<V>() {
        return {error};
      }
    };

    /**
     * Result is a specialization of a variant type with value or error
     * semantics.
     * @tparam V type of value
     * @tparam E error type
     */
    template <typename V, typename E>
    class Result : public boost::variant<Value<V>, Error<E>> {
      using variant_type = boost::variant<Value<V>, Error<E>>;
      using variant_type::variant_type;  // inherit constructors

     public:
      /**
       * match is a function which allows working with result's underlying
       * types, you must provide 2 functions to cover success and failure cases.
       * Return type of both functions must be the same. Example usage:
       * @code
       * result.match([](Value<int> v) { std::cout << v.value; },
       *              [](Error<std::string> e) { std::cout << e.error; });
       * @nocode
       */
      template <typename ValueMatch, typename ErrorMatch>
      constexpr auto match(ValueMatch &&value_func, ErrorMatch &&error_func) {
        return visit_in_place(*this,
                              std::forward<ValueMatch>(value_func),
                              std::forward<ErrorMatch>(error_func));
      }
    };

    // Factory methods for avoiding type specification
    template <typename T>
    Value<T> makeValue(T &&value) {
      return Value<T>{std::forward<T>(value)};
    }

    template <typename E>
    Error<E> makeError(E &&error) {
      return Error<E>{std::forward<E>(error)};
    }

    /**
     * Bind operator allows chaining several functions which return result. If
     * result contains error, it returns this error, if it contains value,
     * function f is called.
     * @param f function which return type must be compatible with original
     * result
     */
    template <typename T, typename E, typename Transform>
    constexpr auto operator|(Result<T, E> r, Transform &&f)
        -> decltype(f(std::declval<T>())) {
      using return_type = decltype(f(std::declval<T>()));
      return r.match(
          [&f](const Value<T> &v) { return f(v.value); },
          [](const Error<E> &e) { return return_type(makeError(e.error)); });
    }
  }  // namespace expected
}  // namespace iroha
#endif  // IROHA_RESULT_HPP