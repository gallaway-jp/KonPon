#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(SETTINGSLIB_LIB)
#  define SETTINGSLIB_EXPORT Q_DECL_EXPORT
# else
#  define SETTINGSLIB_EXPORT Q_DECL_IMPORT
# endif
#else
# define SETTINGSLIB_EXPORT
#endif
