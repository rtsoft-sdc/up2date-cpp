#include <exception>
#include <string>

namespace ritms {
    namespace dps {
        const int HTTP_UNAUTHORIZED = 401;
        const int HTTP_OK = 200;
        const int HTTP_CREATED = 201;

        // Error in httplib
        class httplib_error: public std::exception {
            std::string message;
        public:
            httplib_error(int error_num) {
                message = "HTTPLib request error. Error code " + std::to_string(error_num);
            }
            const char *what() const noexcept override {
                return message.c_str();
            }
        };

        // Error while provisioning (unexpected response code)
        class provisioning_error: public std::exception {
            std::string message;
        public:
            provisioning_error(int error_num) {
                message = "Provisioning request error. HTTP code " + std::to_string(error_num);
            }
            const char *what() const noexcept override {
                return message.c_str();
            }
        };

        // Error in cloud
        class up2date_cloud_error: public std::exception {
            std::string message;
        public:
            up2date_cloud_error(const std::string &msg) {
                message = "Error from cloud: " + msg;
            }

            const char *what() const noexcept override {
                return message.c_str();
            }
        };
    }
}