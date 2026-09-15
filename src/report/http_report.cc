#include "report/http_report.h"

#include "boost/asio/connect.hpp"
#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/version.hpp"
#include "spdlog/spdlog.h"

#include <utility>
#include <exception>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp = net::ip::tcp;

namespace buried {
static net::io_context ioc;

HttpReporter::HttpReporter(std::shared_ptr<spdlog::logger> logger)
    : logger_(std::move(logger)) {}

bool HttpReporter::Report() {
    try {
        constexpr int kHttpVersion = 11;

        tcp::resolver resolver(ioc);

        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host_, port_);

        stream.connect(results);

        http::request<http::string_body> req {
            http::verb::post, topic_, kHttpVersion};

        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");

        req.body() = body_;
        req.prepare_payload();

        http::write(stream, req);

        beast::flat_buffer buffer;

        http::response<http::dynamic_body> res;

        http::read(stream, buffer, res);

        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        if (ec && ec != beast::errc::not_connected) {
            throw beast::system_error{ec};
        }

        if (res.result() != http::status::ok) {
            SPDLOG_LOGGER_ERROR(
                logger_,
                "report error {}",
                res.result_int());
            return false;
        }

        std::string response_body = beast::buffers_to_string(res.body().data());

        SPDLOG_LOGGER_TRACE(
            logger_,
            "report success {}",
            response_body);

        return true;
    } catch (const std::exception& e) {
        SPDLOG_LOGGER_ERROR(
            logger_,
            "report error {}",
            e.what());
        
        return false;
    }
}
} // namespace buried