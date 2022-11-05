#ifndef CAEN_UNPACKER_H
#define CAEN_UNPACKER_H

#include "Unpacker.h"

namespace Specter {

    class CaenUnpacker : public Unpacker
    {
    public:
        CaenUnpacker();
        virtual ~CaenUnpacker();

        virtual bool IsHeader(uint32_t word) const override;

    private:
        virtual bool IsBody(uint32_t word) const override;
        virtual bool IsEnd(uint32_t word) const override;

        virtual void UnpackHeader(uint32_t* word) override;
        virtual void UnpackEnd(uint32_t* word) override;

        virtual void UnpackDatum(uint32_t* word) override;

        static constexpr uint32_t s_typeMask = 0x07000000;
        static constexpr uint32_t s_typeHeader = 0x02000000;
        static constexpr uint32_t s_typeBody = 0x00000000;
        static constexpr uint32_t s_typeEnd = 0x04000000;

        static constexpr uint32_t s_geoAddressShift = 27;
        static constexpr uint32_t s_geoAddressMask = 0xf8000000;

        static constexpr uint32_t s_headerCountShift = 8;
        static constexpr uint32_t s_headerCountMask = 0x00003f00;

        static constexpr uint32_t s_dataChannelShift = 16;
        static constexpr uint32_t s_dataChannelMask = 0x001f0000;
        static constexpr uint32_t s_dataMask = 0x00000fff;
        static constexpr uint32_t s_dataStatusMask = 0x00003000;
        static constexpr uint32_t s_dataOverflow = 0x00001000;
        static constexpr uint32_t s_dataUnderflow = 0x00002000;
    };
}


#endif