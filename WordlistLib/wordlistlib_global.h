#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(WORDLISTLIB_LIB)
#  define WORDLISTLIB_EXPORT Q_DECL_EXPORT
# else
#  define WORDLISTLIB_EXPORT Q_DECL_IMPORT
# endif
#else
# define WORDLISTLIB_EXPORT
#endif
