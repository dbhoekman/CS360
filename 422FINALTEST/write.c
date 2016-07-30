/*****************************************
CptS 360 Final Project

write

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 12/8/15
*****************************************/

//write function
int my_write(int fd, char buf[], int nbytes)
{
	int i, j;
	int *ip;
	int counter = 0;
	int remain;
	int lbk, startByte;
	int blk;
	int indirect_blk, indirect_off;

	OFT *oftp;
	MINODE *mip;
	char write_buf[1024];
	char *cp, *cq = buf;

	//Check to make sure the fd can exist
	if(fd < 0 || fd >= NFD)
	{
		printf("ERROR: invalid file descriptor\n");
		return;
	}

	//Goes through the OpenFileTable to find the fd
	for(i = 0; i < NOFT; i++)
	{
		if(OpenFileTable[i].inodeptr == running->fd[fd]->inodeptr)
		{
			oftp = running->fd[fd];
			break;
		}
	}

	//Ensures the fd is open for write
	if(!oftp || (oftp->mode != 1 && oftp->mode != 2 && oftp->mode != 3))
	{
		printf("ERROR: wrong file mode for write\n");
		return;
	}

	//Getes the minode
	mip = oftp->inodeptr;

	printf("About to write to file\n");

	//Writes while there are more bytes to be written
	while(nbytes > 0)
	{
		lbk = oftp->offset / BLKSIZE;
		startByte = oftp->offset % BLKSIZE;

		if(lbk < 12)
		{
			if(mip->INODE.i_block[lbk] == 0)
			{
				mip->INODE.i_block[lbk] = balloc(mip->dev);
			}
			blk = mip->INODE.i_block[lbk];
		}
		else if(lbk >= 12 && lbk < 256 + 12)
		{
			//indirect

			//check if indirect block, else we need to allocate it
			if(!mip->INODE.i_block[12])
			{
				mip->INODE.i_block[12] = balloc(mip->dev);

				//fill the new block with 0's
				get_block(mip->dev, mip->INODE.i_block[12], write_buf);
				for(i = 0; i < BLKSIZE; i++)
					write_buf[i] = 0;
				put_block(mip->dev, mip->INODE.i_block[12], write_buf);
			}
			get_block(mip->dev, mip->INODE.i_block[12], write_buf);

			ip = (int*)write_buf + lbk - 12;
			blk = *ip;

			//if data block does not exist yet, have to allocate
			if(blk == 0)
			{
				*ip = balloc(mip->dev);
				blk = *ip;
			}
		}
		else
		{
			//double indirect

			//same stuff, if it doesn't exist, we need to allocate it
			if(mip->INODE.i_block[13] == 0)
			{
				mip->INODE.i_block[13] = balloc(mip->dev);
				//fill it with 0's!
				get_block(mip->dev, mip->INODE.i_block[13], write_buf);
				for(i = 0; i < BLKSIZE; i++)
					write_buf[i] = 0;
				put_block(mip->dev, mip->INODE.i_block[13], write_buf);
			}

			get_block(mip->dev, mip->INODE.i_block[13], write_buf);

			indirect_blk = (lbk - 256 - 12) / 256;
			indirect_off = (lbk - 256 - 12) % 256;

			ip = (int *)write_buf + indirect_blk;
			blk = *ip;

			//if no block yet, have to allocate it
			if(!blk)
			{
				ip = balloc(mip->dev);
				blk = *ip;

				get_block(mip->dev, blk, write_buf);
				for(i = 0; i < BLKSIZE; i++)
					write_buf[i] = 0;
				put_block(mip->dev, blk, write_buf);
			}

			get_block(mip->dev, blk, write_buf);

			ip = (int*)write_buf + indirect_off;
			blk = *ip;

			if(!blk)
			{
				*ip = balloc(mip->dev);
				blk = *ip;
				put_block(mip->dev, blk, write_buf);
			}
		}

		get_block(mip->dev, blk, write_buf);
		cp = write_buf + startByte;
		remain = BLKSIZE - startByte;

		//Writes byte by byte
		while(remain > 0)
		{
			*cp++ = *cq++;
			nbytes--;
			counter++;
			remain--;
			oftp->offset++;
			if(oftp->offset > mip->INODE.i_size)
				mip->INODE.i_size++;
			if(nbytes <= 0)
				break;
		}

		put_block(mip->dev, blk, write_buf);
	}

	mip->dirty = 1;
	printf("Wrote %d char into file descripter fd=%d\n", counter, fd);
	return nbytes;
}

//write function, this is the one that gets called in main
void do_write(char *path)
{
	int i, fd, nbytes;
	char *buf = (char*)malloc( (strlen(third) + 1) * sizeof(char*) );
	//open file table
	OFT *ofp;

	//Checks
	if(!path)
	{
		printf("ERROR: no path!\n");
		return;
	}

	if(!third)
	{
		printf("ERROR: no text to write\n");
		return;
	}

	//after making sure there's the right number of inputs,
	//take in the fd
	fd = atoi(path);

	for(i = 0; i < NOFT; i++)
	{
		//make sure it's in the global open file table
		ofp = &OpenFileTable[i];

		if(ofp->refCount == 0)
		{
			printf("ERROR: bad fd\n");
			return;
		}

		//have to verify the fd and its mode
		if(i == fd)
		{
			if(ofp->mode == 1 || ofp->mode == 2 || ofp->mode == 3)
				break;
			else
			{
				printf("ERROR: wrong mode for writing\n");
				return;
			}
		}
	}

	//copy the text to be written to a buffer
	strcpy(buf, third);
	nbytes = strlen(buf);
	printf("fd is %d\n", fd);
	my_write(fd, buf, nbytes);
	strcpy(write_test, buf);
	return;
}
