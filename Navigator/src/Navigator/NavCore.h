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

#define BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

#endif
