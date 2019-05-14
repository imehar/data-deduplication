#include<iostream>
// ./client <ip_add> <port>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/stat.h>
 
#include <sys/sendfile.h>
#include <fcntl.h>

using namespace std;


int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 

    if(argc != 3)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);		
        return 1;
    } 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");			
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 			

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2])); 				

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)			
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    int choice;
    char choice_str[1000];
    char filename[20],buf[100],ext[20],command[20];
    FILE *fp;
    int filehandle;
    struct stat obj;
    int size,status,i=1;
    char *f;
    int already_exits=0;
    int overwirte_choice = 1;
    char *pos;
    int num_lines;


    while(1)
    {
        printf("Enter a choice:\n1 - put\n2 - get\n3 - quit\n");

        // fgets(choice_str,1000,stdin);
        // choice_str[strlen(choice_str)-1] = '\0';
        // if(strlen(choice_str)>1)
        // {
        // 	printf("error\n");
        // 	continue;
        // }
        // choice = atoi(choice_str);
        cin >> choice;
        switch(choice)
        {


            case 1:{													
                printf("Enter the filename to put in server (with extension)\n");
                scanf("%s",filename);						
                if( access( filename, F_OK ) == -1 )
	            {
	                printf(" %s does not exits in client side \n",filename );
	                break;
	            } 
                
                strcpy(buf,"put ");
                strcat(buf,filename);
                send(sockfd,buf,100,0);						
                // recv(sockfd,&already_exits,sizeof(int),0);
                // if(already_exits){
                //     printf("same name file already exits in server 1. overwirte 2.NO overwirte\n");	
                //     scanf("%d",&overwirte_choice);
                // }
                // send(sockfd,&overwirte_choice,sizeof(int),0);			
                // if(overwirte_choice==1)
                // {
                    stat(filename, &obj);
                    size = obj.st_size;
                    send(sockfd, &size, sizeof(int), 0);
                    filehandle = open(filename,O_RDONLY);
                    sendfile(sockfd, filehandle, NULL, size);				
                    recv(sockfd, &status, sizeof(int), 0);
                    if(status){
                        printf("%s File stored successfully\n",filename);
                    }
                    else{
                        printf("%s File failed to be stored to remote machine\n" , filename); 
                    }

                // }
                
              break;
                }
            case 2:{
                int c=0;
                printf("Enter the filename to get from server (with extension)\n");
                scanf("%s",filename);
                strcpy(buf,"get ");
                strcat(buf,filename);
                send(sockfd,buf,100,0);
                recv(sockfd,&already_exits,sizeof(int),0);
                if(already_exits==0){
                    cout<<"File not found on server\n";
                    continue;
                }
                int size_file=0;


                recv(sockfd,&size_file,sizeof(int),0);
                char* in_char = (char*) malloc(size_file);
                recv(sockfd, in_char, size_file, 0);
                int filehandle_ = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                c = write(filehandle_, in_char, size_file);
                close(filehandle_);
                // send(sockfd, &c, sizeof(int), 0);
                if (c)
                {
                    cout<<"File successfully received from server\n";
                }
                else{
                    cout<<"File failed to receive from server \n";
                }

                break;
            }
            case 3:{
                strcpy(buf, "quit");
                send(sockfd, buf, 100, 0);					
                recv(sockfd, &status, 100, 0);
                if(status)
                {
                    printf("Server closed\nQuitting..\n");
                    exit(0);
                }
                printf("Server failed to close connection\n");		
                break;
            }

            default:{
            	printf("choose the vaild option\n");
            	break;
            }


        }

    }

    return 0;
}
