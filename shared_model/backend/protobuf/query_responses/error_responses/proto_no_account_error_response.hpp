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

#ifndef IROHA_SHARED_MODEL_PROTO_NO_ACCOUNT_ERROR_RESPONSE_HPP
#define IROHA_SHARED_MODEL_PROTO_NO_ACCOUNT_ERROR_RESPONSE_HPP

#include "interfaces/query_responses/error_responses/no_account_error_response.hpp"
#include "responses.pb.h"
#include "utils/lazy_initializer.hpp"
#include "utils/reference_holder.hpp"

namespace shared_model {
  namespace proto {
    class NoAccountErrorResponse final
        : public CopyableProto<interface::NoAccountErrorResponse,
                               iroha::protocol::ErrorResponse,
                               NoAccountErrorResponse> {
     public:
      template <typename ErrorResponseType>
      explicit NoAccountErrorResponse(ErrorResponseType &&response)
          : CopyableProto(std::forward<ErrorResponseType>(response)) {}

      NoAccountErrorResponse(const NoAccountErrorResponse &o)
          : NoAccountErrorResponse(o.proto_) {}

      NoAccountErrorResponse(NoAccountErrorResponse &&o) noexcept
          : NoAccountErrorResponse(std::move(o.proto_)) {}
    };
  }  // namespace proto
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_NO_ACCOUNT_ERROR_RESPONSE_HPP
