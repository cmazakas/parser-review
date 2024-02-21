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
    CHECK(response.reason_phrase() ==
          "content not found or something like that");
    CHECK(response.status_line() == input);

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
    CHECK(response.reason_phrase() == "content not found");
    CHECK(response.status_line() == "HTTP/1.1 404 content not found");

    CHECK(buf == "\r\nor something like that");
  }

  {
    std::string_view const inputs[] = {"HTTP/lmao", "rawr", "HTTP/20.02"};

    for (auto const input : inputs) {
      auto buf = input;
      auto response = http::parse_response(buf);
      CHECK(response.md_.version_.major_ == -1);
      CHECK(response.md_.version_.minor_ == -1);
      CHECK(response.md_.status_ == -1);
      CHECK(response.reason_phrase() == "");
      CHECK(response.status_line() == "");

      CHECK(buf == input);
    }
  }

  {
    std::string_view const inputs[] = {"HTTP/2.0 \r\n", "HTTP/2.0 rawr",
                                       "HTTP/2.0 400000000004"};

    for (auto const input : inputs) {
      auto buf = input;
      auto response = http::parse_response(buf);
      CHECK(response.md_.version_.major_ == 2);
      CHECK(response.md_.version_.minor_ == 0);
      CHECK(response.md_.status_ == -1);
      CHECK(response.reason_phrase() == "");
      CHECK(response.status_line() == "");

      CHECK(buf == input);
    }
  }
}
