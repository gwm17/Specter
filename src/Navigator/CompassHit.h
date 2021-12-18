#ifndef COMPASS_HIT_H
#define COMPASS_HIT_H

namespace Navigator {

  struct CompassHit 
  {
    uint16_t board = 400;
    uint16_t channel = 400;
    uint64_t timestamp = 0;
    uint16_t lgate = 0;
    uint16_t sgate = 0;
    uint32_t flags = 0;
    uint32_t Ns = 0;
  };

}

#endif
