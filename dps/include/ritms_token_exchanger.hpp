#pragma once
#include <string>
#include <memory>

namespace ritms {
    namespace dps {
        class TokenExchanger {
        public:

            virtual std::string exchange() = 0;

            virtual ~TokenExchanger() = default;
        };

        class TokenExchangerFactory {
        public:

            static std::unique_ptr<TokenExchanger>
                    buildCloudExchanger(const std::string& exchangerUrl,
                                        const std::string& token);
        };
    }
}