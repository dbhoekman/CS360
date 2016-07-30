#include "type.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ext2fs/ext2_fs.h>

init()
{

}

findCmd(char name[64])
{

}

int main(int argc, char *argv[ ])
{
  int i, cmd;
  char line[128], cname[64], pathname[128], parameter[64];
  PROC running;
  running.pid = 12;

  init();


  while(1){
      printf("P%d running: ", running.pid);
      printf("input command : ");
      fgets(line, 128, stdin);
      line[strlen(line)-1] = 0;  // kill the \r char at end
      if (line[0]==0) continue;

      sscanf(line, "%s %s %64c", cname, pathname, parameter);

      cmd = findCmd(cname); // map cname to an index
    }
}
