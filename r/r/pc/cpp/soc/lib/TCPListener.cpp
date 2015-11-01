/* TCPListener.cpp */

#include <ctime>
#include <iostream>

#include "TCPListener.h"

using namespace std;
using boost::asio::ip::tcp;

TCPListener::TCPListener() 
    : io(), listener(io, tcp::endpoint(tcp::v4(), TCP_PORTNO) {
    TCPListener::begin();
}

void TCPListener::begin() {
    TCP_Connection::pointer conn = TCP_Connection::create(io);
    listener.async_accept(conn->getSocket(), boost::bind(&TCPListener::rcv_handler, this, conn,
                                                      boost::asio::placeholders::err));
}

void TCPListener::rcv_handler(TCP_Connection::pointer conn, const boost::system::error_code& ec) {
    if (!ec) {
        conn->start();
    }
    TCPListener::begin();
}
