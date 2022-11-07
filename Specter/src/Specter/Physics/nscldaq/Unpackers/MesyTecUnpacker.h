#ifndef MESY_TEC_UNPACKER_H
#define MESY_TEC_UNPACKER_H

#include "Unpacker.h"

namespace Specter {

    class MesyTecUnpacker : public Unpacker
    {
    public:
        MesyTecUnpacker();
        virtual ~MesyTecUnpacker();

        virtual bool IsHeader(uint32_t word) const override;

    private:
        virtual bool IsBody(uint32_t word) const override;
        virtual bool IsEnd(uint32_t word) const override;

        virtual void UnpackHeader(uint32_t* word) override;
        virtual void UnpackEnd(uint32_t* word) override;

        virtual void UnpackDatum(uint32_t* word) override;

        static constexpr uint32_t s_typeMask = 0xc0000000;
        static constexpr uint32_t s_typeHeader = 0x40000000;
        static constexpr uint32_t s_typeBody = 0x00000000;
        static constexpr uint32_t s_typeEnd = 0xc0000000;

        static constexpr uint32_t s_idShift = 16;
        static constexpr uint32_t s_idMask = 0x00ff0000;

        static constexpr uint32_t s_headerCountMask = 0x00000fff;

        static constexpr uint32_t s_dataChannelShift = 16;
        static constexpr uint32_t s_dataChannelMask = 0x001f0000;
        static constexpr uint32_t s_dataMask = 0x0000ffff;
    };
}

#endif