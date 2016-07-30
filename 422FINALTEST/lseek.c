/*****************************************
CptS 360 Final Project

lseek

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 12/9/15
*****************************************/

//takes in fd and position by converting the string inputs into ints
int my_lseek(char *path)
{
	int fd;
	int position;
	OFT *oft;

	//Check for the fd
	if(!path)
	{
		printf("ERROR: missing fd\n");
		return;
	}
	//Cjeck for the wanted position
	if(!third)
	{
		printf("ERROR: missing position\n");
		return;
	}

	//Conver fd and postion to integers
	fd = atoi(path);
	position = atoi(third);

	//Ensure the fd is exists and is not busy
	if(running->fd[fd] && running->fd[fd]->refCount > 0)
	{
		oft = running->fd[fd];

		//Ensure we are not putting the position beyond the size of the fd
		if(position <= oft->inodeptr->INODE.i_size)
		{
			//Change the offset variable of the fd to the given position
			oft->offset = position;
			running->fd[fd] = oft;
		}
		else
		{
			printf("ERROR: position out of bounds\n");
			return -1;
		}
		return position;
	}
	else
	{
		printf("ERROR\n");
		return -1;
	}
}
