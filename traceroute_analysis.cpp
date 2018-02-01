/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Pawan
 *
 * Created on 30 January, 2017, 9:19 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>

using namespace std;

    string trim(const string& str)
    {   string s;
        size_t first = str.find_first_not_of(' ');
         if (string::npos == first)
             {
               return str;
             }
        size_t last = str.find_last_not_of(' ');
        s = str.substr(first, (last - first + 1));
        size_t second = s.find_first_not_of('\t');
        if (string::npos == second)
             {
               return s;
             }
        size_t slast = s.find_last_not_of(' ');
        return s.substr(second, (slast - second + 1));
    }
  
    
int main(int argc, char** argv) {

    string line;
    ifstream myfile;
    ofstream tmpfile ("temp.txt");                  // Making a temp file with proper formatting
    if (tmpfile.is_open())
	{	if (argv[1] == NULL)
		{
		cout << "Please Enter the File name with location as Command Line Argument";
		}
		else {
             myfile.open(argv[1]);
             int i = 0;
             while(!myfile.eof())
                {   
                    getline(myfile, line);
                    line = trim(line);
                    tmpfile<<line<<"\n";
                    i++;
                }   
     
		}
		
	}
    myfile.close();
    tmpfile.close();
    
    string incoming[80][4];
    string outgoing[100][3];
    
    int i = 0;
    int j = 0;
    
    ifstream uyfile ("temp.txt");
    if (uyfile.is_open())                           // Logic for Extraction of specific information
    {
        while(!uyfile.eof())  
        {
        getline(uyfile, line);
        size_t found = line.find(" proto ICMP (1),");
        
             if(found != string::npos)
                 {
                     size_t l = line.find_first_of(' ');
                     incoming[i][0] = line.substr(0,l);
                     getline(uyfile, line);
                     size_t k = line.find_first_of(' ');
                     incoming[i][1] = line.substr(0,k);
                     getline(uyfile, line);
                     incoming[i][2] = line.substr(line.find("id")+3,line.find(", of")-line.find("id")-3); 
                     i++;
                }  
             else{
            size_t found1 = line.find(" proto TCP (6),");
            
            if(found1 != string::npos)
                 {
                    size_t m = line.find_first_of(' ');
                    outgoing[j][0] = line.substr(0,m);
                    outgoing[j][1] = line.substr(line.find("ttl ")+3,line.find(", id")-line.find("ttl ")-3);
                    outgoing[j][2] = line.substr(line.find("id")+3,line.find(", of")-line.find("id")-3);
                    getline(uyfile, line);
                    size_t k = line.find_first_of(' ');
                     outgoing[j][3] = line.substr(0,k);
                    j++;
            }
             }
         }  
    }   
    uyfile.close();
    
    cout<<"\n\n Outgoing Packet Information \n";
    for (int p = 0;  p<j; p++)                          // Printing Outgoing packet information
    { cout<<"Outgoing Packet : "<<p<<"\n";
        cout<<"TimeStamp : "<<outgoing[p][0]<<"\n";
        cout<<"TTL : "<<outgoing[p][1]<<"\n";
        cout<<"ID : "<<outgoing[p][2]<<"\n";
        cout<<"Sender's ID : "<<outgoing[p][3]<<"\n\n";
    }
    
    cout<<"\n\n Incoming Packet Information \n";
    
    for (int a = 0;  a<i; a++)                        // Printing Incoming packet information
    { 
        cout<<"Incoming Packet : "<<a<<"\n";
        cout<<"TimeStamp : "<<incoming[a][0]<<"\n";
        cout<<"Router's IP Address : "<<incoming[a][1]<<"\n";
        cout<<"ID : "<<incoming[a][2]<<"\n\n";
    }    
    
    for (int a=0; a<i; a++)                           // For Consecutive Packets related to same ICMP Source
    {  for (int b=0; b<i; b++)
         { if (incoming[a][1]==incoming[b][1])
                incoming[b][2] = incoming[a][2];
         }
    }
    cout<<"===================================================================";
    cout<<"\n\n REQUIRED OUTPUT - TCPDUMP ANALYSIS";
    
    int d;
    cout<<setprecision(10);
    for (int a=0; a<j; a++)
        { d=0;
        for (int b=0; b<i; b++)
            {
                if(outgoing[a][2] == incoming[b][2])
                    {
                        if (d == 0)
                            {   d = 1;
                                cout<<"\n";
                                cout<<"TTL : "<<outgoing[a][1]<<"\n";
                                cout<<"Router's IP : "<<incoming[b][1]<<"\n";
                                cout<<"RTT : "<<(atof(incoming[b][0].c_str())- atof(outgoing[a][0].c_str()))*1000<<" ms"<<"\n";
                        
                            }
                        else {
                            cout<<"RTT : "<<(atof(incoming[b][0].c_str())- atof(outgoing[a][0].c_str()))*1000<<" ms"<<"\n";
                            }
                    }
            }
        }
        
    return 0;
}

