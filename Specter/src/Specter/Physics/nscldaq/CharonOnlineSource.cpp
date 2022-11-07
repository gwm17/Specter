#include "CharonOnlineSource.h"
#include "Unpackers/CaenUnpacker.h"
#include "Unpackers/MesyTecUnpacker.h"

namespace Specter {

    CharonOnlineSource::CharonOnlineSource(const std::string& hostname, const std::string& port) :
        DataSource(0), m_client(hostname, port)
    {
        m_validFlag = m_client.IsConnected();
        m_readyEvents.emplace_back();

        m_unpackers.push_back(std::make_shared<CaenUnpacker>());
        m_unpackers.push_back(std::make_shared<MesyTecUnpacker>());
    }

    CharonOnlineSource::~CharonOnlineSource()
    {
    }

    void CharonOnlineSource::ProcessData()
    {
        if(!m_client.IsConnected())
        {
            m_validFlag = false;
            return;
        }

        if(m_client.GetNextEvent(m_rawBuffer))
        {
            m_event.clear();
            UnpackRawBuffer();
            m_isEventReady = true;
        }
    }

    void CharonOnlineSource::UnpackRawBuffer()
    {
        uint32_t* iter = (uint32_t*) m_rawBuffer.data();
        uint32_t* end = iter + m_rawBuffer.size() / sizeof(uint32_t);
        bool wasUnpacked = false;

        UnpackerResult result;
        while(iter < end)
        {
            wasUnpacked = false;
            for(auto& unpacker : m_unpackers)
            {
                if(unpacker->IsHeader(*iter))
                {
                    result = unpacker->Unpack(iter, end);
                    iter = result.finalPosition;
                    wasUnpacked = true;
                    m_event.insert(m_event.end(), result.data.begin(), result.data.end());
                    break;
                }
            }

            if(!wasUnpacked)
            {
                iter++;
            }
        }

        m_readyEvents[0] = m_event;
    }
}