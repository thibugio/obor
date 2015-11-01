/* TCPListener.h */

#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H

#include "TCP_Connection.h"

#define TCP_PORTNO 13;

using boost::asio::ip::tcp;

class TCPListener {
    public:
        TCPListener();

    private:
        boost::asio::io_service io;
        tcp::acceptor listener;
        void begin();
        void rcv_handler(TCP_Connection::pointer conn, const boost::system::error_code& ec);
};

#endif
