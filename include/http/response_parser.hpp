#pragma once

#include <http/config.hpp>
#include <string_view>

namespace http {
struct response {
  struct metadata {
    struct version {
      int major_ = -1;
      int minor_ = -1;
    };

    version version_;
    int status_ = -1;
    char const *status_line_begin_ = nullptr;
    char const *status_line_end_ = nullptr;
    char const *reason_phrase_begin_ = nullptr;
    char const *reason_phrase_end_ = nullptr;
  };

  metadata md_;

  std::string_view status_line() const noexcept {
    return {md_.status_line_begin_,
            static_cast<std::size_t>(md_.status_line_end_ -
                                     md_.status_line_begin_)};
  }

  std::string_view reason_phrase() const noexcept {
    return {md_.reason_phrase_begin_,
            static_cast<std::size_t>(md_.reason_phrase_end_ -
                                     md_.reason_phrase_begin_)};
  }
};

HTTP_DECL
response parse_response(std::string_view &sv);

} // namespace http
