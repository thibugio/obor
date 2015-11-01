#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "test_utils.h"

#define PORT_NO 12345
#define rcv_buf_len 64
#define ret_buf_len 256

#define debug 0

using boost::asio::ip::udp;

struct udp_packet {
    char field[64] = "hello";
    unsigned int data = 0;
};


class udp_server
{
public:
  udp_server(boost::asio::io_service& io, bool (*gateFnc)(uint8_t bytesIn[], size_t nbytesIn, 
             uint8_t bytesOut[], size_t bytesOutSize, size_t *nbytesOut, bool *overflow))
    : sock(io, udp::endpoint(udp::v4(), PORT_NO)) {
    
    gateFnc = gateFnc;

    assertfail(sock.is_open(), "ERROR: socket is not open.\n");
    start_receive();
  }

private:
  void start_receive()
  {
    boost::asio::socket_base::bytes_readable command(true);
    sock.io_control(command);
    assertfail(command.get() <= rcv_buf_len, "ERROR: receive buffer is too small.\n");
    
    sock.async_receive_from(
        boost::asio::buffer(rcv_buf), remote_endpoint,
        boost::bind(&udp_server::handle_receive, this, 
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive(const boost::system::error_code& error,
      std::size_t bytes_transferred)
  {
    if (!error || error == boost::asio::error::message_size)
    {
      if (debug) printf("received %d bytes from remote client: \t", (int)bytes_transferred); 
      for (int i=0; i<(int)bytes_transferred; i++) { if (debug) printf("%d ",rcv_buf[i]); }
      
      struct udp_packet p;
      packets.push_back(p);
      if (debug) printf("\t(packets rcvd: %d)\n", (int)packets.size());
     
      // do stuff
      size_t bytes_returned;
      bool overflow;
      bool ret = gateFnc(rcv_buf, bytes_transferred, ret_buf, ret_buf_len, &bytes_returned, &overflow);

      // send ack to client
      if (ret) {
          /*if (bytes_returned) {
              sock.async_send_to(boost::asio::buffer(ret_buf, bytes_returned), remote_endpoint,
                boost::bind(&udp_server::handle_send, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
          } else {*/
              boost::shared_ptr<std::string> message(new std::string("1"));
              sock.async_send_to(boost::asio::buffer(*message), remote_endpoint,
                boost::bind(&udp_server::handle_send, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
         // }
      } else {
        boost::shared_ptr<std::string> message(new std::string("0"));
        sock.async_send_to(boost::asio::buffer(*message), remote_endpoint,
          boost::bind(&udp_server::handle_send, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      }

      start_receive();
    }
  }

  void handle_send(const boost::system::error_code& error,
      std::size_t bytes_transferred) {
  }

  bool (*gateFnc)(uint8_t bytesIn[], size_t nbytesIn, 
                  uint8_t bytesOut[], size_t bytesOutSize, size_t *nbytesOut, bool *overflow);
  udp::socket sock;
  udp::endpoint remote_endpoint;
  std::vector<struct udp_packet> packets;
  uint8_t rcv_buf[rcv_buf_len];
  uint8_t ret_buf[ret_buf_len];
};


