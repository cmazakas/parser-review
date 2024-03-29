#include <boost/parser/parser.hpp>

#include <boost/parser/error_handling_fwd.hpp>
#include <http/response_parser.hpp>

#include <boost/hana/tuple.hpp>

namespace hana = boost::hana;
namespace parser = boost::parser;

#include <iostream>

using namespace hana::literals;

namespace http {
namespace {
// HTTP-message = start-line CRLF *( field-line CRLF ) CRLF [
//  message-body ]
// start-line = request-line / status-line

auto const on_status_code = [](auto &ctx) {
  auto status = _attr(ctx);
  if (status < 100 || status > 599) {
    _pass(ctx) = false;
    return;
  }
  response::metadata &md = _globals(ctx).md_;
  md.status_ = status;
  auto subrange = _where(ctx);
  md.status_line_begin_ = _begin(ctx);
  md.status_line_end_ = subrange.end();
};

template <class> struct X;

// HTTP-version = HTTP-name "/" DIGIT "." DIGIT
// HTTP-name = %x48.54.54.50 ; HTTP
auto const on_version = [](auto &ctx) {
  auto const &tup = _attr(ctx);
  auto const major = tup[0_c];
  auto const minor = tup[1_c];
  if (major > 9 || minor > 9) {
    _pass(ctx) = false;
    return;
  }

  response::metadata &md = _globals(ctx).md_;
  md.version_.major_ = major;
  md.version_.minor_ = minor;
};

auto const http_name = parser::lit("HTTP");
auto const http_version =
    (http_name >> "/" >> parser::uint_ >> "." >> parser::uint_)[on_version];

// status-code = 3DIGIT
auto const status_code = parser::uint_[on_status_code];

// reason-phrase = 1*( HTAB / SP / VCHAR / obs-text )
// VCHAR         = %x21-7E
auto const on_reason_phrase = [](auto &ctx) {
  auto sv = _attr(ctx);

  response::metadata &md = _globals(ctx).md_;

  md.reason_phrase_begin_ = sv.begin();
  md.reason_phrase_end_ = sv.end();

  md.status_line_end_ = sv.end();
  ;
};

auto const reason_phrase =
    parser::string_view[+(parser::char_('\x21', '\x7e') | '\t' | ' ')]
                       [on_reason_phrase];

// status-line = HTTP-version SP status-code SP [ reason-phrase ]
auto const status_line = http_version >> " " >> status_code >> " " >>
                         -(reason_phrase);

} // namespace

response parse_response(std::string_view &sv) {
  response res;

  auto pos = sv.begin();
  auto begin = sv.begin();
  auto end = sv.end();

  auto b =
      parser::prefix_parse(pos, end, parser::with_globals(status_line, res));
  (void)b;

  sv.remove_prefix(static_cast<std::size_t>(pos - begin));
  return res;
}
} // namespace http
