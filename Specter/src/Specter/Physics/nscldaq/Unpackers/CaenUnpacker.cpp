#include "CaenUnpacker.h"
#include "Specter/Utils/Functions.h"

namespace Specter {

    CaenUnpacker::CaenUnpacker() :
        Unpacker()
    {
    }

    CaenUnpacker::~CaenUnpacker() {}

    void CaenUnpacker::UnpackHeader(uint32_t* word)
    {
        if(!IsHeader(*word))
        {
            SPEC_WARN("In CaenADCUnpacker::UnpackHeader() found non-header word!");
            m_bodyWordCount = 0;
            m_moduleID = s_illegalModuleID;
            return;
        }

        m_bodyWordCount = (*word & s_headerCountMask) >> s_headerCountShift;
        m_moduleID = (*word & s_geoAddressMask) >> s_geoAddressShift;
    }

    void CaenUnpacker::UnpackEnd(uint32_t* word)
    {
        if(!IsEnd(*word))
        {
            SPEC_WARN("In CaenADCUnpacker::UnpackEnd() found non-end word!");
        }
        //CAEN doesnt really put anything useful here
    }

    void CaenUnpacker::UnpackDatum(uint32_t* word)
    {
        if(!IsBody(*word))
        {
            SPEC_WARN("In CaenADCUnpacker::UnpackDatum() found non-body word!");
            return;
        }

        SpecData datum;
        uint32_t channel = (*word & s_dataChannelMask) >> s_dataChannelShift;
        datum.id = Utilities::GetBoardChannelUUID(m_moduleID, channel);
        datum.longEnergy = (*word & s_dataMask);
        m_parsedEvent.data.push_back(datum);
    }

    bool CaenUnpacker::IsHeader(uint32_t word) const
    {
        return ((word & s_typeMask) == s_typeHeader);
    }

    bool CaenUnpacker::IsBody(uint32_t word) const
    {
        return ((word & s_typeMask) == s_typeBody);
    }

    bool CaenUnpacker::IsEnd(uint32_t word) const
    {
        return ((word & s_typeMask) == s_typeEnd);
    }
}