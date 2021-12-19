#ifndef NAVCORE_H
#define NAVCORE_H

//Bit field setter
#define BIT(x) (1<<x)

#define BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

#endif