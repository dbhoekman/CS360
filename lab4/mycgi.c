#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX 10000
#define BLKSIZE 4096
typedef struct {
    char *name;
    char *value;
} ENTRY;

ENTRY entry[MAX];

main(int argc, char *argv[])
{
  int i, m, r, c, n, total = 0;
  char cwd[128];
  char buf[4096];
  FILE *fp, *gp;
  DIR *dirptr;
  struct dirent *ent;

  m = getinputs();    // get user inputs name=value into entry[ ]
  getcwd(cwd, 128);   // get CWD pathname

  printf("Content-type: text/html\n\n");
  printf("<p>pid=%d uid=%d cwd=%s\n", getpid(), getuid(), cwd);

  printf("<H1>Echo Your Inputs</H1>");
  printf("You submitted the following name/value pairs:<p>");

  for(i=0; i <= m; i++)
     printf("%s = %s<p>", entry[i].name, entry[i].value);
  printf("<p>");


  /*****************************************************************
   Write YOUR C code here to processs the command
         mkdir dirname
         rmdir dirname
         rm    filename
         cat   filename
         cp    file1 file2
         ls    [dirname] <== ls CWD if no dirname
  *****************************************************************/
  if(!strcmp(entry[0].value, "mkdir"))
  {
    mkdir(entry[1].value, 0755);
  }
  else if (!strcmp(entry[0].value, "rmdir"))
  {
    rmdir(entry[1].value);
  }
  else if (!strcmp(entry[0].value, "rm"))
  {
    unlink(entry[1].value);
  }
  else if (!strcmp(entry[0].value, "cat"))
  {
    if (entry[1].value != NULL)
    {
      fp = fopen(entry[1].value, "r");
      if (fp)
      {
        while ((c = fgetc(fp)) != EOF)
	{
	  putchar(c);
	  if (c == 10)
	  {
	    putchar(13);
	  }
	}
      }
      fclose(fp);
    }
  }
  else if (!strcmp(entry[0].value, "cp"))
  {
    fp = fopen(entry[1].value, "r");
    if (fp)
    {
      gp = fopen(entry[2].value, "w");
      if (gp)
      {
        while (n = fread(buf, 1, BLKSIZE, fp))
	{
	  fwrite(buf, 1, n, gp);
	  total += n;
	}
	printf("total = %d\n", total);
      }
    }
    fclose(fp);
    fclose(gp);
  }
  else if (!strcmp(entry[0].value, "ls"))
  {

    dirptr = opendir(".");

    if (strlen(entry[1].value))
    {
      dirptr = opendir(entry[1].value);
    }

    while ((ent = readdir(dirptr)) != NULL)
    {
      printf("<p>%s", ent->d_name);
    }
  }

  // create a FORM webpage for user to submit again
  printf("</title>");
  printf("</head>");
  printf("<body bgcolor=\"#008080\" link=\"#330033\" leftmargin=8 topmargin=8");
  printf("<p>------------------ DO IT AGAIN ----------------\n");

  printf("<FORM METHOD=\"POST\" ACTION=\"http://cs360.eecs.wsu.edu/~hoekman/cgi-bin/mycgi\">");

  //------ NOTE : CHANGE ACTION to YOUR login name ----------------------------
  //printf("<FORM METHOD=\"POST\" ACTION=\"http://cs360.eecs.wsu.edu/~YOURNAME/cgi-bin/mycgi\">");

  printf("Enter command : <INPUT NAME=\"command\"> <P>");
  printf("Enter filename1: <INPUT NAME=\"filename1\"> <P>");
  printf("Enter filename2: <INPUT NAME=\"filename2\"> <P>");
  printf("Submit command: <INPUT TYPE=\"submit\" VALUE=\"Click to Submit\"><P>");
  printf("</form>");
  printf("------------------------------------------------<p>");

  printf("</body>");
  printf("</html>");
}
