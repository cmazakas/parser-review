#include <catch2/catch_test_macros.hpp>

#include <boost/parser/parser.hpp>
#include <string_view>

// HTTP-message = start-line CRLF *( field-line CRLF ) CRLF [
//  message-body ]
//
// start-line = request-line / status-line
//
// status-line = HTTP-version SP status-code SP [ reason-phrase ]
// HTTP-version = HTTP-name "/" DIGIT "." DIGIT
// HTTP-name = %x48.54.54.50 ; HTTP
// status-code = 3DIGIT

namespace http {

struct version {
  int major_ = -1;
  int minor_ = -1;
};
static_assert(std::is_aggregate_v<std::decay_t<version &>>);

struct status_line {
  version v_;
  int status_ = -1;
};
static_assert(std::is_aggregate_v<std::decay_t<status_line &>>);

} // namespace http

boost::parser::rule<struct http_version, http::version> http_version =
    "http_version";

auto const digit_parser = boost::parser::parser_interface<
    boost::parser::uint_parser<int, 10, 1, 1>>();

auto const http_name = boost::parser::lit("HTTP");
auto http_version_def = http_name >> "/" >> digit_parser >> "." >> digit_parser;

auto const status_code = boost::parser::parser_interface<
    boost::parser::uint_parser<int, 10, 3, 3>>();

boost::parser::rule<struct status_line, http::status_line> status_line =
    "status_line";

auto const status_line_def = http_version >> " " >> status_code;

BOOST_PARSER_DEFINE_RULES(http_version, status_line)

TEST_CASE("parsing http/1.1 messages") {
  std::string_view const input = "HTTP/1.1 200";

  {
    auto pos = input.begin();
    http::version version;
    auto b =
        boost::parser::prefix_parse(pos, input.end(), http_version, version);
    CHECK(b);
    CHECK(version.major_ == 1);
    CHECK(version.minor_ == 1);

    REQUIRE(pos != input.end());
    CHECK(*pos == ' ');
  }

  {
    auto pos = input.begin();
    http::status_line md;
    auto b = boost::parser::prefix_parse(pos, input.end(), status_line, md);
    CHECK(b);
    CHECK(md.v_.major_ == 1);
    CHECK(md.v_.minor_ == 1);
    CHECK(md.status_ == 200);

    CHECK(pos == input.end());
  }
}
