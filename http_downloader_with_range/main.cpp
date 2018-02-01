/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: pawan
 *
 * Created on February 13, 2017, 4:11 AM
 */

#include <cstdlib>  // atof, size_t functions
#include <iostream> // cout, data structures
#include <fstream>  // I/O functions
#include <cstring>  // For c_str(), string functions
#include <pthread.h>    // For Creating Threads
#include <sys/socket.h> // To Create Threads
#include <sys/types.h>  // For handling Threads
#include <netinet/in.h> // For Struct in_addr_t, in_port_t, sockaddr_in to store addresses for Internet address family
#include <netdb.h>  // Definens hostent structure
#include <arpa/inet.h>  // Provides in_addr structure, htons
#include <sstream>  // String related conversions
#include <iomanip> // For setprecision() function
#include <unistd.h> // For close(), read(), recv() functions



#define Split 20
using namespace std;

// Data Structure to store data for passing to a Thread

struct thread_data{
   int  thread_id, max;
   float sta;
   string path, host;
};

// To convert Number to String

template <typename T>
string NumberToString(T pNumber)
{
 ostringstream oOStrStream;
 cout<<setprecision(15);
 oOStrStream << pNumber;
 return oOStrStream.str();
}

// Thread in order to open Tcp Connection to Server and download and write a part on file

void *Tcpconnection(void *threadarg) {
   struct thread_data *my_data;
   
   my_data = (struct thread_data *) threadarg;
   string req, host, path;
   float a;
   int c, d; 
   c= my_data->thread_id;
   d= my_data->max;
   a = my_data->sta;
   host = my_data->host;
   path = my_data->path;
   cout<<setprecision(15);
   if(c==d)
    {
       req = "GET /"+path+" HTTP/1.1\r\nHOST: "+host+"\r\nConnection: close\r\nRange: bytes="+NumberToString(a*(d-1)+(d-1))+"-\r\n\r\n";
     }
   else if(c == 1)
   {
       req = "GET /"+path+" HTTP/1.1\r\nHOST: "+host+"\r\nConnection: close\r\nRange: bytes="+NumberToString(0)+"-"+NumberToString((a*c)+(c-1))+"\r\n\r\n";
   }
   else
   {
       req = "GET /"+path+" HTTP/1.1\r\nHOST: "+host+"\r\nConnection: close\r\nRange: bytes="+NumberToString((a*(c-1))+(c-1))+"-"+NumberToString((a*c)+(c-1))+"\r\n\r\n";
   }
   cout<<"\n Thread Created "<<c<<"\n"<<req<<endl;
   
    int sock;
    int bufsize = 1024;
    char buffer[bufsize];
    char *serverIP;
    unsigned int dstrlen;
    struct sockaddr_in server;
    struct hostent *hp;
    struct in_addr **addr_list;
    
    // Opening a part file to write partial data
    
    string filename;
    filename = "Part"+NumberToString(c);
    fstream out (filename.c_str(), ios::out | ios::binary);
    
    memset((char *) &server, 0, sizeof(server));
    
    hp = gethostbyname(host.c_str());
    cout<<hp->h_name<<endl;
    addr_list = (struct in_addr **)hp->h_addr_list;
    for(int i = 0; addr_list[i] != NULL; i++) {
        serverIP = inet_ntoa(*addr_list[i]);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr=inet_addr(serverIP);
    server.sin_port=htons(80);
    
    
    // Opening a TCP connection
    
    	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
             printf("Diagnostics download failure – Please try again.") ;
        if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
     	 	printf("Cannot connect to Server\n") ;
		exit(1);
	}
    
    dstrlen = strlen(req.c_str());
    
    // Sending the HTTP GET Request to Server via Socket
    
    if (send(sock, req.c_str(), dstrlen, 0) != dstrlen)
     	 printf("send() sent a different number of bytes than expected\r\n");
    else
	 printf("Successfully sent the request \n");
    
    memset(buffer, '\0', sizeof(buffer)-1);
    int e= 0;
    string clen;
    int rec = 0;
    
    //Receiving data from server
    
    while((rec = read(sock, buffer, 1)))
    {  
        clen = buffer;
        
        // Seprating HTTP header from Data
        
        if(e == 0)
        {
            if (clen == "\r")
            {
             rec = recv(sock, buffer, 1, 0);
             clen = buffer;
             if(clen == "\n")
             {rec = recv(sock, buffer, 1, 0);
             clen = buffer;
                if(clen == "\r")
                {rec = recv(sock, buffer, 1, 0);
                    clen = buffer;
                    if(clen == "\n")
                       e= 1;
                        e = recv(sock, buffer, 1, 0);
                }
             }        
                            
             }   
        }
    if(e==1){
        out.write(buffer,rec);
    }
    }
  
    close(sock);
    out.close();
   pthread_exit(NULL);
}


