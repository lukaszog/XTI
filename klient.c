#include "headers.h"

int main(int argc, char *argv[])
{
	struct sockaddr_in server;
	struct hostent     *hoste;
	struct in_addr     *inaddr;
	struct t_call      *tCall;
	in_addr_t adress_t;
	char bufor[1024];
	int sock,ile,flags,addrLength,port,check;

	if (argc != 3)
	{
		printf("Uzycie: %s [adres] [port]\n", argv[0]);
		exit(0);
	}
	port = atoi(argv[2]);


	if ((sock = t_open("/dev/tcp", O_RDWR, (struct t_info *) NULL)) < 0)
	{
		t_error("t_open");
		exit(1);
	}

	if (t_bind(sock, (struct t_bind *) NULL, (struct t_bind *) NULL) < 0)
	{
		t_error("t_bind");
		exit(1);
	}


	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(port);

	addrLength = sizeof(server);


	if (NULL == (tCall = (struct t_call *) t_alloc(sock, T_CALL, T_ADDR)))
	{
		t_error("t_call");
		exit(1);
	}


	tCall->addr.maxlen = addrLength;           /* max. rozmiar bufora */
	tCall->addr.len    = addrLength;           /* aktualny rozmiar bufora */
	tCall->addr.buf    = (char *) &server; /* bufor */
	tCall->opt.len     = 0;                    /* bez opcji */
	tCall->udata.len   = 0;                    /* bez danych uzytkownika */


	if (0 > t_connect(sock, tCall, (struct t_call *) NULL))
	{
		t_error("t_connect");
		exit(1);
	}

	bufor[0] = 0;
	while (strncasecmp(bufor, "quit", 4) != 0)
	{
		scanf("%s",bufor);
		bufor[1024]=0;

		if (0 > t_snd(sock, bufor, sizeof(bufor), 0))
		{
			t_error("t_snd");
			exit(1);
		}


		flags = 0;
		if (0 > (ile = t_rcv(sock, &check, sizeof(int), &flags)))
		{
			t_error("t_rcv");
			exit(1);
		}

		bufor[ile] = 0;

		if(check==1)
		{
			printf("To jest palindrom\n");
		}
		else
		{
			printf("To nie jest palindrom\n");
		}
	}

	t_close(sock);
	return 0;
}
