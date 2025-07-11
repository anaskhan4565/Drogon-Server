#include <drogon/drogon.h>
#include <atomic>
#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include "concurrentqueue.h"
#include "sbe/my_app_messages/LoginRequest.h"
#include "sbe/my_app_messages/MessageHeader.h"

using namespace std;
std::atomic<int> queueSize{0};

// Struct for passing raw request data and callback
struct RawTask {
    std::string email;
    std::string password;
    std::function<void(const drogon::HttpResponsePtr &)> callback;
};

// Thread-safe queue for storing tasks
moodycamel::ConcurrentQueue<RawTask> taskQueue;

// Starts a pool of worker threads that process SBE encode/decode and send response
void startWorkerThreads(int threadCount) {
    for (int i = 0; i < threadCount; ++i) {
        std::thread([]() {
            while (true) {
                RawTask task;
                if (taskQueue.try_dequeue(task)) {
                    // Step 1: Encode to SBE
                     queueSize--;
                    std::vector<uint8_t> buffer(256);
                    my::app::messages::LoginRequest encoder;
                    encoder.wrapForEncode(reinterpret_cast<char *>(buffer.data()), 0, buffer.size());

                    std::memcpy(encoder.email().buffer(), task.email.c_str(), std::min((size_t)64, task.email.length()));
                    std::memcpy(encoder.password().buffer(), task.password.c_str(), std::min((size_t)64, task.password.length()));

                    // Step 2: Decode from SBE
                    my::app::messages::LoginRequest decoder;
                    decoder.wrapForDecode(reinterpret_cast<char *>(buffer.data()), 0,
                                          decoder.sbeBlockLength(), decoder.sbeSchemaVersion(), buffer.size());

                    char emailBuf[65] = {0};
                    char passwordBuf[65] = {0};
                    std::memcpy(emailBuf, decoder.email().buffer(), 64);
                    std::memcpy(passwordBuf, decoder.password().buffer(), 64);

                    // Step 3: Create response JSON
                    Json::Value response;
                    response["email"] = std::string(emailBuf);
                    response["password"] = std::string(passwordBuf);
                    response["message"] = "Handled by worker thread";

                    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
                    task.callback(resp);
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            }
        }).detach();
    }
}

int main() {
    // Start 4 background worker threads
    startWorkerThreads(8);

    // Register the /hello endpoint handler
    drogon::app().registerHandler(
        "/hello",
        [](const drogon::HttpRequestPtr &req,
           std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

            auto json = req->getJsonObject();
            if (!json || !(*json)["email"].isString() || !(*json)["password"].isString()) {
                Json::Value error;
                error["error"] = "Invalid JSON";
                auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
                callback(resp);
                return;
            }

            RawTask task;
            task.email = (*json)["email"].asString();
            task.password = (*json)["password"].asString();
            task.callback = callback;

            taskQueue.enqueue(std::move(task));
            queueSize++;
            std::cout << "Current queue size: " << queueSize << std::endl;
            cout.flush();

        },
        {drogon::Post}
    );
drogon::app().registerHandler(
    "/queue/status",
    [](const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        Json::Value res;
        res["queue_size"] = queueSize.load();
        auto resp = drogon::HttpResponse::newHttpJsonResponse(res);
        callback(resp);
    },
    {drogon::Get}
);

    drogon::app().addListener("0.0.0.0", 5555);
    drogon::app().setThreadNum(12);  // Optional: app thread pool
    drogon::app().run();
}
