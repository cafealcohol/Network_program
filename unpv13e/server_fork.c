#include "./lib/unp.h"
#include <stdio.h>
#include <stdlib.h>

void Err_sys(const char* X)
{
	perror(X);
	exit(1);
}

void str_echo(int sockfd)
{
	char result[]="hello,world\n";
	write(sockfd,&result,sizeof(result));
//	execl("/hello.html","echo","execute bt execl",NULL);
}

void sig_chld(int signo)
{
	pid_t	pid;
	int	stat;

	pid = wait(&stat);
	printf("child %d terminated\n", pid);
	return;
}

int main(int argc, char **argv)
{
	int			listenfd, connfd;
	pid_t			childpid;
	socklen_t		clilen;
	struct sockaddr_in	cliaddr, servaddr;
	void			sig_chld(int);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	signal(SIGCHLD, sig_chld);	/* must call waitpid() */

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				Err_sys("accept error");
		}

		if ( (childpid = fork()) == 0) {	/* child process */
			close(listenfd);	/* close listening socket */
			str_echo(connfd);	/* process the request */
			exit(0);
		}
		close(connfd);			/* parent closes connected socket */
	}
}
