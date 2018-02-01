/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
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
#ifndef IROHA_AMOUNT_H
#define IROHA_AMOUNT_H

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <nonstd/optional.hpp>
#include <string>

namespace iroha {

  using namespace boost::multiprecision;

  /**
   * Keeps integer and scale values allowing performing math
   * operations on them
   */
  class Amount {
   public:
    /**
     * Creates Amount with integer = 0 and scale = 0
     */
    Amount();

    /**
     * Amount with integer = amount and scale = 0
     * @param amount integer part
     */
    Amount(uint256_t amount);

    /**
     * Amount with provided integer and scale part
     * @param amount integer part
     * @param precision scale part
     */
    Amount(uint256_t amount, uint8_t precision);

    Amount(uint64_t first, uint64_t second, uint64_t third, uint64_t fourth);

    Amount(uint64_t first,
           uint64_t second,
           uint64_t third,
           uint64_t fourth,
           uint8_t precision);

    std::vector<uint64_t> to_uint64s();

    /**
     * Copy constructor
     */
    Amount(const Amount &);
    Amount &operator=(const Amount &);

    /**
     * Move constructor
     */
    Amount(Amount &&);
    Amount &operator=(Amount &&);

    uint256_t getIntValue();
    uint8_t getPrecision();

    static nonstd::optional<Amount> createFromString(std::string str_amount);

    /**
     * Takes percentage from current amount
     * @param percents
     * @return
     */
    Amount percentage(uint256_t percents) const;

    /**
     * Takes percentage represented as amount value
     * The current scale and scale of percents may differ
     * @param percents
     * @return
     */
    Amount percentage(const Amount &percents) const;

    /**
     * Sums up two optionals of the amounts.
     * Requires to have the same scale.
     * Otherwise nullopt is returned
     * @param a left term
     * @param b right term
     * @param optional result
     */
    friend nonstd::optional<Amount> operator+(nonstd::optional<Amount> a,
                                              nonstd::optional<Amount> b) {
      // check precisions
      if (a->precision_ != b->precision_) {
        return nonstd::nullopt;
      }
      auto res = a->add(*b);
      // check overflow
      if (res.value_ < a->value_ or res.value_ < b->value_) {
        return nonstd::nullopt;
      }
      return res;
    }

    /**
     * Subtracts right term from the left term
     * Requires to have the same scale.
     * Otherwise nullopt is returned
     * @param a left term
     * @param b right term
     * @param optional result
     */
    friend nonstd::optional<Amount> operator-(nonstd::optional<Amount> a,
                                              nonstd::optional<Amount> b) {
      // check precisions
      if (a->precision_ != b->precision_) {
        return nonstd::nullopt;
      }
      // check if a greater than b
      if (a->value_ < b->value_) {
        return nonstd::nullopt;
      }
      return a->subtract(*b);
    }

    /**
     * Comparisons are possible between amounts with different precisions.
     *
     * @return
     */
    bool operator==(const Amount &) const;
    bool operator!=(const Amount &) const;
    bool operator<(const Amount &) const;
    bool operator>(const Amount &) const;
    bool operator<=(const Amount &) const;
    bool operator>=(const Amount &) const;

    std::string to_string() const;
    ~Amount() = default;

   private:
    /**
     * Support function for comparison operators.
     * Returns 0 when equal, -1 when current Amount smaller, and 1 when it is
     * greater
     * @param other
     * @return
     */
    int compareTo(const Amount &other) const;

    /**
     * Sums two amounts.
     * @return
     */
    Amount add(const Amount &) const;
    /**
     * Subtracts one amount from another.
     * Requires to have the same scale between both amounts.
     * Otherwise nullopt is returned
     * @return
     */
    Amount subtract(const Amount &) const;

    boost::multiprecision::uint256_t value_{0};
    uint8_t precision_{0};
  };
}  // namespace iroha
#endif  // IROHA_AMOUNT_H
