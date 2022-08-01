#pragma once
#include "ritms_token_exchanger.hpp"
#include "uriparse.hpp"

namespace ritms {
    namespace dps {
        class TokenExchanger_impl : public TokenExchanger {

            uri::URI url;
            std::string ott;

        public:

            explicit TokenExchanger_impl(uri::URI url_, std::string token_)
                : url(std::move(url_)), ott(std::move(token_)) {};

            std::string exchange() override;
        };
    }
}