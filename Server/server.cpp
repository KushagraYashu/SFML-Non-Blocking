#include <stdio.h>
#include<iostream>
#include <list>
#include <SFML/Network.hpp>
#include <SFML/Network/IpAddress.hpp>

int main() {
	printf("Echo Server\n");

	int port = 5555; //port number

	sf::IpAddress ipAddr = sf::IpAddress("127.0.0.1"); //ipaddress

	sf::TcpListener listener; //listener for the socket

	std::list<sf::TcpSocket*> clients;

	sf::SocketSelector selector;

	// bind the listener to a port
	if (listener.listen(port, ipAddr) != sf::Socket::Done)
	{
		// error...
	}
	else {
		std::string addr = ipAddr.toString();
		printf("listening at port %d and address %s\n", port, addr.data());
	}

	selector.add(listener);

	int limit = 0;
	printf("Enter the max client you want to talk to\n");
	std::cin >> limit;

	printf("waiting for a connection\n");

	while (true) {
		if (selector.wait()) {

			if (selector.isReady(listener)) {
				sf::TcpSocket* client = new	sf::TcpSocket;
				client->setBlocking(false);
				if (listener.accept(*client) == sf::Socket::Done) {
					if (clients.size() < limit) {
						printf("A client connected at port %d and address %s\n", client->getRemotePort(), client->getRemoteAddress().toString().data());
						clients.push_back(client); //add the client to the client list
						printf("lets name them %d\n", client->getRemotePort());
						std::cout << "Total clients connected " << clients.size() << std::endl;
						selector.add(*client);
						sf::Packet packet;
						std::string emptyMsg = " ";
						packet << emptyMsg;
						client->send(packet);
					}
					else {
						//I couldnt make this non blocking
						//i tried adding to selector and then checking .isReady(), but it doesn't work :(
						printf("I'm on limit, tell em to stop\n");
						sf::Packet limitHitPacket;
						std::string limitHitMsg = "Sorry, I cant handle more";
						limitHitPacket << limitHitMsg;
						client->send(limitHitPacket);
						client->disconnect();
					}
				}
				else {
					delete client;
				}
			}
			else {
				for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it) {
					sf::TcpSocket& client = **it;
					if (selector.isReady(client)) {
						
						sf::Packet dataPacket;
						
						sf::Socket::Status recvStat; //storing status for error conditions
						recvStat = client.receive(dataPacket); 
						if (recvStat == sf::Socket::Disconnected) {
							//error...
							printf("client %d just shut the door on you, you disgusted them out\n", client.getRemotePort());
							clients.remove(&client);
							selector.remove(client);
							client.disconnect(); //is this closing socket properly? cause there is a method "close()" but its protected so inaccessible
							printf("waiting for a connection\n");
							break;
						}

						std::string line;
						dataPacket >> line;

						printf((char*)dataPacket.getData());
						
						//quitting condition
						if (strcmp(line.c_str(), "quit") == 0) {
							printf("Quit entered, client %d said \"eww, go away\"\n", client.getRemotePort());
							clients.remove(&client);
							selector.remove(client);
							client.disconnect(); //is this closing socket properly? cause there is a method "close()" but its protected so inaccessible
							break;
						}
						else {
							printf("the (love letter) message from client %d is: %s\n", client.getRemotePort(), line.c_str());

							if (selector.isReady(client)) //non blocking send, is this it?
							{
								dataPacket << line;
								sf::Socket::Status sendStat = client.send(dataPacket); //sending the msg back
							}
						}
					}
				}
			}
		}
	}
	

	return 0;
}