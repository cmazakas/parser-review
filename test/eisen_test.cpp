#include <catch2/catch_test_macros.hpp>

#include <boost/parser/parser.hpp>

#include <iostream>
#include <string_view>
#include <vector>

namespace p = boost::parser;

struct metadata {
  std::vector<std::string_view> msgs_;
  int nlbraces_ = 0;
  int nrbraces_ = 0;
};

TEST_CASE("for eisen") {

  auto const on_lbrace = [](auto &ctx) {
    metadata &md = _globals(ctx);
    ++md.nlbraces_;
  };

  auto const on_rbrace = [](auto &ctx) {
    metadata &md = _globals(ctx);
    ++md.nrbraces_;
  };

  auto const on_content = [](auto &ctx) {
    metadata &md = _globals(ctx);
    md.msgs_.push_back(_attr(ctx));
    std::cout << _attr(ctx) << std::endl;
  };

  auto const on_complete = [](auto &ctx) {
    metadata &md = _globals(ctx);
    if (md.nlbraces_ != md.nrbraces_) {
      _pass(ctx) = false;
    }
  };

  auto const lbrace = p::lit("{")[on_lbrace];
  auto const rbrace = p::lit("}")[on_rbrace];

  auto const ascii = p::char_(32, 126);
  auto const comment =
      p::lit("/*") >> *(+(ascii - '*') >> !p::char_('/')) >> "*/";
  auto const content =
      p::string_view[+((ascii - p::char_("{}")) - comment)][on_content];

  auto const parser = (+lbrace >> +(content | comment) >> +rbrace)[on_complete];

  {
    std::string_view input = "{{{ rawr! }}}";
    metadata md;
    auto pos = input.begin();
    auto result =
        p::prefix_parse(pos, input.end(), p::with_globals(parser, md), p::ws);
    CHECK(result);
    CHECK(md.nlbraces_ == 3);
    CHECK(md.nrbraces_ == 3);
    CHECK(md.msgs_.at(0) == "rawr!");
  }

  {
    std::string_view input = "{{{ rawr! }}";
    metadata md;
    auto pos = input.begin();
    auto result =
        p::prefix_parse(pos, input.end(), p::with_globals(parser, md), p::ws);
    CHECK(!result);
    CHECK(md.nlbraces_ == 3);
    CHECK(md.nrbraces_ == 2);
    CHECK(md.msgs_.at(0) == "rawr!");
  }

  {
    std::string_view input = "{ rawr! }}";
    metadata md;
    auto pos = input.begin();
    auto result =
        p::prefix_parse(pos, input.end(), p::with_globals(parser, md), p::ws);
    CHECK(!result);
    CHECK(md.nlbraces_ == 1);
    CHECK(md.nrbraces_ == 2);
    CHECK(md.msgs_.at(0) == "rawr!");
  }

  {
    std::string_view input = "{{{ ra{}wr! }}}";
    metadata md;
    auto pos = input.begin();
    auto result =
        p::prefix_parse(pos, input.end(), p::with_globals(parser, md), p::ws);
    CHECK(!result);
    CHECK(md.nlbraces_ == 3);
    CHECK(md.nrbraces_ == 0);
    CHECK(md.msgs_.at(0) == "ra");
  }

  {
    std::string_view input = "{{{ ra/* {lmao}}}}}}} */wr! }}}";
    metadata md;
    auto pos = input.begin();
    auto result =
        p::prefix_parse(pos, input.end(), p::with_globals(parser, md), p::ws);
    CHECK(result);
    CHECK(md.nlbraces_ == 3);
    CHECK(md.nrbraces_ == 3);
    CHECK(md.msgs_.at(0) == "ra");
    CHECK(md.msgs_.at(1) == "wr!");
  }
}

auto const on_text = [](auto &ctx) { std::cout << _attr(ctx) << std::endl; };

p::rule<struct braced> const braced = "braced";
auto const ascii = p::char_(32, 126);
auto const c = ascii - p::char_("{}");
auto const braced_def = p::lit('{') >>
                        *(p::string_view[+c][on_text] | braced) >> '}';
BOOST_PARSER_DEFINE_RULES(braced);

TEST_CASE("eisen take 2") {
  {
    std::string_view input = "{}";

    auto pos = input.begin();
    auto end = input.end();
    auto result = p::prefix_parse(pos, end, braced, p::ws);
    CHECK(result);
    CHECK(pos == end);
  }

  {
    std::string_view input = "{ {} lmao {} {{{rawr{{}}}}}  }";

    auto pos = input.begin();
    auto end = input.end();
    auto result = p::prefix_parse(pos, end, braced, p::ws);
    CHECK(result);
    CHECK(pos == end);
  }
}
