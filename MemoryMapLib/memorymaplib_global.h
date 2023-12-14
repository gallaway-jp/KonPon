#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MEMORYMAPLIB_LIB)
#  define MEMORYMAPLIB_EXPORT Q_DECL_EXPORT
# else
#  define MEMORYMAPLIB_EXPORT Q_DECL_IMPORT
# endif
#else
# define MEMORYMAPLIB_EXPORT
#endif
