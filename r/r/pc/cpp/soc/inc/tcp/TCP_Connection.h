/* TCP_Connection.h */

#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <boost/asio.hpp>
#include <string>

class TCP_Connection : public boost::enable_shared_from_this<TCP_Connection> {
    public: 
       typedef boost::shared_ptr<TCP_Connection> pointer;
       static pointer create(boost::asio::io_service& io);
       tcp::socket& getSocket();
       void start();
       std::string createReturnString();

    private:
        TCP_Connection(boost::asio::io_service& io);
        void write_handler(const boost::system::error_code& ec, size_t bytes_transferred);
        tcp::socket sock;
        std::string msg;
};

#endif
