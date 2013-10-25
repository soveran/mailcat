/*

Copyright (c) 2013 Michel Martens

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define MT_MAXREAD_SIZE 4096
#define MT_DEFAULT_PORT 25

struct {

	/* Server and client addresses. */
	struct sockaddr_in sa, ca;

	/* Socket and connection descriptors. */
	int sd, cd;
} mt;

/* Socket reading buffer. */
char buff[MT_MAXREAD_SIZE];

/* Verify the port is in the range 0-65535. */
void assert_port(int port) {
	if (port < 0 || port > 65535) {
		fprintf(stderr, "Invalid port: %d\n", port);
		exit(EXIT_FAILURE);
	}
}

/* Verify the file descriptor is valid. */
void assert_descriptor(int desc) {
	if (desc < 0) {
		fprintf(stderr, "Error: %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

/* Write <str> to socket <mt.cd>. */
void send_chunk(char *str) {
	send(mt.cd, str, strlen(str), 0);
}

/* Read from socket <mt.cd>. */
ssize_t recv_chunk() {

	/* Number of bytes read from socket. */
	ssize_t br = recv(mt.cd, &buff, sizeof(buff)-1, 0);

	if (br <= 0) {
		fprintf(stderr, "Error: %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Terminate the string. */
	buff[br] = '\0';

	return br;
}

/* Read from <fd> until <ending> is found, and display to stdout. */
void display_upto(int fd, const char *ending) {
	char ch;

	int curr = 0;
	int last = strlen(ending) - 1;

	recv(fd, &ch, 1, 0);

	while (ch) {
		if (ch == ending[curr]) {
			if (curr == last) {
				return;
			} else {
				curr++;
			}
		} else {
			if (curr == 0) {
				printf("%c", ch);
			} else {
				printf("%.*s", curr, ending);
				curr = 0;
				continue;
			}
		}

		recv(fd, &ch, 1, 0);
	}
}

int main(int argc, char **argv) {

	/* Server port number. */
	int port;

	/* Socket length. */
	socklen_t sl;

	/* Number of bytes read from socket. */
	ssize_t br;

	if (argv[1] == NULL) {
		port = MT_DEFAULT_PORT;
	} else {
		port = atoi(argv[1]);
		assert_port(port);
	}

	/* Socket descriptor. */
	mt.sd = socket(AF_INET, SOCK_STREAM, 0);
	assert_descriptor(mt.sd);

	/* Fill server address struct with zeroes. */
	memset((char *)&mt.sa, 0, sizeof(mt.sa));

	/* Configure server address. */
	mt.sa.sin_family = AF_INET;
	mt.sa.sin_addr.s_addr = INADDR_ANY;
	mt.sa.sin_port = htons(port);

	/* Try to bind server. */
	if (bind(mt.sd, (struct sockaddr *)&mt.sa, sizeof(mt.sa)) < 0) {
		fprintf(stderr, "Couldn't bind to port %d.\n", port);
		exit(EXIT_FAILURE);
	} else {
		printf("Listening on port %d\n", port);
	}

	/* Listen with a limit of 10 waiting connections. */
	listen(mt.sd, 10);

	sl = sizeof(mt.ca);

	while (1) {
		mt.cd = accept(mt.sd, (struct sockaddr *)&mt.ca, &sl);
		assert_descriptor(mt.cd);

		/* Service ready. */
		send_chunk("220\r\n");
		br = recv_chunk();

		/* Skip everything upto DATA. */
		while (strncmp(buff, "DATA\r\n", br) != 0) {

			/* Requested mail action okay, completed. */
			send_chunk("250\r\n");
			br = recv_chunk();
		}

		/* Start mail input; end with `<CRLF>.<CRLF>`. */
		send_chunk("354\r\n");

		printf("---\n");

		/* Display the mail body. */
		display_upto(mt.cd, "\r\n.\r\n");

		printf("\n");

		/* Requested mail action okay, completed. */
		send_chunk("250\r\n");
		recv_chunk();

		/* Service closing transmission channel. */
		send_chunk("221\r\n");

		/* Close file descriptor. */
		close(mt.cd);
	}
}
