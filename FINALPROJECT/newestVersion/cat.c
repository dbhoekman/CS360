/*****************************************
CptS 360 Final Project

cat

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 12/8/15
*****************************************/


//cat a file
void my_cat(char *path)
{
	int n, i;
	int fd = 0;
	char buf[1024];
	int size = 1024;

	int testing = 0;
	
	//check for path
	if(!path)
	{
		printf("ERROR: missing path\n");
		return;
	}

	//make sure open mode is read
	strcpy(third, "0");

	//open with 0 for RD
	fd = open_file(path);
	my_pfd("");

	//printf("fd is %d\n", fd);
	while((n = my_read(fd, buf, size)))
	{
		//printf("count is %d\n", n);
		
		//null terminate the buffer
		buf[n] = '\0';

		i = 0;
		//print each char in the buffer, this is to handle \n
		while(buf[i])
		{
			putchar(buf[i]);
			if(buf[i] == '\n')
				putchar('\r');
			i++;
		}
	}
	
	printf("\n\r");
	close_file(fd);

	return;
}
