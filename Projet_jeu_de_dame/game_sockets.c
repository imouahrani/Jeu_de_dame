#include "game_engine.h"
#include "game_login.h"
#include "game_sockets.h"

void fonc_perror(const char *msg)
{
	perror(msg);
	exit(-1);
}

// returns a connected socket or fails and exits
int socket_connect(char *host, char *port)
{
	int csock;
	int status;
	
	struct addrinfo *res, hints;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	status = getaddrinfo(host, port, &hints, &res);
	if( status != 0 )
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		exit(-1);
	}
	
	csock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if( csock == -1 )
	{
		fonc_perror("socket");
	}
	
	// connect to host
	status = connect(csock, res->ai_addr, res->ai_addrlen);
	if( status == -1 )
	{
		fonc_perror("connect");
	}
	freeaddrinfo(res);
	
	return csock;
}

// sends data or fails
int socket_send(int csock, void *buffer, int len)
{
	int bytes_sent = send(csock, buffer, len, 0);
	if( bytes_sent == -1 )
	{
		perror("send");
	}
	
	return bytes_sent;
}

// receives data or fails
int socket_recv(int csock, void *buffer, int len)
{
	int bytes_recvd = recv(csock, buffer, len, 0);
	if( bytes_recvd < -1 )
	{
		perror("recv");
	}
	
	return bytes_recvd;
}

int recv_client(int csock, char *buffer)
{
	//memset(buffer, 0, strlen(buffer));
	int r = recv(csock, buffer, BUFFSIZE, 0);
	if( r < 0)
	{
		perror("recv()");
		exit(-1);
	}
	
	return r;
}

int send_client(int csock, char *buffer)
{

	int a = send(csock, buffer, strlen(buffer), 0);
	
	if( a < 0 )
	{
		perror("send()");
		exit(-1);
	}
	
	//memset(buffer, 0, BUFFSIZE); le memset ne marche pas ici 
	return a;
}

// a modifier
int listen_socket(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0); 
	if( sock == SOCKET_ERROR ) 
	{
		perror("socket()");
		exit(-1);
	}
		
	struct sockaddr_in sin;
	sin.sin_addr.s_addr = htonl(INADDR_ANY); 
	sin.sin_port = htons(port);
	sin.sin_family = AF_INET;
	
	int a = bind (sock, (struct sockaddr*) &sin, sizeof(sin)); 
	if (a == SOCKET_ERROR)
	{
		perror("bind()");
		exit(-1);
	}
	
	
	int b = listen(sock, 1); 
	if ( b == SOCKET_ERROR )
	{
		perror("listen()");
		exit(-1);
	}

	return sock;
}

struct Client add_client(int ssock, int* nb_c, int* max_fd, FILE * flux)
{

	struct sockaddr_in csin; int csock; unsigned int csinsize = sizeof(csin);
	csock = accept(ssock, (struct sockaddr *)&csin, &csinsize); 
	//printf("csock : %d\n", csock);
	

	struct Client ret_client;
	ret_client.csock = csock;

	if(*max_fd < ret_client.csock)
	{
		*max_fd = ret_client.csock;
	}
	
	char buffer[BUFFSIZE] = {0};

	ret_client = definirModeConnexion( nb_c, buffer, ret_client, flux);
	ret_client.en_attente = 0;
	
	// envoyer sa couleur
	//if( ret_client.csock > -1 && ret_client.mode == 'J' ) // que si on est en mode Joueur on lui envoie la couleur
		//envoyerCouleur( ret_client, (*nb_c-1) ); // -1 parce que le nb_c est deja augmenter dans ce cas 

	
	return ret_client;
}

void rmv_client(struct Client clients[MAX_CLIENT], int i_to_remove, int* nb_c)
{
	
	 int i;	 
	 printf("avant : %d\n", *nb_c);
	 
	 close(clients[i_to_remove].csock);
	 
	 for(i = i_to_remove; i < *nb_c-1 ; i++)
	 {
			 clients[i] = clients[i+1];
	 } 
	 
	 (*nb_c)--;
	  printf("apres: %d\n", *nb_c);
}

void socket_close(int csock)
{
	close(csock);
	
	return;
}

