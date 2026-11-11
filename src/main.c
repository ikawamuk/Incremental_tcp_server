#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

# define LISTEN_QUEUE_NUMBER	10
# define READ_BUF_SIZE			10
# define WRITE_BUF_SIZE			10

int	xsocket(int domain, int type, int protcol) {
	int	server_fd = socket(domain, type, protcol);
	if (server_fd < 0) {
		perror("socket");
		exit(1);
	}
	return (server_fd);
}

void	bind_local_tcp_server(int server_fd, int port) {
	struct sockaddr_in	serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	serv_addr.sin_port = htons(port);
	if (bind(server_fd, (const struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		close(server_fd);
		exit(1);
	}
}

void	xlisten(int listen_fd, int queue_n) {
	if (listen(listen_fd, queue_n) < 0) {
		perror("listen");
		close(listen_fd);
		exit(1);
	}
}

int	xaccept(int server_fd) {
	int	client_fd = accept(server_fd, NULL, NULL);
	if (client_fd < 0) {
		perror("accept");
		exit(1);
	}
	return (client_fd);
}

int	main(int argc, char *argv[]) {
	if (argc < 2) {
		dprintf(2, "Usage: %s <port number>\n", argv[0]);
		return (1);
	}
	int	server_fd = xsocket(AF_INET, SOCK_STREAM, 0);
	bind_local_tcp_server(server_fd, atoi(argv[1]));
	xlisten(server_fd, LISTEN_QUEUE_NUMBER);
	int		client_fd = xaccept(server_fd);
	char	read_buf[READ_BUF_SIZE];
	while (1) {
		ssize_t	rd_bytes = read(client_fd, read_buf, sizeof(read_buf));
		if (rd_bytes < 0) {
			perror("read");
			close(client_fd);
			close(server_fd);
			exit(1);
		}
		if (rd_bytes == 0)
			break ;
		ssize_t	sent = 0;
		while (sent < rd_bytes) {
			ssize_t	wr_bytes = write(client_fd, read_buf + sent, rd_bytes - sent);
			if (wr_bytes <= 0) {
				perror("write");
				close(client_fd);
				close(server_fd);
				exit(1);
			}
			sent += wr_bytes;
		}
	}
	close(client_fd);
	close(server_fd);
	return (0);
}
