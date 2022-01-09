#ifndef NAVCORE_H
#define NAVCORE_H


#ifdef NAV_WINDOWS
	#ifdef NAV_EXPORT
		#define NAV_API __declspec(dllexport)
	#else
		#define NAV_API __declspec(dllimport)
	#endif
#else
	#define NAV_API
#endif


//Bit field setter
#define BIT(x) (1<<x)

#define BIND_EVENT_FUNCTION(x) [this](auto&&... args) -> decltype(auto) { return this->x(std::forward<decltype(args)>(args)...); }

#endif
