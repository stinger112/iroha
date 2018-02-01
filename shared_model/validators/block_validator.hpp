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

#ifndef IROHA_BLOCK_VALIDATOR_HPP
#define IROHA_BLOCK_VALIDATOR_HPP

#include <boost/format.hpp>
#include <boost/variant/static_visitor.hpp>
#include <regex>
#include "datetime/time.hpp"
#include "interfaces/common_objects/types.hpp"
#include "utils/polymorphic_wrapper.hpp"
#include "validators/answer.hpp"

// TODO 22/01/2018 x3medima17: write stateless validator IR-837

namespace shared_model {
  namespace validation {

    /**
     * Class that validates block
     */
    class BlockValidator {
     private:
      /**
       * Visitor used by commands validator to validate fields from block
       */
      class BlockValidatorVisitor
          : public boost::static_visitor<ReasonsGroupType> {
       public:
        ReasonsGroupType operator()(
            const detail::PolymorphicWrapper<interface::Block> &block) const {
          ReasonsGroupType reason;

          return reason;
        }

       private:
      };

     public:
      /**
       * Applies validation on block
       * @param block
       * @return Answer containing found error if any
       */
      Answer validate(
          detail::PolymorphicWrapper<interface::Block> block) const {
        Answer answer;

        return answer;
      }

      Answer answer_;
    };

  }  // namespace validation
}  // namespace shared_model

#endif  // IROHA_BLOCK_VALIDATOR_HPP
