#ifndef __MODULE_H__
#define __MODULE_H__

#include <api/module.h>
#include <string>

struct __ModuleInit
{
	template <typename ...Dependencies> struct CheckName;
	template <typename ...Dependencies> struct CheckDependencies;

	inline static std::string commasToSpaces(const std::string &s)
	{
		string result = s;
		std::replace(result.begin(), result.end(), ',', ' ');
		return result;
	}
};

#define	PROPER_MODULE_INIT(name, dependencies...) \
	typedef __ModuleInit::CheckName<name> __ModuleNameCheck; \
	typedef __ModuleInit::CheckDependencies<dependencies> __ModuleDependenciesCheck; \
	MODULE_INIT(name, __ModuleInit::commasToSpaces(#dependencies))

#endif //__MODULE_H__
