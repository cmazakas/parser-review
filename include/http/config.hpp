#pragma once

#include <boost/config.hpp>

#if defined(FIONA_DYN_LINK)
#if defined(FIONA_SOURCE)
#define HTTP_DECL BOOST_SYMBOL_EXPORT
#else
#define HTTP_DECL BOOST_SYMBOL_IMPORT
#endif
#else
#define HTTP_DECL
#endif
