#include "report/buried_report.h"

#include <chrono>
#include <filesystem>
#include <utility>

#include "boost/asio/deadline_timer.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "context/context.h"
#include "crypt/crypt.h"
#include "database/database.h"
#include "report/http_report.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "boost/asio/post.hpp"

namespace buried {

static constexpr char kDbName[] = "buried.db";

class BuriedReportImpl {
public:
    BuriedReportImpl(std::shared_ptr<spdlog::logger> logger,
                     CommonService common_service, std::string work_path)
        : logger_(std::move(logger)),
          common_service_(std::move(common_service)),
          work_dir_(std::move(work_path)) {
        
        if (logger_ == nullptr) {
            logger_ = spdlog::stdout_color_mt("buried");
        }

        std::string key = AESCrypt::GetKey("buried_salt", "buried_password");

        crypt_ = std::make_unique<AESCrypt>(key);

        SPDLOG_LOGGER_INFO(logger_, "BuriedReportImpl init success");

        boost::asio::post(
            Context::GetGlobalContext().GetReportStrand(),
            [this]() { Init_(); });
    }

    ~BuriedReportImpl() = default;

    void Start();

    void InsertData(const BuriedData& data);

private:
    void Init_();

    void ReportCache_();

    void NextCycle_();

    BuriedDb::Data MakeDbData_(const BuriedData& data);

    std::string GenReportData_(const std::vector<BuriedDb::Data>& datas);

    bool ReportData_(const std::string& data);

private:
    std::shared_ptr<spdlog::logger> logger_;
    std::string work_dir_;
    std::unique_ptr<BuriedDb> db_;
    CommonService common_service_;
    std::unique_ptr<Crypt> crypt_;

    std::unique_ptr<boost::asio::deadline_timer> timer_;

    std::vector<BuriedDb::Data> data_caches_;
};

void BuriedReportImpl::Init_() {
    std::filesystem::path db_path = work_dir_;

    SPDLOG_LOGGER_INFO(
        logger_,
        "BuriedReportImpl init db path: {}",
        db_path.string());

    db_path /= kDbName;

    db_ = std::make_unique<BuriedDb>(db_path.string());
}


} // namespace buried