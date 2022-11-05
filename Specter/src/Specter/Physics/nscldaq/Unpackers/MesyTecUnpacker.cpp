#include "MesyTecUnpacker.h"
#include "Specter/Utils/Functions.h"

namespace Specter {

    MesyTecUnpacker::MesyTecUnpacker() :
        Unpacker()
    {
    }

    MesyTecUnpacker::~MesyTecUnpacker() {}

    void MesyTecUnpacker::UnpackHeader(uint32_t* word)
    {
        if(!IsHeader(*word))
        {
            SPEC_WARN("In MesyQDCUnpacker::UnpackHeader() found non-header word!");
            m_moduleID = s_illegalModuleID;
            m_bodyWordCount = 0;
        }

        m_moduleID = (*word & s_idMask) >> s_idShift;
        m_bodyWordCount = (*word & s_headerCountMask) - 1; //For MesyTec, count includes the end word
    }

    void MesyTecUnpacker::UnpackDatum(uint32_t* word)
    {
        if(!IsBody(*word))
        {
            SPEC_WARN("In MesyQDCUnpacker::UnpackDatum() found non-body word!");
            return;
        }
        
        SpecData datum;
        uint32_t channel = (*word & s_dataChannelMask) >> s_dataChannelShift;
        datum.id = Utilities::GetBoardChannelUUID(m_moduleID, channel);
        datum.longEnergy = (*word & s_dataMask);
    }

    void MesyTecUnpacker::UnpackEnd(uint32_t* word)
    {
        if(!IsEnd(*word))
        {
            SPEC_WARN("In MesyQDCUnpacker::UnpackEnd() found non-end word!");
        }
    }

    bool MesyTecUnpacker::IsHeader(uint32_t word) const
    {
        return ((word & s_typeMask) == s_typeHeader);
    }

    bool MesyTecUnpacker::IsBody(uint32_t word) const
    {
        return ((word & s_typeMask) == s_typeBody);
    }

    bool MesyTecUnpacker::IsEnd(uint32_t word) const
    {
        return ((word & s_typeMask) == s_typeEnd);
    }
}