#ifndef _PLATFORMUTILS_H_
#define _PLATFORMUTILS_H_

#include <string>

namespace Kaimos {

	class FileDialogs
	{
	public:

		// If cancelled, returns empty string
		static std::string OpenFile(const char* filter);

		// If cancelled, returns empty string
		static std::string SaveFile(const char* filter);
	};
}
#endif //_PLATFORMUTILS_H_
