#include "headers.h"


int main (int argc, char *argv[])
{
	struct sockaddr_in server, *clientA;
	struct hostent     *hostE;
	struct t_bind      tBind;
	struct t_call      *tCall;

	char bufor[1024];
	int sock, sd, flags, addrLength, nmb, port;
	pid_t child;

	if (argc != 2)
	{
		printf("Uzycie: %s [port]\n", argv[0]);
		exit(0);
	}
	port = atoi(argv[1]);
	if (0 > (sock = t_open("/dev/tcp", O_RDWR, (struct t_info *) NULL)))
	{
		t_error("t_open");
		exit(1);
	}
	memset((void*) &server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	addrLength = sizeof(server);

	tBind.addr.maxlen = addrLength;           /* max. wielkosc bufora */
	tBind.addr.len    = addrLength;           /* aktualna wielkosc bufora */
	tBind.addr.buf    = (char *) &server; /* w buforze nasza struktura */
	tBind.qlen        = 5;                    /* wielkosc kolejki polaczen */

	if (t_bind(sock, &tBind, (struct t_bind *) NULL) < 0)
	{
		t_error("t_bind");
		exit(1);
	}


	if (NULL == (tCall = (struct t_call *) t_alloc(sock, T_CALL, T_ADDR)))
	{
		t_error("t_alloc");
		exit(1);
	}


	for(;;)
	{
		printf("[] czekam\n");

		if (0 > (t_listen(sock, tCall)))
		{
			t_error("t_listen");
			exit(1);
		}
		if (NULL == (clientA = (struct sockaddr_in *) tCall->addr.buf))
		{
			perror("clientA");
		}
		printf("\t%s (%s:%i)\n", hostE->h_name,inet_ntoa(clientA->sin_addr), ntohs(clientA->sin_port));

		if (0 > (sd = t_open("/dev/tcp", O_RDWR, (struct t_info *) NULL)))
		{
			t_error("t_open");
			exit(1);
		}

		if (0 > t_bind(sd, (struct t_bind *) NULL, (struct t_bind *) NULL))
		{
			t_error("t_bind");
			exit(1);
		}
		if (0 > t_accept(sock, sd, tCall))
		{
			t_error("t_accept");
			exit(1);
		}

		if (0 > (child = fork())) exit(1);

		switch(child)
		{
			case 0:

				t_close(sock);

				printf("[%i] polaczenie\n", (int) getpid());

				while(strncasecmp(bufor, "exit", 4) != 0)
				{
					flags = 0;
					if (0 > (nmb = t_rcv(sd, bufor, sizeof(bufor), &flags)))
					{
						perror("read");
						exit(1);
					}
					if (nmb != 0)
						printf("[%i] odczytalem:\n%s", (int) getpid(), bufor);
					int check,left,right;
					if(strlen(bufor)==0)
					{
						check=1;
					}
					else
					{
						left=0;
						right=strlen(bufor)-1;
						check=1;

						while(left<right&&check)
						{
							if(bufor[left]!=bufor[right])
							{
								check=0;
							}
							else
							{
								left++;
								right--;
							}
						}
					}

					if (t_snd(sd, (void*)&check, sizeof(int), 0) < 0)
					{
						perror("write");
						exit(1);
					}
				}
				t_close(sd);
				printf("[%i] polaczenie zakonczone.\n", (int) getpid());
				exit(0);
			default:
				t_close(sd);
		}
	}
}
