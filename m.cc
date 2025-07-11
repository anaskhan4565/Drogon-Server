#include <drogon/drogon.h>
#include <atomic>
#include <iostream>
#include <vector>
#include <sstream>  // Needed for istringstream
#include "concurrentqueue.h"
#include "sbe/my_app_messages/LoginRequest.h"
#include "sbe/my_app_messages/MessageHeader.h"

using namespace std;
using namespace drogon;

int main() {
    app().registerHandler("/hello",
        [](const HttpRequestPtr &req,
           function<void(const HttpResponsePtr &)> &&callback) {
            
            Json::Value responseJson;
            auto json = req->getJsonObject();

            if (!json||!(*json)["name"].isString()||!(*json)["id"].isString()||!(*json)["dateOfIssue"].isString()||!(*json)["verified"].isString())
        {
           Json::Value error;
           error["error"]="Invalid Json";
           auto resp=HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
           callback(resp);
           return;
        }

            // Log incoming JSON
            // std::cout << "Incoming body: " << json->toStyledString() << std::endl;

            auto client = HttpClient::newHttpClient("http://localhost:5556");

            auto apiRequest = HttpRequest::newHttpRequest();
            apiRequest->setMethod(Post);
            apiRequest->setPath("/hello");
            apiRequest->setContentTypeCode(CT_APPLICATION_JSON);

            // Forward the incoming JSON as the body
            apiRequest->setBody(json->toStyledString());

            // std::cout << "Forwarding to Flask: " << json->toStyledString() << std::endl;

            client->sendRequest(apiRequest, [callback](ReqResult result, const HttpResponsePtr &resp) {
                if (result == ReqResult::Ok) {
                    Json::CharReaderBuilder readerBuilder;
                    Json::Value parsedJson;
                    std::string errs;

                    std::istringstream stream(std::string(resp->body()));  // Convert string_view to string

                    if (Json::parseFromStream(readerBuilder, stream, &parsedJson, &errs)) {
                        auto response = HttpResponse::newHttpJsonResponse(parsedJson);
                        // cout<<response->getBody()<<endl;
                        callback(response);
                    } else {
                        Json::Value error;
                        error["error"] = "Failed to parse JSON from Flask response";
                        auto errResp = HttpResponse::newHttpJsonResponse(error);
                        errResp->setStatusCode(k500InternalServerError);
                        callback(errResp);
                    }
                } else {
                    Json::Value error;
                    error["error"] = "Failed to call external API";
                    auto errResp = HttpResponse::newHttpJsonResponse(error);
                    errResp->setStatusCode(k500InternalServerError);
                    callback(errResp);
                }
            });

        },
        {Post}
    );

    app().addListener("0.0.0.0", 5555);
    app().run();
}
