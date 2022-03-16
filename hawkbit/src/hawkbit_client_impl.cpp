#include <chrono>
#include <thread>
#include <string>

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "hawkbit_client_impl.hpp"
#include "response_impl.hpp"
#include "actions_impl.hpp"
#include "utils.hpp"


namespace hawkbit {
    void checkHttpCode(int presented, int expected) {
        if (presented == HTTP_UNAUTHORIZED)
            throw unauthorized_exception();
        if (presented != expected)
            throw http_unexpected_code_exception(presented, expected);
    }


    [[noreturn]] void HawkbitCommunicationClient::run() {
        while (true) {
            ignoreSleep = false;
            doPoll();
            if (!ignoreSleep && currentSleepTime > 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(currentSleepTime));
        }
    }

    httplib::Client HawkbitCommunicationClient::newHttpClient(uri::URI &hostEndpoint) {

        return httplib::Client(hostEndpoint.getScheme() + "://" + hostEndpoint.getAuthority());
    }

    // set actionId here (hawkbit api requires it but in docs not)
    void fillResponseDocument(Response *response, rapidjson::Document &document, int actionId) {
        if (response == nullptr) {
            throw wrong_response();
        }

        rapidjson::Value status(rapidjson::kObjectType);
        rapidjson::Value result(rapidjson::kObjectType);

        result.AddMember("finished", Response::finishedToString(response->getFinished()), document.GetAllocator());
        status.AddMember("result", result, document.GetAllocator());
        status.AddMember("execution", Response::executionToString(response->getExecution()), document.GetAllocator());

        rapidjson::Value details(rapidjson::kArrayType);
        for (const auto &val: response->getDetails()) {
            details.PushBack(rapidjson::Value{}.SetString(val.c_str(), val.length(), document.GetAllocator()),
                             document.GetAllocator());
        }
        status.AddMember("details", details, document.GetAllocator());

        document.AddMember("status", status, document.GetAllocator());

        if (actionId >= 0) {
            document.AddMember("id", std::to_string(actionId), document.GetAllocator());
        }
    }

    void HawkbitCommunicationClient::followConfigData(uri::URI &followURI) {
        auto req = handler->onConfigRequest();
        auto requestData = req->getData();
        if (requestData.empty()) {
            return;
        }

        rapidjson::Document document;
        document.SetObject();

        // fill data object
        rapidjson::Value data(rapidjson::kObjectType);
        for (auto &val: requestData) {
            rapidjson::Value key(val.first, document.GetAllocator());
            rapidjson::Value value(val.second, document.GetAllocator());
            data.AddMember(key, value, document.GetAllocator());
        }

        document.AddMember("data", data, document.GetAllocator());
        auto builder = ResponseBuilder::newInstance();
        auto resp = builder->setFinished(Response::SUCCESS)->setExecution(Response::CLOSED)->build();

        fillResponseDocument(resp.get(), document, -1);

        rapidjson::StringBuffer buf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
        document.Accept(writer);

        retryHandler(followURI, [&](httplib::Client &cli) {
            return cli.Put(followURI.getPath().c_str(),defaultHeaders, buf.GetString(), "application/json");
        });

        ignoreSleep = req->isIgnoredSleep();
    }


    // see documentation: https://www.eclipse.org/hawkbit/rest-api/rootcontroller-api-guide/#_post_tenant_controller_v1_controllerid_cancelaction_actionid_feedback
    std::string formatFeedbackPath(uri::URI uri) {
        auto path = uri.getPath();
        if (path[path.length() - 1] != '/') {
            path += "/";
        }
        path += "feedback";
        return path;
    }

    void HawkbitCommunicationClient::followCancelAction(uri::URI &followURI) {
        auto resp = retryHandler(followURI, [&](httplib::Client &cli) {
            return cli.Get(followURI.getPath().c_str(), defaultHeaders);
        });

        auto cancelAction = CancelAction_::fromString(resp->body);
        auto actionId = cancelAction->getId();
        auto cliResp = handler->onCancelAction(std::move(cancelAction));

        rapidjson::Document document;
        document.SetObject();

        fillResponseDocument(cliResp.get(), document, actionId);

        rapidjson::StringBuffer buf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
        document.Accept(writer);
        try {
            retryHandler(followURI, [&](httplib::Client &cli) {
                return cli.Post(formatFeedbackPath(
                        followURI).c_str(), defaultHeaders, buf.GetString(), "application/json");
            });

            if (cliResp->getDeliveryListener()) {
                cliResp->getDeliveryListener()->onSuccessfulDelivery();
            }
            // catch only error http code, if no handler defined pass through
        } catch (http_unexpected_code_exception &e) {
            if (cliResp->getDeliveryListener()) {
                cliResp->getDeliveryListener()->onError();
            } else {
                throw e;
            }
        }

        ignoreSleep = cliResp->isIgnoredSleep();
    }

