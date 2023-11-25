#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ANKICONNECTLIB_LIB)
#  define ANKICONNECTLIB_EXPORT Q_DECL_EXPORT
# else
#  define ANKICONNECTLIB_EXPORT Q_DECL_IMPORT
# endif
#else
# define ANKICONNECTLIB_EXPORT
#endif
