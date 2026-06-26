#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>

#define SERVER_PORT 5000
#define SERVER_IP4 "192.168.10.130"
#define LISTEN_BACKLOG 50
#define BUFF_SIZE 255

int* arr;

struct Range {
	int num;
	int start;
	int end;
	Range(const int& n, const int& s, const int& e)
		: num(n), start(s), end(e) {}
};

void * fillTable(void *arg) {
	Range *rng = (Range* )arg;
	printf("start %d, end %d\n", rng->start, rng->end);
	for (int i = rng->start; i <= rng->end; ++i) {
		arr[i] = rng->num;
	}
	delete rng;
	return NULL;
}

int main()
{
	int sd = socket(AF_INET,SOCK_STREAM,0);
	if (sd == -1) {
		char buff[BUFF_SIZE] = "cannot create socket";
		write(2, buff, strlen(buff));
	        exit(1);	
	}

	int optval = 1;
	int rv = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    	if (rv == -1) {
        	char buff[BUFF_SIZE] = "setsockopt failed\n";
        	write(2, buff, strlen(buff));
		exit(10);
    	}

	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP4);
	int b = bind(sd, (const struct sockaddr*) &addr, sizeof(addr));
	if (b == -1) {
		char buff[BUFF_SIZE] = "cannot bind a name to a socket\n";
		write(2, buff, strlen(buff));
		exit(2);
	}

	int l = listen(sd, LISTEN_BACKLOG);
	if (l == -1) {
		char buff[BUFF_SIZE] = "listen failed\n";
		write(2, buff, strlen(buff));
		exit(3);
	}

	socklen_t addrlen = sizeof(addr);
	int ad = accept(sd, (struct sockaddr *) &addr, &addrlen);
	if (ad == -1) {
		char buff[BUFF_SIZE] = "accept failed\n";
		write(2, buff, strlen(buff));
		exit(4);
	}

	int numbers[3];
	read(ad, numbers, sizeof(numbers));
	std::cout << numbers[0] << ' ' << numbers[1] << ' ' << numbers[2] << '\n'; 
	int numT = numbers[0];
	int numR = numbers[1];
	int numC = numbers[2];

	int size = numR * numC;
	int d = size / numT;
	int r = size % numT;
	arr = new int[size];
	pthread_t* threads = new pthread_t[numT];

	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < numT; ++i) {
		Range* rng = new Range(i, i*d, (i+1)*d - 1);
		pthread_create(&threads[i], NULL, fillTable, rng);
	}
	for (int i = 0; i < numT; ++i) {
		pthread_join(threads[i], NULL);
	}
	delete[] threads;
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> diff = end - start;
	std::cout << "time: " << diff.count() << '\n';

	int lastStart = d*numT;
	for (int i = lastStart; i < lastStart + r; ++i) {
		arr[i] = -1;
	}
	write(ad, arr, 4 * size);
	delete[] arr;
	exit(0);
}
