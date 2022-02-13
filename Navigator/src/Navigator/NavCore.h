#ifndef NAVCORE_H
#define NAVCORE_H


#ifdef NAV_WINDOWS
	#ifdef NAV_EXPORT
		#define NAV_API __declspec(dllexport)
	#else
		#define NAV_API __declspec(dllimport)
	#endif

	#ifdef _MSC_VER
		#pragma warning (disable: 4127)             // condition expression is constant
		#pragma warning (disable: 4251)				// class 'xxx' needs to have dll-interface to be used by clients of struct 'xxx' // when NAV_API is set to__declspec(dllexport)
		#pragma warning (disable: 4091)				// '__declspec(dllimport)': ignored on left of class 'xxx' when no variable is declared
		#pragma warning (disable: 4996)             // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
		#if defined(_MSC_VER) && _MSC_VER >= 1922   // MSVC 2019 16.2 or later
			#pragma warning (disable: 5054)             // operator '|': deprecated between enumerations of different types
		#endif
		#pragma warning (disable: 26451)            // [Static Analyzer] Arithmetic overflow : Using operator 'xxx' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator 'xxx' to avoid overflow(io.2).
		#pragma warning (disable: 26495)            // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
		#pragma warning (disable: 26812)            // [Static Analyzer] The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum.3).
	#endif
#else
	#define NAV_API
#endif


//Bit field setter
#define BIT(x) (1<<x)

#define BIND_EVENT_FUNCTION(x) [this](auto&&... args) -> decltype(auto) { return this->x(std::forward<decltype(args)>(args)...); }

#endif
