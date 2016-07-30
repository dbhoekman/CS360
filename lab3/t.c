//David Brent Hoekman
//Lab 3
#include "h.h"

int main (int argc, char **argv, char* const envp[])
{
    char* input_string;
    char** input_array;
    int i = 0;

    getPath();
    getHome();

    while(1)
    {
      input_string = getInput();

      printf ("The input was: ");
      printf (input_string);
      printf ("\n");
      
      input_array = parseInput(input_string);

      runCommand(input_array, envp);
      printf ("\n\n");
      memset(input_string, 0, strlen(input_string));
      memset(input_array, 0, sizeof(input_array));
    }
    return 0;
}
