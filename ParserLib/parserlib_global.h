#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PARSERLIB_LIB)
#  define PARSERLIB_EXPORT Q_DECL_EXPORT
# else
#  define PARSERLIB_EXPORT Q_DECL_IMPORT
# endif
#else
# define PARSERLIB_EXPORT
#endif
