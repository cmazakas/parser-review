#include <http/response_parser.hpp>

#include <catch2/catch_test_macros.hpp>

#include <boost/parser/parser.hpp>
#include <string_view>

TEST_CASE("using semantic actions") {
  {
    std::string_view const input =
        "HTTP/1.1 404 content not found or something like that";

    auto buf = input;
    auto response = http::parse_response(buf);
    CHECK(response.md_.version_.major_ == 1);
    CHECK(response.md_.version_.minor_ == 1);
    CHECK(response.md_.status_ == 404);
    CHECK(response.md_.reason_phrase_ ==
          "content not found or something like that");
    CHECK(response.md_.status_line_ == input);

    CHECK(buf.empty());
  }

  {
    std::string_view const input =
        "HTTP/1.1 404 content not found\r\nor something like that";

    auto buf = input;
    auto response = http::parse_response(buf);
    CHECK(response.md_.version_.major_ == 1);
    CHECK(response.md_.version_.minor_ == 1);
    CHECK(response.md_.status_ == 404);
    CHECK(response.md_.reason_phrase_ == "content not found");
    CHECK(response.md_.status_line_ == "HTTP/1.1 404 content not found");

    CHECK(buf == "\r\nor something like that");
  }
}
