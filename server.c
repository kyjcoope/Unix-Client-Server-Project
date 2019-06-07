//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include	<unistd.h>
#include	<sys/types.h>
#include	<string.h>
#include	<sys/wait.h>
#include <math.h>

#include <signal.h>
    
#define TRUE   1 
#define FALSE  0 
#define PORT 13719 // 1025 - 32000
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
int main(int argc , char *argv[])  
{  

	srand (time(NULL));
	int x_dir = 1, y_dir = 1;
	struct timeval tv;
	tv.tv_sec = 1;
    tv.tv_usec = 0;
	int random = (rand()%360);
	char player_move;
    int opt = TRUE;  
	int flag = 1;
	int start_flag = 1;
	int j,k;
	int num_client = 0;
    int master_socket , addrlen , new_socket , client_socket[30] , 
          max_clients = 2 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
    struct R* r = (R*)malloc(sizeof(R));
	r->player1 = 8;
	r->player2 = 8;
	r->x_ball = 8;
	r->y_ball = 40;
	r->you_are = 0;
	r->player1_score = 0;
	r->player2_score = 0;
    char buffer[1025];  //data buffer of 1K 
        
    //set of socket descriptors 
    fd_set readfds;  
        
    //a message 
    char *message1 = "1";  
	char *message2 = "2";  
	// ** IMPORTANT ** for simplicity, ignore SIGPIPE
	signal(SIGPIPE, SIG_IGN);
    
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
        
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
        
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
        
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
        
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
    
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
    
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
		
        activity = select( max_sd + 1 , &readfds , NULL , NULL , &tv);  
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }
		else if(activity>0){
			//If something happened on the master socket , 
			//then its an incoming connection 
			if (FD_ISSET(master_socket, &readfds))  
			{  
				if ((new_socket = accept(master_socket, 
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
				{  
					perror("accept");  
					exit(EXIT_FAILURE);  
				}  
            
				//inform user of socket number - used in send and receive commands 
				printf("New connection , socket fd is %d , ip is : %s , port : %d\n" ,
				new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
          
				//send new connection greeting message 
				if(flag){
					if( send(new_socket, message1, strlen(message1), 0) != strlen(message1) )  {  
						perror("send");  
					}
					flag = 0;
				}
				else{
					if( send(new_socket, message2, strlen(message2), 0) != strlen(message2) ) {  
						perror("send");  
					}
					flag = 1;
				}
              
              
                
				puts("Welcome message sent successfully");  
                
				//add new socket to array of sockets 
				for (i = 0; i < max_clients; i++)  
				{  
					//if position is empty 
					if( client_socket[i] == 0 )  
					{  
						client_socket[i] = new_socket;  
						printf("Adding to list of sockets as %d\n" , i);  
						break;  
					}  
				}	  
			}
			//else its some IO operation on some other socket
			for (i = 0; i < max_clients; i++)  
			{  
				sd = client_socket[i];  
               
				if (FD_ISSET( sd , &readfds))  
				{  
					//Check if it was for closing , and also read the 
					//incoming message 
					if ((valread = read( sd , buffer, 1024)) == 0)  //read socket, store message in buffer
					{  
						//Somebody disconnected , get his details and print 
						getpeername(sd , (struct sockaddr*)&address , \
							(socklen_t*)&addrlen); 
						printf("Host disconnected , ip %s , port %d \n" , 
							inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                        
						//Close the socket and mark as 0 in list for reuse 
						close( sd );  
						client_socket[i] = 0;  
					}  
                    
					//Echo back the message that came in 
					else
					{  
						//set the string terminating NULL byte on the end 
						//of the data read 
						buffer[valread] = '\0';  
						printf("Read from client %d: %s\n", sd, buffer);
						player_move = buffer[0];
						printf("%c\n", player_move);
						if(i){
							if(player_move=='w')
								if(r->player1>3)
									r->player1--;
							if(player_move=='s')
								if(r->player1<13)
									r->player1++;
						}else{
							if(player_move=='w')
								if(r->player2>3)
									r->player2--;
								
							if(player_move=='s')
								if(r->player2<13)
									r->player2++;
						}
						r->you_are = i;
						r->test = player_move;
					}//else end  
				}//if end
				
			}//for end
		}else{
			if(start_flag){
				num_client = 0;
				for(j=0;j<max_clients;j++){
					if(client_socket[j]!=0)
						num_client++;
				}
				if(num_client==2){
					start_flag = 0;
					printf("starting game\n");
				}
			}else{
				
					if(k>=100000){
						if(x_dir)//ball moving up
							if(r->x_ball<16)
								r->x_ball++;
							else
								x_dir = 0;
				
						if(!x_dir) //ball moving down
							if(r->x_ball>1)
								r->x_ball--;
							else
								x_dir = 1;
						if(y_dir)//ball moving right
							if(r->y_ball<80)
								r->y_ball++;
							else{
								y_dir = 0;
								r->player1_score++;
							}
						if(!y_dir) //ball moving left
							if(r->y_ball>1)
								r->y_ball--;
							else{
								y_dir = 1;
								r->player2_score++;
							}
						//player1	
						if(!y_dir)//ball moving left
							if((r->x_ball<(r->player1+3))&&(r->x_ball>(r->player1-3))&&(r->y_ball>3)&&(r->y_ball<6))
								y_dir = 1;
							else
								y_dir = 0;
						
						//player2	
						if(y_dir)//ball moving left
							if((r->x_ball<(r->player2+3))&&(r->x_ball>(r->player2-3))&&(r->y_ball<(80-3))&&(r->y_ball>(80-5)))
								y_dir = 0;
							else
								y_dir = 1;
						
						k = 0;
						for(j=0;j<2;j++){
							sd = client_socket[j];
							if( send(sd, (void*)r, sizeof(R), 0) != sizeof(R))  
								perror("send");
						}
					}	
				k++;
			}

		}		
	}//while end
        
    return 0;  //fork to update ball every sec, just need to change r
}
