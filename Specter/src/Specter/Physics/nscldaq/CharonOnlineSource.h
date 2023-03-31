#ifndef CHARON_ONLINE_SOURCE
#define CHARON_ONLINE_SOURCE

#include "Specter/Physics/DataSource.h"
#include "CharonClient.h"
#include "Unpackers/Unpacker.h"

namespace Specter {

    class CharonOnlineSource : public DataSource
    {
    public:
        CharonOnlineSource(const std::string& hostname, const std::string& port);
        virtual ~CharonOnlineSource();

        virtual void ProcessData() override;
        virtual std::vector<SpecEvent> GetEvents() override
        {
            m_isEventReady = false;
            return m_readyEvents;
        }
        virtual const bool IsEventReady() const override { return m_isEventReady; }

    private:
        void UnpackRawBuffer();

        CharonClient m_client;
        bool m_isEventReady;
        std::vector<uint8_t> m_rawBuffer;
        SpecEvent m_event;
        std::vector<SpecEvent> m_readyEvents;
        std::vector<Unpacker::Ref> m_unpackers;
    };
}


#endif