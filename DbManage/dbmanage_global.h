#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(DBMANAGE_LIB)
#  define DBMANAGE_EXPORT Q_DECL_EXPORT
# else
#  define DBMANAGE_EXPORT Q_DECL_IMPORT
# endif
#else
# define DBMANAGE_EXPORT
#endif
