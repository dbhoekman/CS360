/*****************************************
CptS 360 Final Project

open

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 11/23/15
*****************************************/


int open_file(char path[124])
{
	int i, ino, fd = 0, perm = 0, mask = 0, mode = -1, offset = 0;
	MINODE *mip;
	INODE* ip;
	OFT* of = NULL;

	char buf[1024];

	printf("Opening file!\n");
	//Checks for the open mode and sets it to the correct int
	if (!strcmp(third, ""))
	{
		printf("No open mode specified!\n");
		return;
	}
	if (!strcmp(third, "0"))
		mode = 0;
	else if (!strcmp(third, "1"))
		mode = 1;
	else if (!strcmp(third, "2"))
		mode = 2;
	else if (!strcmp(third, "3"))
		mode = 3;
	else
	{
		printf("Invalid mode!\n");
		return;
	}

	//Checks the path and gets the ino for the pathname of what we want to open
	if (path[0] == '/')
		ino = getino(root, path);
	else  //get the ino
		ino = getino(running->cwd, path);

	//Ensures the file exists
	printf("%d\n", ino);
	if (ino == 0)
	{
		printf("ERROR: No such file!\n");
		return;
	}

	//iget on the ino and ensure it is a file
	mip = iget(dev, ino);
	ip = &mip->INODE;

	if (!S_ISREG(ip->i_mode))
	{
		printf("ERROR: Not a file!\n");
		iput(mip);
		return;
	}


	//checking permissions
	if (running->uid == ip->i_uid)
	{
		//Correct user ownership
		perm = 6;
	}
	else if (running->gid == ip->i_gid)
	{
		//Correct group ownership
		perm = 3;
	}

	mask = mode & (7 << perm);
	iput(mip);

	//Checks the created mask to the permissions of the file
	//This ensures that the running process has the permissions to read, write, or execute the file
	//Returns if the permissions are not met, and therefore will not open the file
	//Mode is the mode that the user would like to open the file in
	switch(mode)
	{
		//4 = 100
		case 1:
			if (((4 << perm) & mask) == 0 && running->uid != SUPER_USER)
			{
				printf("PERMISSION DENY!\n");
				iput(mip);
				return;
			}
			break;
		case 2:
		//2 = 010
			if (((2 << perm) & mask) == 0 && running->uid != SUPER_USER)
			{
				printf("PERMISSION DENY!\n");
				iput(mip);
				return;
			}
			break;
		case 3:
		//6 = 110
			if (((6 << perm) & mask) == 0 && running->uid != SUPER_USER)
			{
				printf("PERMISSION DENY!\n");
				iput(mip);
				return;
			}
			break;
		case 4:
		//2 = 010
		if (((2 << perm) & mask) == 0 && running->uid != SUPER_USER)
		{
			printf("PERMISSION DENY!\n");
			iput(mip);
			return;
		}
		offset = ip->i_size;
		break;
	}

	//Check the fd's in runnning to ensure we have an open fd for the file
	//If we do not have any useable fd's returns, and does not open the file
	for (fd = 0; fd < NFD; fd++)
	{
		if (running->fd[fd] == NULL)
			break;

		if (fd == NFD -1)
		{
			printf("Out of processes!\n");
			iput(mip);
			return;
		}
	}

	//Runs trough the OpenFileTable
	for (i = 0; i < NOFT; i++)
	{
		of = &OpenFileTable[i];

		//Checking to make sure it is not open already
		//If it is open, making sure it is open in read mode
		//Multiple reads is OK
		if(mode != 0 && of->refCount > 0 && of->inodeptr == mip && of->mode != 0)
		{
			printf("Cannot open, file is being used!\n");
			iput(mip);
			return;
		}

		//Checks to make sure the fd is not busy
		//Set some varibales and brake cause we have our fd
		if (of->refCount == 0)
		{
			of->mode = mode;
			of->offset = offset;
			of->inodeptr = mip;
			of->refCount++;

			running->fd[fd] = of;

			break;
		}

		//Breaks and returns if we do not have any room
		//Does not open the file
		if (i == NOFT - 1)
		{
			printf("No more room!\n");
			iput(mip);
			return;
		}
	}

	//Set the time and dirty
	ip->i_atime = time(0L);
	mip->dirty = 1;

	return fd;
}
