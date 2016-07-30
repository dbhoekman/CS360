#include "functions.c"

int main(int argc, char *argv[ ])
{
    int i, cmd = -1;
    char line[128];
    int (*func[4]) (char pathname[], char *parameter);

    //This is for the function pointer table
    func[0] = menu;
    func[1] = cd;
    func[2] = ls;
    func[3] = quit;

    init();
    mount_root();

    while(1){
        while (cmd == -1)
        {
            printf("P%d running: ", P0->pid);
            printf("input command : ");
            fgets(line, 128, stdin);
            line[strlen(line)-1] = 0;  // kill the \r char at end
            if (line[0]==0) continue;
            sscanf(line, "%s %s %64c", cname, pathname, parameter);
            cmd = findCmd(cname); // map cname to an index
            if (cmd == -1)
            {
                printf("No such operation!\n");
            }
        }
        (*func[cmd])(pathname, parameter);

        memset(line, 0, 128);
        memset(cname, 0, 64);
        memset(pathname, 0, 128);
        memset(parameter, 0, 64);
        for(i = 0; i < 64; i++)
        {
            memset(pathparts[i], 0, 128);
        }
        cmd = -1;

    }
}
