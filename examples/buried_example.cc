#include <chrono>
#include <thread>

#include "buried.h"

int main() {
    Buried* buried = Buried_Create("D:/buried");

    if (!buried) {
        return -1;
    }

    BuriedConfig config;

    config.host = "localhost";
    config.port = "5678";
    config.topic = "/buried";
    config.user_id = "test_user";
    config.app_version = "1.0.0";
    config.app_name = "test_app";
    config.custom_data = "{\"test\":\"test\"}";

    Buried_Start(buried, &config);

    for (int i = 0; i < 5; i ++) {
        Buried_Report(buried, "button_click", "{\"page\":\"home\"}", i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(12));

    Buried_Destroy(buried);

    return 0;
}