/* TCPSender.cpp */

#include "TCPSender.h"

using boost::asio::ip::tcp;
using namespace std;

TCPSender::TCPSender((void *)onReceiveFcn)
    : io(), resolver(io), sock(io), host(TCP_HOST), onReceive=onReceiveFcn {}

void TCPSender::connect() {
    tcp::resolver::query query(host.c_str(), TCP_CLIENT);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::connect(sock, endpoint_iterator);
}

void beginRead() {
    //spawn new thread somehow
    while(1) {
        std::vector<uint8_t> buf;
        boost::system::error_code ec;
        size_t nbytes = sock.read_some(boost::asio::buffer(buf), ec);
        if (ec == boost::asio::error::eof)
            break;
        else if (ec)
            handle_error(ec);

        //do something with the bytes read
        std::cout.write(buf.data(), nbytes);
        *onReceive(buf.data(), nbytes);
    }
    //destroy thread
}

void handle_error(boost::system::error_code ec) {}
