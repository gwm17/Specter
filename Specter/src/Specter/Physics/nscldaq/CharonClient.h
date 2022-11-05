#ifndef CHARON_CLIENT_H
#define CHARON_CLIENT_H

#include "Specter/Utils/ThreadSafeQueue.h"
#include <asio.hpp>
#include <thread>

namespace Specter {

    struct StygianMessage
    {
        uint64_t size;
        std::vector<uint8_t> body;
    };

    class CharonClient
    {
    public:
        CharonClient(const std::string& hostname, const std::string& port);
        ~CharonClient();

        bool GetNextEvent(std::vector<uint8_t>& event);

        void Connect(const std::string& hostname, const std::string& port);
        void Disconnect();

        const bool IsConnected() const { return m_socket.is_open(); }

    private:
        void ReadHeader();
        void ReadBody();

        asio::io_context m_context;
        asio::ip::tcp::socket m_socket;
        std::thread m_ioThread;

        StygianMessage m_tempMessage;
        ThreadSafeQueue<StygianMessage> m_queue;
    };

}

#endif