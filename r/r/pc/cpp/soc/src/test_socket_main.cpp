#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/asio.hpp>

#define PORT_NO 12345

using namespace std;
using namespace boost::asio;

std::vector<struct udp_packet> buf;

//enum field { ""...}

struct udp_packet {
    char * field;
    unsigned int data;
};

void assertfail(bool cond, const char * msg) {
    if (!cond) {
        printf("%s\n", msg);
        exit(EXIT_FAILURE);
    }
}

// invoked AFTER the service request completes
void snd_handler (const boost::system::error_code& ec, std::size_t bytes_to_send) { }

struct rcv_handler_struct {
    struct udp_packet p;
    //ip::udp::socket sock;
    void operator() (const boost::system::error_code& ec, std::size_t bytes_received) {
        if (!ec) {
            // return an ack to the client
            //boost::shared_ptr<std::string> msg("1");
            //sock.async_send(buffer(*msg), snd_handler);
            // do stuff
            printf("received %d bytes from remote client.\n", (int)bytes_received);
            p=buf[0];
            // ready to receive again
            //sock.async_receive(buffer(buf), this);
        }
    }
};

//void listen(ip::udp::socket sock, struct rcv_handler_struct rcvr) {
//    sock.async_receive(buffer(buf), rcvr);
//}

unsigned short bind(ip::udp::socket& sock, ip::udp::endpoint endpoint) {
    boost::system::error_code ec;
    sock.open(endpoint.protocol(), ec);
    assertfail(!ec, ("Error opening socket: " + ec.message() + "\n").c_str());
    printf("successfully opened socket.\n");
    ec.clear();
    unsigned short port = endpoint.port() - 1;
    do {
        sock.bind(ip::udp::endpoint(endpoint.protocol(), ++port), ec);
    } while (ec && port > 0);
    assertfail(!ec, ("Error binding socket to local endpoint: " + ec.message() + "\n").c_str());
    printf("successfully bound socket to local endpoint at port %d.\n", (int)port);
    return port;
}

int main(int argc, char * argv[]) {
    boost::system::error_code ec;
    ip::udp::endpoint local_endpoint(ip::udp::v4(), PORT_NO);
    io_service io;
    try {   
        //create a udp socket and bind it to the local endpoint
        ip::udp::socket sock(io, local_endpoint); 
        //ip::udp::socket sock(io); 
        //unsigned int port = bind(sock, local_endpoint);
        
        //set socket options 
        boost::asio::socket_base::reuse_address option(true);
        sock.set_option(option, ec);
        assertfail(!ec, ("Error setting option reuse_address: " + ec.message() + "\n").c_str());
        ec.clear();
        socket_base::keep_alive option2(true);
        sock.set_option(option2, ec);
        assertfail(!ec, ("Error setting option keep_alive: " + ec.message() + "\n").c_str());
        ec.clear();

        //start listening
        struct rcv_handler_struct rcvr;
        struct udp_packet p;
        //rcvr.sock = sock;
        buf.push_back(p);
        sock.async_receive(buffer(buf), rcvr);
        sock.get_io_service().run();
        
        sock.close(ec);
        assertfail(!ec, ("Error closing socket: " + ec.message() + "\n").c_str());
        printf("successfully closed socket. Exiting application.\n");
    } catch (boost::system::system_error e) {
        assertfail(false, ("boost system_error: " + string(e.what()) + "\n").c_str());
    }
    return 0;
}
