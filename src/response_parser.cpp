#include <http/response_parser.hpp>

#include <boost/parser/parser.hpp>
#include <iostream>

namespace http {
namespace {
// HTTP-message = start-line CRLF *( field-line CRLF ) CRLF [
//  message-body ]
//
// start-line = request-line / status-line
//
// status-line = HTTP-version SP status-code SP [ reason-phrase ]
// HTTP-version = HTTP-name "/" DIGIT "." DIGIT
// HTTP-name = %x48.54.54.50 ; HTTP
// status-code = 3DIGIT

auto const on_major = [](auto &ctx) {
  auto const major = _attr(ctx);
  if (major < 1 || major > 9) {
    _pass(ctx) = false;
    return;
  }
  _globals(ctx).md_.version_.major_ = major;
};

auto const on_minor = [](auto &ctx) {
  auto const minor = _attr(ctx);
  if (minor < 0 || minor > 9) {
    _pass(ctx) = false;
    return;
  }
  _globals(ctx).md_.version_.minor_ = minor;
};

auto const on_status = [](auto &ctx) {
  auto status = _attr(ctx);
  if (status < 100 || status > 599) {
    _pass(ctx) = false;
    return;
  }
  _globals(ctx).md_.status_ = status;
  auto subrange = _where(ctx);
  _globals(ctx).md_.status_line_ = {
      _begin(ctx), static_cast<std::size_t>(subrange.end() - _begin(ctx))};
};

auto const http_name = boost::parser::lit("HTTP");

boost::parser::rule<struct http_version> http_version = "http_version";
auto const http_version_def = http_name >> "/" >>
                              boost::parser::uint_[on_major] >> "." >>
                              boost::parser::uint_[on_minor];

boost::parser::rule<struct status_code> status_code = "status_code";
auto const status_code_def = boost::parser::uint_[on_status];

// reason-phrase = 1*( HTAB / SP / VCHAR / obs-text )
// VCHAR         = %x21-7E

auto const on_vchar = [](auto &ctx) {
  auto ch = _attr(ctx);
  if (ch != '\t' && ch != ' ' && (ch < '\x21' || ch > '\x7e')) {
    _pass(ctx) = false;
    return;
  }

  auto sub = _where(ctx);
  auto &reason_phrase = _globals(ctx).md_.reason_phrase_;
  if (!reason_phrase.data()) {
    reason_phrase = std::string_view(sub.begin(), sub.end() - sub.begin());
  } else {
    reason_phrase = std::string_view(reason_phrase.begin(), sub.end());
    _globals(ctx).md_.status_line_ = {
        _globals(ctx).md_.status_line_.data(),
        static_cast<std::size_t>(sub.end() - _begin(ctx))};
  }
};

boost::parser::rule<struct reason_phrase> reason_phrase = "reason_phrase";
auto const reason_phrase_def =
    boost::parser::omit[+boost::parser::char_[on_vchar]];

boost::parser::rule<struct status_line> status_line = "status_line";
auto const status_line_def = http_version >> " " >> status_code >> " " >>
                             -(reason_phrase);

BOOST_PARSER_DEFINE_RULES(http_version, status_code, reason_phrase, status_line)
} // namespace

response parse_response(std::string_view &sv) {
  response res;

  auto pos = sv.begin();
  auto begin = sv.begin();
  auto end = sv.end();

  auto b = boost::parser::prefix_parse(
      pos, end, boost::parser::with_globals(status_line, res));
  (void)b;

  sv.remove_prefix(static_cast<std::size_t>(pos - begin));
  return res;
}
} // namespace http
