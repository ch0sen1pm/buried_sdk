#include "buried_core.h"

#include <utility>

#include "context/context.h"
#include "report/buried_report.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


void Buried::InitWorkPath_(const std::string& work_dir) {
    std::filesystem::path base_path(work_dir);

    if (!std::filesystem::exists(base_path)) {
        std::filesystem::create_directories(base_path);
    }

    work_path_ = base_path / "buried";

    if (!std::filesystem::exists(work_path_)) {
        std::filesystem::create_directories(work_path_);
    }

}


void Buried::InitLogger_() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    std::filesystem::path log_path = work_path_ / "buried.log";

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        log_path.string(), true);

    logger_ = std::shared_ptr<spdlog::logger>(
        new spdlog::logger(
            "buried_sink",
            {console_sink, file_sink}));

    logger_->set_pattern("[%c] [%s:%#] [%l] %v");
    logger_->set_level(spdlog::level::trace);
}