/*
Luke Darrow 11349190
CptS 360
*/
// This is the echo SERVER server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netdb.h>

#define  MAX 256

// Define variables:
struct sockaddr_in  server_addr, client_addr, name_addr;
struct hostent *hp;

int  sock, newsock;                  // socket descriptors
int  serverPort;                     // server port number
int  r, length, n;                   // help variables
char *token;

char** parseInput(char *input)
{
    int count = 0;
    char *str = NULL;
    char *tmpStr = NULL;
    // Allocate char* inputArr[20]
    char** inputArr = (char**)malloc(sizeof(char*)*20);
    str = strtok(input, " ");
    while(str)
    {
        tmpStr = (char *)malloc(sizeof(char)*strlen(str));
        strcpy(tmpStr, str);
        inputArr[count] = tmpStr;
        count++;
        str = strtok(NULL, " ");
    }
    inputArr[count] = NULL;
    return inputArr;
}

// Server initialization code:

int server_init(char *name)
{
   printf("==================== server init ======================\n");
   // get DOT name and IP address of this host

   printf("1 : get and show server host info\n");
   hp = gethostbyname(name);
   if (hp == 0){
      printf("unknown host\n");
      exit(1);
   }
   printf("    hostname=%s  IP=%s\n",
               hp->h_name,  inet_ntoa(*(long *)hp->h_addr));

   //  create a TCP socket by socket() syscall
   printf("2 : create a socket\n");
   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock < 0){
      printf("socket call failed\n");
      exit(2);
   }

   printf("3 : fill server_addr with host IP and PORT# info\n");
   // initialize the server_addr structure
   server_addr.sin_family = AF_INET;                  // for TCP/IP
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // THIS HOST IP address
   server_addr.sin_port = 0;   // let kernel assign port

   printf("4 : bind socket to host info\n");
   // bind syscall: bind the socket to server_addr info
   r = bind(sock,(struct sockaddr *)&server_addr, sizeof(server_addr));
   if (r < 0){
       printf("bind failed\n");
       exit(3);
   }

   printf("5 : find out Kernel assigned PORT# and show it\n");
   // find out socket port number (assigned by kernel)
   length = sizeof(name_addr);
   r = getsockname(sock, (struct sockaddr *)&name_addr, &length);
   if (r < 0){
      printf("get socketname error\n");
      exit(4);
   }

   // show port number
   serverPort = ntohs(name_addr.sin_port);   // convert to host ushort
   printf("    Port=%d\n", serverPort);

   // listen at port with a max. queue of 5 (waiting clients)
   printf("5 : server is listening ....\n");
   listen(sock, 5);
   printf("===================== init done =======================\n");
}


main(int argc, char *argv[])
{
   char *hostname;
   char **inputArray;
   char line[MAX], copy[MAX], cwd[128];
   double arg1, arg2, sum;
   DIR *dir;
   struct dirent *ent;

   getcwd(cwd, 128);

   if (argc < 2)
      hostname = "localhost";
   else
      hostname = argv[1];

   server_init(hostname);

   // Try to accept a client request
   while(1){
     printf("server: accepting new connection ....\n");

     // Try to accept a client connection as descriptor newsock
     length = sizeof(client_addr);
     newsock = accept(sock, (struct sockaddr *)&client_addr, &length);
     if (newsock < 0){
        printf("server: accept error\n");
        exit(1);
     }
     printf("server: accepted a client connection from\n");
     printf("-----------------------------------------------\n");
     printf("        IP=%s  port=%d\n", inet_ntoa(client_addr.sin_addr.s_addr),
                                        ntohs(client_addr.sin_port));
     printf("-----------------------------------------------\n");

     // Processing loop
     while(1){
       n = read(newsock, line, MAX);
       if (n==0){
           printf("server: client died, server loops\n");
           close(newsock);
           break;
      }

      if(!strcmp(line, "quit"))
        exit(1);

      // show the line string
      printf("server: read  n=%d bytes; line=[%s]\n", n, line);

      strcpy(copy, line);
      inputArray = parseInput(copy);

      if(!strcmp(inputArray[0], "pwd"))
      {
        printf("printing cwd\n");
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf(stdout, "Current working dir: %s\n", cwd);
      }
      else if (!strcmp(inputArray[0], "ls"))
      {
        printf("doing ls\n");
        //if no path given, ls off cwd, else ls off pathname
        if(inputArray[1] == NULL)
        {
            printf("printing cwd\n");
            dir = opendir(".");
            while((ent = readdir(dir)) != NULL)
                printf("%s ", ent->d_name);
            closedir(dir);
            printf("\n");
        }
        else
        {
            printf("printing directory\n");
            dir = opendir(inputArray[1]);
            while((ent = readdir(dir)) != NULL)
                printf("%s ", ent->d_name);
            closedir(dir);
            printf("\n");
        }
      }
      else if (!strcmp(inputArray[0], "cd"))
      {
        if(strcmp(inputArray[1], ""))
        {
            printf("Changing directory to %s", inputArray[1]);
            chdir(inputArray[1]);
            //change current working directory
            getcwd(cwd, 128);
        }
        else
            printf("lcd Error: No Directory Specified\n");
      }
      else if (!strcmp(inputArray[0], "mkdir"))
      {
        mkdir(inputArray[1], 0755);
      }
      else if (!strcmp(inputArray[0], "rmdir"))
      {
        rmdir(inputArray[1]);
      }
      else if (!strcmp(inputArray[0], "rm"))
      {
        unlink(inputArray[1]);
      }
      else if (!strcmp(inputArray[0], "get"))
      {
      }
      else if (!strcmp(inputArray[0], "put"))
      {
      }

      strcat(line, " ECHO");

      // send the echo line to client
      n = write(newsock, line, MAX);

      printf("server: wrote n=%d bytes; ECHO=[%s]\n", n, line);
      printf("server: ready for next request\n");
    }
 }
}
