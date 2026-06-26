#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#define SERVER_IP "192.168.10.130"
#define SERVER_PORT 5000
#define BUFF_SIZE 255

int main(int argc, char* argv[])
{
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		char buff[BUFF_SIZE] = "cannot create socket\n";
		write(2, buff, strlen(buff));
		exit(1);
	}


	struct sockaddr_in addr;
        addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	socklen_t addrlen = sizeof(addr);
	int cd = connect(sd, (const struct sockaddr *) &addr, sizeof(addr));
	if (cd == -1) {
		char buff[BUFF_SIZE] = "accept failed\n";
		write(2, buff, strlen(buff));
		exit(2);
	}

	char buff[BUFF_SIZE];
	int rb;
	int numbers[3];
	std::cin >> numbers[0] >> numbers[1] >> numbers[2];
	write(sd, numbers, sizeof(numbers));

	int numT = numbers[0];
	int numR = numbers[1];
	int numC = numbers[2];
	int size = numR * numC;
	int* arr = new int[size];
	read(sd, arr, 4 * size);
	for (int i = 0; i < size; ++i) {
		std::cout << arr[i];
		if ((i + 1) % numC == 0) {
			std::cout << '\n';
		}
		else {
			std::cout << ' ';
		}
	}
	delete[] arr;
	exit(0);
}
