#include<iostream>
// ./server <port>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <fcntl.h>
#include<fstream>
#include<sstream>
#include <map>
#include "../utils/listfile.h"
 

#include <sys/stat.h>

#include <sys/sendfile.h>

std::string getexepath()
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( result, (count > 0) ? count : 0 );
}


int main(int argc, char *argv[])
{
    string loc = getexepath();
    loc = loc.substr(0,loc.length()-1);
    string loc_hash = loc.substr(0,loc.length()-10) + "utils/hashtable/hashtable.txt";
    // cout<<"loc "<<loc<<endl<<loc_hash<<endl;

    //genereating hash of all files in current servercli folder
    listfiles(loc,loc_hash);

    //connection between server client
    int listenfd = 0, connfd = 0,k;
    struct sockaddr_in serv_addr , client; 

    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);             
    serv_addr.sin_port = htons(atoi(argv[1]));                     

    k = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));        


    k = listen(listenfd, 10);       

    socklen_t client_size = sizeof(sockaddr_in);
    connfd = accept(listenfd, (struct sockaddr*)&client, &client_size);      
    
    printf("connected to the client\n");
    printf("IP address is: %s\n", inet_ntoa(client.sin_addr));

    char buf[100],command[5],filename[20],ext[20],lscommand[20];      
    int size,i,filehandle;
    struct stat obj;
    int already_exits = 0;
    int overwrite_choice = 1;
    char *pos;
    string client_addr_str = inet_ntoa(client.sin_addr);
    string client_addr_loc = client_addr_str;
    client_addr_str += ".txt";
    cout<< client_addr_str<<endl;
    map<string, string> client_details;
    if( access( ( loc + "/clientdetails/" +client_addr_str).c_str(), F_OK ) != -1 ){
        ifstream ifs { loc + "/clientdetails/" +client_addr_str};
        boost::archive::text_iarchive iarch(ifs);
        iarch >> client_details;
    }
    else{
        fstream filecreating;
        filecreating.open(loc + "/clientdetails/" +client_addr_str,ios::out );
        filecreating.close();
    }



    while(1)                    
    {   
        
        recv(connfd,buf,100,0);
        sscanf(buf,"%s",command);              
//----------- for put command -------------------------//

        if(!strcmp(command,"put"))
        {
            int c = 0, len;
            char *f;
            
            sscanf(buf+strlen(command), "%s", filename);
            printf("Putting the file %s\n",filename );
            i = 1;
            recv(connfd, &size, sizeof(int), 0);
            f = (char*) malloc(size);
            recv(connfd, f, size, 0);
            filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
            c = write(filehandle, f, size);
            close(filehandle);

            string outString (f);
            string outputsha = sha256(outString);
            string fname_( filename);
            
            client_details[fname_] = outputsha;

            send(connfd, &c, sizeof(int), 0);

            std::map<string, string> hasht;
            ifstream ifs_hash {loc_hash};
            boost::archive::text_iarchive iarch_hash(ifs_hash);
            iarch_hash >> hasht;
            if (hasht.find(outputsha) == hasht.end())
            {
                hasht[outputsha] = loc + fname_;
            }else{
                remove((loc + fname_).c_str());
            }

            ofstream ofs_hash {loc_hash};
            boost::archive::text_oarchive oarch_hash(ofs_hash);
            oarch_hash << hasht;

            ofstream ofs {loc + "/clientdetails/" +client_addr_str};
            boost::archive::text_oarchive oarch(ofs);
            oarch << client_details;
            
        } // ending of put option
        else if (!strcmp(command,"get"))
        {
            sscanf(buf+strlen(command), "%s", filename);
            // strcat(filename,"\0");
            string f_name_ (filename);
            cout<<"Getting the file "<<f_name_<<endl;
            string hashval_;
            if (client_details.find(f_name_) != client_details.end())
            {
                hashval_ = client_details[f_name_];
                already_exits = 1;
                send(connfd,&already_exits,sizeof(int),0);
                struct stat buffer;

                std::map<string, string> hasht;
                ifstream ifs_hash {loc_hash};
                boost::archive::text_iarchive iarch_hash(ifs_hash);
                iarch_hash >> hasht;

                string loc_file = hasht[hashval_];
                stat (loc_file.c_str(), &buffer);
                int size_file = buffer.st_size;

                send(connfd,&size_file,sizeof(int),0);

                int filehandle_ = open(loc_file.c_str(),O_RDONLY);
                sendfile(connfd,filehandle_,NULL,size_file);


            }
            else{
                // cout<<"File not found on server \n";
                already_exits  =0;
                send(connfd,&already_exits,sizeof(int),0);
                continue;
            }

        }


//--------------------quit command----------------------------------------//
        else if(!strcmp(command, "quit"))
        {
            printf("FTP server quitting..\n");
            i = 1;
            send(connfd, &i, sizeof(int), 0);           
            exit(0);
        }//ending of quit option 

    }
}
