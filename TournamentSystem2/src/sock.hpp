#pragma once

#include <iostream>
#include <iostream>
#include <string>
#include <winsock2.h>

/*
namespace asio = boost::asio;
using asio::ip::udp;
using asio::ip::tcp;

class Udp_server
{
	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	asio::streambuf receive_buff_;

public:
	Udp_server(asio::io_service& io_service, int port)
		: socket_(io_service, udp::endpoint(asio::ip::address::from_string("127.0.0.1"), port)) {
		start_receive();
	}

private:
	void start_receive() {
		socket_.async_receive_from(
			receive_buff_.prepare(65536), remote_endpoint_,
			bind(&Udp_server::on_receive, this,
				asio::placeholders::error, _2));
	}

	void send(std::string data) {
		try {
			socket_.send_to(asio::buffer(data), remote_endpoint_);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	void on_receive(const boost::system::error_code& error, size_t len) {
		if (!error || error == asio::error::message_size) {
			std::string message = asio::buffer_cast<const char *>(receive_buff_.data());

			std::cout << remote_endpoint_.address() << ":" << remote_endpoint_.port() << std::endl;
			std::cout << message << std::endl;

			receive_buff_.consume(receive_buff_.size());
			// socket_.send_to(receive_buff_, remote_endpoint_);

			start_receive();
			send("Tournament");
		}
	}

};*/
class Udp_server {

	WSADATA wsaData;
	SOCKET sock;
	struct sockaddr_in addr;
	char buf[20];
	u_long val;
public:
	enum ErrorCode {
		MADA,
		KITA,
		ERR
	};
	Udp_server(int port) {

		WSAStartup(MAKEWORD(2, 0), &wsaData);
		sock = socket(AF_INET, SOCK_DGRAM, 0);

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.S_un.S_addr = INADDR_ANY;

		bind(sock, (struct sockaddr *)&addr, sizeof(addr));

		val = 1;
		ioctlsocket(sock, FIONBIO, &val);
		memset(buf, 0, sizeof(buf));
		
	}
	~Udp_server() {
		closesocket(sock);
		WSACleanup();
	}

	char *readsome(ErrorCode *ec) {

		int n = recv(sock, buf, sizeof(buf), 0);
		if (n < 1) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				// まだ来ない。
				cout << "MADA KONAI" << endl;
				*ec = ErrorCode::MADA;
			} else {
				cout << "Some Error occured" << endl;
				printf("Code: 0x%x\n", WSAGetLastError());
				*ec = ErrorCode::ERR;
			}
		} else {
			*ec = ErrorCode::KITA;
			printf("[udp_server::received]", buf);
		}
		return buf;
	}
};

class UDPSocket {
	int		m_bufferSize;
	WSAData m_wsaData;
	SOCKET	m_sock;
	struct	sockaddr_in m_addr;
	u_long val;

public:
	UDPSocket(u_short port, int bufferSize = 1024) {
		m_bufferSize = bufferSize;

		// Initialize winsock and create socket
		WSAStartup(MAKEWORD(2, 0), &m_wsaData);
		m_sock = socket(AF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.S_un.S_addr = INADDR_ANY;

		::bind(m_sock, (struct sockaddr *)&addr, sizeof(addr));

		val = 1;
		ioctlsocket(m_sock, FIONBIO, &val);
	}

	~UDPSocket() {
		// close socket & cleanup
		closesocket(m_sock);
		WSACleanup();
	}

	string recv() {
		string result;

		char *buf = new char[m_bufferSize + 1];
		memset(buf, 0, m_bufferSize + 1);

		struct sockaddr_in server_addr;
		int sockaddr_in_size = sizeof(struct sockaddr_in);

		::recvfrom(m_sock, buf, m_bufferSize, 0, (struct sockaddr *)&server_addr, &sockaddr_in_size);

		result = string(buf);
		delete buf;

		const char *send_buff = "Tournament System";
		sendto(m_sock, send_buff, strlen(send_buff), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));

		return result;
	}
};
class TCPClientSync {
	TCPClient client;
	unsigned char ip1, ip2, ip3, ip4;
	unsigned short port;
public:
	TCPClientSync(unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4, unsigned short port) : ip1(ip1), ip2(ip2), ip3(ip3), ip4(ip4), port(port) {
	}
	TCPClientSync(string ip, unsigned short port) : port(port) {
		int ip1_, ip2_, ip3_, ip4_;
		sscanf(ip.c_str(), "%d.%d.%d.%d", &ip1_, &ip2_, &ip3_, &ip4_);
		ip1 = ip1_;
		ip2 = ip2_;
		ip3 = ip3_;
		ip4 = ip4_;

	}
	string sendAndReceive(string send_str) {
		client.connect({ ip1,ip2,ip3,ip4 }, port);
		for (;;) {
			if (client.isConnected()) {
				client.send(send_str.c_str(), sizeof(char) * send_str.length());
				break;
			}
		}
		Sleep(500); // 次の!client.read(character)で弾かれないように（応急処置）。
		string ret;
		for (;;) {
			char character;
			if (!client.read(character)) break;
			ret.push_back(character);
		}
		return ret;
	}
};

