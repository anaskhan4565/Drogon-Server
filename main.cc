#include <drogon/drogon.h>
#include <atomic>
#include <iostream>
#include <vector>
#include <cstring>
#include "concurrentqueue.h"
#include "sbe/my_app_messages/LoginRequest.h"
#include "sbe/my_app_messages/MessageHeader.h"

using namespace std;
using namespace drogon;

// Global SBE queue
moodycamel::ConcurrentQueue<std::vector<uint8_t>> sbeQueue;
std::atomic<int> requestCount{0};

// Data wrapper
class Data {
public:
    string email;
    string password;

    void setEmail(const string& e) { email = e; }
    void setPassword(const string& p) { password = p; }
    string getEmail() const { return email; }
    string getPassword() const { return password; }
};

// SBE handler
class SBE {
public:
void Producer(const Data &data) {
    std::vector<uint8_t> buffer(256, 0);  // zeroed

    my::app::messages::LoginRequest encoder;
    encoder.wrapForEncode(reinterpret_cast<char *>(buffer.data()), 0, buffer.size());

    auto email = data.getEmail();
    auto password = data.getPassword();

    for (size_t i = 0; i < std::min(email.size(), size_t(64)); ++i)
        encoder.email().chars(i, email[i]);

    for (size_t i = 0; i < std::min(password.size(), size_t(64)); ++i)
        encoder.password().chars(i, password[i]);

    sbeQueue.enqueue(std::move(buffer));
}


std::optional<Data> Consumer() {
    std::vector<uint8_t> buffer;
    if (!sbeQueue.try_dequeue(buffer)) return std::nullopt;

    my::app::messages::LoginRequest decoder;
    decoder.wrapForDecode(reinterpret_cast<char *>(buffer.data()), 0,
                          decoder.sbeBlockLength(), decoder.sbeSchemaVersion(), buffer.size());

    char emailBuf[65] = {0};
    char passwordBuf[65] = {0};

    for (size_t i = 0; i < 64; ++i)
        emailBuf[i] = decoder.email().chars(i);

    for (size_t i = 0; i < 64; ++i)
        passwordBuf[i] = decoder.password().chars(i);

    Data result;
    result.setEmail(emailBuf);
    result.setPassword(passwordBuf);
    return result;
}



};

// Separate method for all route handlers
void setupDrogonRoutes() {
    app().registerHandler("/hello", [](const HttpRequestPtr &req,
                                       std::function<void(const HttpResponsePtr &)> &&callback) {
        int count = ++requestCount;
        auto json = req->getJsonObject();

        if (!json || !(*json)["email"].isString() || !(*json)["password"].isString()) {
            Json::Value error;
            error["error"] = "Invalid JSON";
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        // Parse input
        Data input;
        input.setEmail((*json)["email"].asString());
        input.setPassword((*json)["password"].asString());
        cout<<"Request"<<input.getEmail()<<endl;
        cout<<"Request"<<input.getPassword()<<endl;

        // Encode and immediately decode
        SBE sbe;
        sbe.Producer(input);
        auto result = sbe.Consumer();

        Json::Value response;
        if (result.has_value()) {
            response["email"] = result->getEmail();
            response["password"] = result->getPassword();
            response["request_number"] = count;
            response["message"] = "Login received (via SBE)";
            cout<<"Response"<<response<<endl;
        } else {
            response["error"] = "Queue empty or decode failed";
        }

        callback(HttpResponse::newHttpJsonResponse(response));
    }, {Post});
}

// Main function
int main() {
    setupDrogonRoutes();
    app().addListener("0.0.0.0", 5555);
    app().setThreadNum(12);
    app().run();
}
