/* TCPSender.h */

#ifndef TCP_SENDER_H
#define TCP_SENDER_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>

#define TCP_HOST "hostname"
#define TCP_CLIENT "client"

using boost::asio::ip::tcp;

class TCPSender {
    public:
        TCPSender(char * hostname);
        std::string getHost();
        void connect();
        void beginRead();

    private:
        boost::asio::io_service io;
        tcp::resolver resolver;
        tcp::socket sock;
        std::string host;
        void * onReceive;
        void handle_error(boost::asio::error_code ec);
};

#endif
