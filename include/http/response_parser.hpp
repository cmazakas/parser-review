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
    std::string_view status_line_;
    std::string_view reason_phrase_;
  };

  metadata md_;
};

HTTP_DECL
response parse_response(std::string_view &sv);

} // namespace http
