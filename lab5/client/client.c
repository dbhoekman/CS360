/*
Luke Darrow 11349190
CptS 360
*/
//**************************** ECHO CLIENT CODE **************************
// The echo client client.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX 256

// Define variables
struct hostent *hp;              
struct sockaddr_in  server_addr; 

int sock, r;
int SERVER_IP, SERVER_PORT; 

// clinet initialization code

int client_init(char *argv[])
{
  printf("======= clinet init ==========\n");

  printf("1 : get server info\n");
  hp = gethostbyname(argv[1]);
  if (hp==0){
     printf("unknown host %s\n", argv[1]);
     exit(1);
  }

  SERVER_IP   = *(long *)hp->h_addr;
  SERVER_PORT = atoi(argv[2]);

  printf("2 : create a TCP socket\n");
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock<0){
     printf("socket call failed\n");
     exit(2);
  }

  printf("3 : fill server_addr with server's IP and PORT#\n");
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = SERVER_IP;
  server_addr.sin_port = htons(SERVER_PORT);

  // Connect to server
  printf("4 : connecting to server ....\n");
  r = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (r < 0){
     printf("connect failed\n");
     exit(1);
  }

  printf("5 : connected OK to \007\n"); 
  printf("---------------------------------------------------------\n");
  printf("hostname=%s  IP=%s  PORT=%d\n", 
          hp->h_name, inet_ntoa(SERVER_IP), SERVER_PORT);
  printf("---------------------------------------------------------\n");

  printf("========= init done ==========\n");
}

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

main(int argc, char *argv[ ])
{
  int n, c, i = 0;
  char line[MAX], ans[MAX], copy[MAX], cwd[128];
  //token for parsing
  char *token;
  //input array to look for local command
  char **inputArray;
  //files and dir for operations
  FILE *fp, *gp;
  DIR *dir;
  struct dirent *dir_entry;

  getcwd(cwd, 128);

  if (argc < 3){
     printf("Usage : client ServerName SeverPort\n");
     exit(1);
  }

  client_init(argv);

  printf("********  processing loop  *********\n");
  while (1){
    printf("input a line : ");
    bzero(line, MAX);                // zero out line[ ]
    fgets(line, MAX, stdin);         // get a line (end with \n) from stdin

    line[strlen(line)-1] = 0;        // kill \n at end
    if (line[0]==0)                  // exit if NULL line
       exit(0);

    //our code
    strcpy(copy, line);
    inputArray = parseInput(copy);

    if(!strcmp(inputArray[0], "lcat"))
    {
        if(inputArray[1])
        {
            fp = fopen(inputArray[1], "r");
            if(fp != NULL)
            {
                while((c = fgetc(fp)) != EOF)
                {
                    putchar(c);
                }
            }
            fclose(fp);
        }
    }
    else if(!strcmp(inputArray[0], "lpwd"))
    {
        printf("printing cwd\n");
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf(stdout, "Current working dir: %s\n", cwd);
    }
    else if(!strcmp(inputArray[0], "lls"))
    {
        printf("doing lls\n");
        //if no path given, ls off cwd, else ls off pathname
        if(inputArray[1] == NULL)
        {
            printf("printing cwd\n");
            dir = opendir(".");
            while((dir_entry = readdir(dir)) != NULL)
                printf("%s ", dir_entry->d_name);
            closedir(dir);
            printf("\n");
        }
        else
        {
            printf("printing directory\n");
            dir = opendir(inputArray[1]);
            while((dir_entry = readdir(dir)) != NULL)
                printf("%s ", dir_entry->d_name);
            closedir(dir);
            printf("\n");
        }
    }
    else if(!strcmp(inputArray[0], "lcd"))
    {
        //if directory name present, chdir
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
    else if(!strcmp(inputArray[0], "lmkdir"))
    {
        //check if directory name present, if so mkdir
        if(!strcmp(inputArray[1], ""))
            printf("missing directory name\n");
        else
        {
            printf("making directory\n");
            mkdir(inputArray[1], 0775);
        }
    }
    else if(!strcmp(inputArray[0], "lrmdir"))
    {
        printf("removing %s\n", inputArray[1]);
        //check if directory name present, if so rmdir
        if(!strcmp(inputArray[1], ""))
            printf("missing directory name\n");
        else
            rmdir(inputArray[1]);
    }
    else if(!strcmp(inputArray[0], "lrm"))
    {
        //check if filename present, if so unlink
        if(!strcmp(inputArray[1], ""))
            printf("missing file name\n");
        else
            unlink(inputArray[1]);
    }
    else
    {
        //send to server to execute
        // Send ENTIRE line to server
        n = write(sock, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);
        
        // Read a line from sock and show it
        n = read(sock, ans, MAX);
        printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
    }
  }
}