/*


class Tcp_socket_sync {
	tcp::socket socket_;
	asio::io_service& io_service_;
public:
	Tcp_socket_sync(asio::io_service& io_service)
		: io_service_(io_service),
		socket_(io_service)
	{}
	void connect(std::string ip_string, int port) {
		try {
			socket_.connect(tcp::endpoint(asio::ip::address::from_string(ip_string), port));
		} 
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	std::string get_information(std::string command) {
		std::string ret = "";
		boost::system::error_code error_send;
		asio::write(socket_, asio::buffer(command), error_send);

		if (!error_send) {

			boost::system::error_code error_recv;
			asio::streambuf receive_buff_;
			asio::read(socket_, receive_buff_.prepare(65536), asio::transfer_all(), error_recv);
			if (error_recv && error_recv != asio::error::eof) {
				std::cout << "receive failed: " << error_recv.message() << std::endl;
			} else {
				cout << &receive_buff_ << endl;
			}
		}
		return ret;
	}

};

class tcp_socket {
	tcp::socket socket_;
	asio::io_service& io_service_;
	asio::streambuf receive_buff_;

public:
	tcp_socket(asio::io_service& io_service)
		: io_service_(io_service),
		socket_(io_service)
	{}

	void connect(std::string ip_string, int port)
	{
		socket_.async_connect(
			tcp::endpoint(asio::ip::address::from_string(ip_string), port),
			bind(&tcp_socket::on_connect, this, asio::placeholders::error));
	}

	void on_connect(const boost::system::error_code& error)
	{
		if (error) {
			std::cout << "connect failed : " << error.message() << std::endl;
		}
		else {
			std::cout << "connected" << std::endl;
		}
	}


	// メッセージ送信
	void start_send(std::string send_data_)
	{
		asio::async_write(
			socket_,
			asio::buffer(send_data_),
			bind(&tcp_socket::on_send, this,
				asio::placeholders::error,
				asio::placeholders::bytes_transferred));
	}

	// メッセージ受信
	void start_receive()
	{
		asio::async_read(
			socket_,
			receive_buff_,
			asio::transfer_all(),
			bind(&tcp_socket::on_receive, this,
				asio::placeholders::error, asio::placeholders::bytes_transferred));
	}

	// 送信完了
	// error : エラー情報
	// bytes_transferred : 送信したバイト数
	void on_send(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (error && error != asio::error::eof) {
			std::cout << "send failed: " << error.message() << std::endl;
		}
		else {
			std::cout << "send correct!" << std::endl;
		}
	}

	// 受信完了
	// error : エラー情報
	// bytes_transferred : 受信したバイト数
	void on_receive(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (error && error != asio::error::eof) {
			std::cout << "receive failed: " << error.message() << std::endl;
		}
		else {
			std::string data = asio::buffer_cast<const char*>(receive_buff_.data());
			std::cout << data << std::endl;

			receive_buff_.consume(receive_buff_.size());

		}
	}

};
*/