#include "conversion.h"

#include <climits>
#include <stdexcept>

namespace Conversion {
	extern inline unsigned short convertULongLongToUShort(unsigned long long data) {
		if (data > USHRT_MAX) {
			throw std::length_error("Data is too large for an unsigned short!");
		}
		return static_cast<unsigned short>(data);
	}
	
	extern inline unsigned short convertLongLongToUShort(long long data) {
		if (data > USHRT_MAX) {
			throw std::length_error("Data is too large for an unsigned short!");
		}
		return static_cast<unsigned short>(data);
	}

	extern inline unsigned int convertULongLongToUInt(unsigned long long data) {
		if (data > UINT_MAX) {
			throw std::length_error("Data is too large for an unsigned int!");
		}
		return static_cast<unsigned int>(data);
	}

	extern inline int convertULongLongToInt(unsigned long long data) {
		if (data > INT_MAX) {
			throw std::length_error("Data is too large for an int!");
		}
		return static_cast<int>(data);
	}

	extern inline unsigned long convertULongLongToULong(unsigned long long data) {
		if (data > ULONG_MAX) {
			throw std::length_error("Data is too large for an unsigned long!");
		}
		return static_cast<unsigned long>(data);
	}

	extern inline long convertULongLongToLong(unsigned long long data) {
		if (data > LONG_MAX) {
			throw std::length_error("Data is too large for a long!");
		}
		return static_cast<long>(data);
	}
}