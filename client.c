/* timeclnt.c - a client for timeserv.c
 *              usage: timeclnt hostname portnumber
 */
#include       <stdio.h>
#include       <sys/types.h>
#include       <sys/socket.h>
#include       <netinet/in.h>
#include       <netdb.h>
#include 	<curses.h> // required
#include	<unistd.h>
#include	<sys/types.h>
#include	<string.h>
#include	<sys/wait.h>
#include 	<signal.h>
 
#define        oops(msg)       { perror(msg); exit(1); }
void a(int s);
typedef struct R {
	int player1_score;
	int player1;
	int player2_score;
	int player2;
	int x_ball;
	int y_ball;
	int you_are;
	char test;
}R;
main(int ac, char *av[])
{
	int player_num;
	struct sockaddr_in  servadd;        /* the number to call */
	struct hostent      *hp;            /* used to get number */
	int    sock_id, sock_fd;            /* the socket and fd  */
	char   message[BUFSIZ];             /* to receive message */
	int    messlen;                     /* for message length */
	pid_t p1, p2;
	
	int pipe1_fd[2], pipe2_fd[2];
	if (pipe(pipe1_fd)==-1)
    {
        fprintf(stderr, "Pipe1 Failed" );
        return 1;
    }
	if (pipe(pipe2_fd)==-1)
    {
        fprintf(stderr, "Pipe2 Failed" );
        return 1;
    }
	int i,j;
	char clientid[4];
	//make some random ID for this client
	srand (time(NULL));
	sprintf(clientid,"%d", rand()%1000);
	
     /*
      * Step 1: Get a socket
      */
	sock_id = socket( AF_INET, SOCK_STREAM, 0 );    /* get a line   */
	if ( sock_id == -1 ) 
		oops( "socket" );          		/* or fail      */

     /*
      * Step 2: connect to server
      *         need to build address (host,port) of server  first
      */

	bzero( &servadd, sizeof( servadd ) );   /* zero the address     */
    
	hp = gethostbyname( av[1] );            /* lookup host's ip #   */
	if (hp == NULL) 
		oops(av[1]);            	/* or die               */
	bcopy(hp->h_addr, (struct sockaddr *)&servadd.sin_addr, hp->h_length);

	servadd.sin_port = htons(atoi(av[2]));  /* fill in port number  */

	servadd.sin_family = AF_INET ;          /* fill in socket type  */

						       /* now dial     */
	if ( connect(sock_id,(struct sockaddr *)&servadd, sizeof(servadd)) !=0)
	       oops( "connect" );

     /*
      * Step 3: transfer data from server, then hangup
      */

	messlen = read(sock_id, message, BUFSIZ);     /* read stuff   */
	if ( messlen == - 1 )
	       oops("read") ;
	player_num = atoi(message);
	WINDOW *wnd;
	wnd = initscr();
	cbreak(); // curses call to set no waiting for Enter key
	noecho(); // curses call to set no echoing
	char c[2];
	char* ptr = &c;
	c[1] = '\0';
	j=0;
	p1 = fork();
	if(p1>0){ //fork three times, one for client read and send to server(which also pipes that too thrind fork), second fork recieve from server and pipe that to thrid fork, thrid fork builds game
			while(1){ //get user input, send to client drawer, and server
				c[0] = getch();
				if(c[0]=='q'){
					printf("done\n");
					break;
				}
				if ( write( sock_id, ptr, messlen ) != messlen )  /* and write to */
					oops( "write" );
			}			
	}
	else{ //recieve input from server and send to client drawer
		struct R* r = (R*)malloc(sizeof(R));
		int temp;
		int x=16,y=80;
		while(1) {
			clear();
			messlen = read(sock_id, (R*)r, sizeof(R));     /* read stuff   */
			if ( messlen == - 1 ){
				printf("program closed\n");
			}
			for(i=0;i<=x;i++){
				move(i,0);
				insch('*');
				move(i,y);
				insch('*');
			}
			for(i=0;i<y;i++){
				move(0,i);
				delch();
				insch('*');
				move(x,i);
				delch();
				insch('*');
			}
			//player1
			move(r->player1-1,4);
			delch();
			insch('*');
			move(r->player1-2,4);
			delch();
			insch('*');
			move(r->player1,4);
			delch();
			insch('*');
			move(r->player1+1,4);
			delch();
			insch('*');
			move(r->player1+2,4);
			delch();
			insch('*');
			
			//player2
			move(r->player2-1,y-4);
			delch();
			insch('*');
			move(r->player2-2,y-4);
			delch();
			insch('*');
			move(r->player2,y-4);
			delch();
			insch('*');
			move(r->player2+1,y-4);
			delch();
			insch('*');
			move(r->player2+2,y-4);
			delch();
			insch('*');
			
			move(18,35);
			delch();
			printw("%d", r->player1_score);
			
			move(18,45);
			delch();
			printw("%d", r->player2_score);
			
			//ball
			move(r->x_ball,r->y_ball);
			delch();
			insch('*');
			
			move(0,0);
			//build
			refresh();
		}
	}	
	endwin();
	close(sock_id);
	system("reset");
}
void a(int s){
    raise(SIGINT); //goes to f function
}