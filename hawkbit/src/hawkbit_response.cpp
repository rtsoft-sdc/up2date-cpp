#include "response_impl.hpp"

namespace hawkbit {
    std::shared_ptr<ConfigResponseBuilder> ConfigResponseBuilder::newInstance() {
        return std::shared_ptr<ConfigResponseBuilder>(new ConfigResponseBuilderImpl());
    }

    std::shared_ptr<ResponseBuilder> ResponseBuilder::newInstance() {
        auto r = new ResponseBuilderImpl();
        r->setFinished(Response::Finished::NONE);

        return std::shared_ptr<ResponseBuilder>(r);
    }

    const char *FINISHED_SUCCESS = "success";
    const char *FINISHED_FAILURE = "failure";
    const char *FINISHED_NONE = "none";

    std::string Response::finishedToString(const Response::Finished &fin) {
        switch (fin) {
            case SUCCESS:
                return FINISHED_SUCCESS;
            case FAILURE:
                return FINISHED_FAILURE;
            case NONE:
                return FINISHED_NONE;
        }
        // should not happen
        throw std::exception();
    }

    const char *EXECUTION_CLOSED = "closed";
    const char *EXECUTION_PROCEEDING = "proceeding";
    const char *EXECUTION_CANCELED = "canceled";
    const char *EXECUTION_SCHEDULED = "scheduled";
    const char *EXECUTION_REJECTED = "rejected";
    const char *EXECUTION_RESUMED = "resumed";

    std::string Response::executionToString(const Execution &exec) {
        switch (exec) {
            case CLOSED:
                return EXECUTION_CLOSED;
            case PROCEEDING:
                return EXECUTION_PROCEEDING;
            case CANCELED:
                return EXECUTION_CANCELED;
            case SCHEDULED:
                return EXECUTION_SCHEDULED;
            case REJECTED:
                return EXECUTION_REJECTED;
            case RESUMED:
                return EXECUTION_RESUMED;
        }
        // should not happen
        throw std::exception();
    }

}