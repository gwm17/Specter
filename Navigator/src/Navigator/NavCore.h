#ifndef NAVCORE_H
#define NAVCORE_H

#ifdef NAV_WINDOWS
	#error "Navigator is not compatible with Windows!"
#endif

#ifdef NAV_APPLE
	#error "Navigator is not compatible with MacOS!"
#endif

//Bit field setter
#define BIT(x) (1<<x)

#define BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

#endif