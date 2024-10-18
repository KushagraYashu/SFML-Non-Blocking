#include <stdio.h>
#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Network/IpAddress.hpp>

int main() {
	printf("Client Program\n");

	sf::TcpSocket socket; //client socket
	
	sf::SocketSelector selector;

	const std::size_t maxMsgSize = 100; //max msg size
	sf::IpAddress ipAddr = sf::IpAddress("127.0.0.1"); //ip address of the server
	int port = 5555; //port of the server

	//connect to a server
	sf::Socket::Status status = socket.connect(ipAddr, port); //storing status for error handling
	if (status != sf::Socket::Done) {
		//error...
		printf("server not found, bitches be sleeping\n");
	}
	else {
		printf("Connected to server at port %d and address %s\n", socket.getRemotePort(), socket.getRemoteAddress().toString().data());

		sf::Packet iniPacket;
		socket.receive(iniPacket);
		std::string iniStr;
		iniPacket >> iniStr;
		if (strcmp(iniStr.c_str(), "Sorry, I cant handle more") == 0) {
			printf("The server is at capacity, they said: %s,\n you should try again later\n", iniStr.c_str());
		}
		else {
			while (true) {

				sf::Packet dataPacket;

				std::string line; //a string that will read the input
				printf("enter your message (type \"quit\" to disconnect): ");
				fflush(stdout); //flush out the contents of the console, so we get fresh content
				std::getline(std::cin, line); //inputting the line

				dataPacket << line;

				sf::Socket::Status sendStat = socket.send(dataPacket); //taking line size - 1 because we dont want to send \0, that creates problems
				if (sendStat == sf::Socket::Disconnected) {
					//error...
					printf("server doesn't want to talk to you anymore, they have a \"fling\" going on\n");
					break;
				}
				if (sendStat != sf::Socket::Done) {
					//error...
					printf("error in sending message, you are high as shit bro");
					break;
				}

				//reading the data from the server
				if (strcmp(line.c_str(), "quit") == 0) {
					printf("you said \"eww, go away\"\n");
					break;
				}
				else {
					sf::Socket::Status recvStat = socket.receive(dataPacket);
					if (recvStat == sf::Socket::Disconnected) {
						//error...
						printf("server didn't respond, youre talking to yourself at this point\n");
						break;
					}
					if (recvStat != sf::Socket::Done) {
						//error...
						printf("server said something dumb and i cant interpret it, sorry\n");
						break;
					}

					dataPacket >> line;
					printf("they said: %s\n", line.c_str());
				}
			}
		}
		
		socket.disconnect(); //disconnecting the socket
	}

	return 0;
}