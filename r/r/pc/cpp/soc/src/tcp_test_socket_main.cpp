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

struct rcv_handler_struct {
    struct udp_packet p;
    void operator() (const boost::system::error_code& ec, std::size_t bytes_received) {
        if (!ec) {
            // do stuff
            printf("received %d bytes from remote client.\n", (int)bytes_received);
            p=buf[0];
        }
    }
};

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
    //ip::tcp::endpoint tcp_local_endpoint(ip::tcp::v4(), PORT_NO); 
    //ip::tcp::endpoint tcp_remote_endpoint;
    io_service io;
    try {   
        //create a tcp welcome socket and open it to listen on the local endpoint
        ip::tcp::acceptor acceptor(io, tcp_local_endpoint);
        
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
        acceptor.set_option(option2, ec);
        assertfail(!ec, ("Error setting option keep_alive: " + ec.message() + "\n").c_str());
        ec.clear();

        //accept connection to remote client
        acceptor.accept(sock, tcp_remote_endpoint, ec);
        assertfail(!ec, ("Error accepting connection: " + ec.message() + "\n").c_str());
        ec.clear();

        //start listening
        struct rcv_handler_struct rcvr;
        struct udp_packet p;
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
