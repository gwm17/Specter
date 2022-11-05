#ifndef UNPACKER_H
#define UNPACKER_H

#include "Specter/Physics/SpecData.h"

namespace Specter {

    struct UnpackerResult
    {
        uint32_t* finalPosition = nullptr;
        std::vector<SpecData> data;
    };

    class Unpacker
    {
    public:
        using Ref = std::shared_ptr<Unpacker>;

        Unpacker() :
            m_bodyWordCount(0), m_moduleID(s_illegalModuleID)
        {

        }
        virtual ~Unpacker() {}

        const UnpackerResult& Unpack(uint32_t* begin, uint32_t* end)
        {
            m_parsedEvent = UnpackerResult();

            uint32_t* iter = begin;
            UnpackHeader(iter);
            ++iter;

            uint32_t* bodyEnd = iter + m_bodyWordCount;
            if(bodyEnd > end || m_moduleID == s_illegalModuleID)
            {
                SPEC_WARN("In Unpacker::Unpack() header unpack error (number of words: {0}, moduleID: {1}), data not parsed!", m_bodyWordCount, m_moduleID);
            }
            else
            {
                iter = UnpackBody(iter, bodyEnd);
                UnpackEnd(iter);
                ++iter;
            }

            m_parsedEvent.finalPosition = iter;

            return m_parsedEvent;
        }

        virtual bool IsHeader(uint32_t word) const = 0;

    protected:
        virtual bool IsBody(uint32_t word) const = 0;
        virtual bool IsEnd(uint32_t word) const = 0;

        virtual void UnpackHeader(uint32_t* word) = 0;
        virtual void UnpackEnd(uint32_t* word) = 0;

        uint32_t* UnpackBody(uint32_t* bodyBegin, uint32_t* bodyEnd)
        {
            uint32_t* iter = bodyBegin;
            while(iter != bodyEnd)
            {
                UnpackDatum(iter);
                iter++;
            }

            return iter;
        }

        virtual void UnpackDatum(uint32_t* word) = 0;

        UnpackerResult m_parsedEvent;
        uint64_t m_bodyWordCount;
        uint32_t m_moduleID;

        static constexpr uint32_t s_illegalModuleID = 999;
    };
}

#endif