#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(GUILIB_LIB)
#  define GUILIB_EXPORT Q_DECL_EXPORT
# else
#  define GUILIB_EXPORT Q_DECL_IMPORT
# endif
#else
# define GUILIB_EXPORT
#endif
