/* TCP_Connection.cpp */

#include "TCP_Connection.h"

TCP_Connection::TCP_Connection(boost::asio::io_service& io)
    : sock(io) {}

static pointer TCP_Connection::create(boost::asio::io_service& io) {
    return pointer(new TCP_Connection(io));
}

tcp::socket& TCP_Connection::getSocket() {
    return sock;
}

void TCP_Connection start() {
    msg = createReturnString();
    boost::asio::async_write(sock, boost::asio::buffer(msg), boost::bind(&TCP_Connection::write_handler, 
        shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void write_handler(const boost::system::error_code ec, size_t bytes_transferred) {
    
}

std::string TCP_Connection::createReturnString() {
    time_t now = time(0);
    return ctime(&now); //daytime protocol
}

