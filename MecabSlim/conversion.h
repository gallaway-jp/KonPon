#ifndef MECAB_CONVERSION_H_
#define MECAB_CONVERSION_H_

namespace Conversion {
	unsigned short convertULongLongToUShort(unsigned long long data);

	unsigned short convertLongLongToUShort(long long data);

	unsigned int convertULongLongToUInt(unsigned long long data);

	int convertULongLongToInt(unsigned long long data);

	unsigned long convertULongLongToULong(unsigned long long data);

	long convertULongLongToLong(unsigned long long data);
}

#endif