    void HawkbitCommunicationClient::followDeploymentBase(uri::URI &followURI) {
        auto resp =  retryHandler(followURI, [&](httplib::Client &cli) {
            return cli.Get(followURI.getPath().c_str(), defaultHeaders);
        });

        auto deploymentBase = DeploymentBase_::from(resp->body, this);
        auto actionId = deploymentBase->getId();
        auto cliResp = handler->onDeploymentAction(std::move(deploymentBase));

        rapidjson::Document document;
        document.SetObject();
        fillResponseDocument(cliResp.get(), document, actionId);

        rapidjson::StringBuffer buf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
        document.Accept(writer);
        try {
            retryHandler(followURI, [&](httplib::Client &cli) {
                return cli.Post(
                        formatFeedbackPath(followURI).c_str(), defaultHeaders, buf.GetString(), "application/json");
            });

            if (cliResp->getDeliveryListener()) {
                cliResp->getDeliveryListener()->onSuccessfulDelivery();
            }
            // catch only error http code, if no handler defined pass through
        } catch (http_unexpected_code_exception &e) {
            if (cliResp->getDeliveryListener()) {
                cliResp->getDeliveryListener()->onError();
            } else {
                throw e;
            }
        }

        ignoreSleep = cliResp->isIgnoredSleep();
    }

    void HawkbitCommunicationClient::doPoll() {
        // firstly do GET request to default endpoint. hawkBit send meta for next poll and
        //  action list to follow
        auto resp = retryHandler(hawkbitURI, [&](httplib::Client &cli) {
            return cli.Get(hawkbitURI.getPath().c_str(), defaultHeaders);
        });
        auto polingData = PollingData_::fromString(resp->body);
        // handle if sleepTime not defined by hawkBit
        currentSleepTime = (polingData->getSleepTime() > 0) ? polingData->getSleepTime() : defaultSleepTime;
        auto followURI = polingData->getFollowURI();
        switch (polingData->getAction()) {
            case Actions_::NONE:
                return handler->onNoActions();
            case Actions_::GET_CONFIG_DATA:
                return followConfigData(followURI);
            case CANCEL_ACTION:
                return followCancelAction(followURI);
            case DEPLOYMENT_BASE:
                return followDeploymentBase(followURI);
        }
    }

    void HawkbitCommunicationClient::downloadTo(uri::URI downloadURI, const std::string& path) {
        std::ofstream file(path);
         retryHandler(downloadURI, [&](httplib::Client &cli) {
             return cli.Get(downloadURI.getPath().c_str(), defaultHeaders,
                     [] (const httplib::Response &r) {
                         checkHttpCode(r.status, HTTP_OK);
                         return true;
                     },
                     [&] (const char * data, size_t size) {
                         file.write(data, size);
                         return !file.bad();
                     }
             );
         });

    }

    std::string HawkbitCommunicationClient::getBody(uri::URI downloadURI) {
        return retryHandler(downloadURI, [&](httplib::Client &cli){
            return cli.Get(downloadURI.getPath().c_str(), defaultHeaders);
        })->body;
    }

    void HawkbitCommunicationClient::downloadWithReceiver(uri::URI downloadURI, std::function<bool(const char *, size_t)> func) {
        retryHandler(downloadURI, [&](httplib::Client &cli) {
           return cli.Get(downloadURI.getPath().c_str(), defaultHeaders,
                           [] (const httplib::Response &r) {
                               checkHttpCode(r.status, HTTP_OK);
                               return true;
                           }, func
                   );
        });

    }

    httplib::Result HawkbitCommunicationClient::retryHandler(uri::URI reqUri, const std::function<httplib::Result(httplib::Client &)> &func) {
        auto cli = newHttpClient(reqUri);
        auto resp = func(cli);
        if (resp.error() != httplib::Error::Success) {
            throw http_lib_error((int)resp.error());
        }
        checkHttpCode(resp->status, HTTP_OK);
        return resp;
    }

}