int main(int argc, char** argv) {
    
    cout<<setprecision(15);
    string url;
    int sp;
    
    if(argv[1]==NULL)
    {
        cout<<"\n Enter the URL of the file to be downloaded as Command Line Argument 1";
    }
    else
    {
       url = argv[1];
       string p;
       if(argv[2]==NULL)
       {
           
        cout<<"\n Enter the number of connections you want to make as Command Line Argument 2";
        
        }
       else 
       {    p= argv[2];
           sp = atoi(p.c_str());
       }
    }
    // Breaking down URL
    
    size_t found1 = url.find("https://");
    size_t found2 = url.find("http://");
    size_t found3 = url.find_last_of('/');
    string host, path, fname, name, format;
        if(found1 != string::npos)
        {
            host = url.substr(url.find("https://")+8, -1);
            string thost = host.substr(0, host.find_first_of('/'));
            path = host.substr(host.find('/')+1,-1);
            host = thost;
            fname = url.substr(found3+1);
        }
        else if(found2 != string::npos)
                {
            host = url.substr(url.find("http://")+7, -1);
            string thost = host.substr(0, host.find_first_of('/'));
            path = host.substr(host.find('/')+1,-1);
            host = thost;
            fname = url.substr(found3+1);
                }
    size_t found4 = fname.find('.');
    name = fname.substr(0,found4);
    format = fname.substr(found4+1);
    cout<<"\n Host = "<<host<<"\n Path = "<<path<<"\n Filename = "<<fname<<"\n Name = "<<name<<"\n Format = "<<format<< endl;
    
    int ssize, sock;
    int bufsize = 1024;
    char buffer[bufsize];
    char *serverIP;
    string dataFile;
    unsigned int dstrlen;
    string clength;
    float flen;
    
    // Opening the Main File to write data
    
    dataFile = fname;
    fstream outfile (dataFile.c_str(), ios::out | ios::binary);
 
    struct sockaddr_in server;
    struct hostent *hp;
    struct in_addr **addr_list;
    
    // Forming Head Request
    
    string request; 
    request  = "HEAD /"+path+" HTTP/1.1\r\nHOST: "+host+"\r\n\r\n";
    cout<<request;
    memset((char *) &server, 0, sizeof(server));
    
    hp = gethostbyname(host.c_str());
    cout<<hp->h_name<<endl;
    addr_list = (struct in_addr **)hp->h_addr_list;
    for(int i = 0; addr_list[i] != NULL; i++) {
        serverIP = inet_ntoa(*addr_list[i]);
    }
    cout<<serverIP;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr=inet_addr(serverIP);
    server.sin_port=htons(80);
    
    
    
    	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
             printf("Diagnostics download failure – Please try again.") ;
        if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
     	 	printf("Cannot connect to Server\n") ;
		exit(1);
	}
    
    dstrlen = strlen(request.c_str());
    cout<<"\n"<<dstrlen<<endl;
  
    // Sending Head Request
    
    if (send(sock, request.c_str(), dstrlen, 0) != dstrlen)
     	 printf("send() sent a different number of bytes than expected\r\n");
    else
	 printf("Successfully sent the request \n");
    
    memset(buffer, '\0', sizeof(buffer));
    
    int rc;
    
    // Reading Head Reply
    
    rc = read(sock, buffer, sizeof(buffer));
    clength = buffer;
    cout<<"\n Head Reply = "<<clength;
    cout<<rc<<endl;
    
    // Extracting the length of File to be retrieved
    
    clength = clength.substr(clength.find("Content-Length: ")+16,clength.find("\r\n")-6);
    flen = atof(clength.c_str());
    ssize = flen/sp;  
    cout<<"\n File length = "<<flen<<"  Split size = "<<ssize;
    
    
    pthread_t threads[sp+1];
    struct thread_data td[sp+1];
    int r;
    cout<<"\n\n\n\n\n";
    
    //Creating multiple threads to handle multiple tcp connections
    
    if (flen > 0){
    for (int i = 1 ; i<= sp ; i++)
        {
            cout << "main() : creating thread, " << i << endl;
            if(i == 1)
            {
                td[i].thread_id = i;
                td[i].sta = ssize;
                td[i].path = path;
                td[i].host = host;
                td[i].max = sp;
            }
            else {
                td[i].thread_id = i;
                td[i].sta = ssize;
                td[i].path = path;
                td[i].host = host;
                td[i].max = sp;

            }
            r = pthread_create(&threads[i], NULL, Tcpconnection, (void *)&td[i]);
		
            if (r){
                    cout << "Error:unable to create thread," << r << endl;
                    exit(-1);
    
                }    
        }
    
        for (int i = 1 ; i<= sp ; i++)
         {
            pthread_join(threads[i], NULL);
    
         }
    
    }
    else
    {   
        cout<<"\n Content Length could not be retrieved or Server does not Support Range";
    }
    
    cout<< "\n Threads Ended\n \n";
    
    // Combining multiple file parts
    
    string filetoopen;
    if(flen > 0){
    for (int j = 1; j<= sp; j++)
    {   
        filetoopen = "Part"+NumberToString(j);
        ifstream infile (filetoopen.c_str(), ios::binary | ios::in);
        if(infile.is_open())
        {   
            cout<<"\n Reading File Part "<<j<<endl;
            char buff[bufsize];
              do {
                 infile.read(buff, 1);
                 outfile.write(buff, infile.gcount());
             } while (infile && outfile);
 
        }
        else {
            cout<<"\n File part does not exist ";
        }
        infile.close();
    }
    }
    
    pthread_exit(NULL); 
    outfile.close();
    close(sock);
    
    return 0;
}
