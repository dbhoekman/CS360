//Read: path is source, third is dest
void cp_file(char *path)
{
	int i = 0, j;
	int fdDest = 0;
  int fdSrc = 0;
	char buf[1024];
  char src[1024];
  char dest[1024];

	MINODE *mip;
	INODE* ip;

	printf("Copying file!\n");

  //Check to make sure there is a sorce and dest defined
  //Strcpy to src and dest so we dont lose them
	if (!strcmp(path, ""))
	{
		printf("No source specified!\n");
		return;
	}
  strcpy(src, path);
	if (!strcmp(third, ""))
	{
		printf("No destination specified!\n");
		return;
	}
  strcpy(dest, third);

  //We call our touch function to see if the destination exists already
  //If the dest doesnt exist, touch calls creat to make it for us
  touch_file(dest);

  //Now we open the src for read
  //make sure open mode is read
  strcpy(third, "0");

  //open with 0 for RD
  fdSrc = open_file(src);

  //Now we open the dest for write
  //make sure open mode is write
  strcpy(third, "1");

  //open with 0 for RD
  fdDest = open_file(dest);
	my_pfd("");

  //This loop reads from the src until there is nothing left to read
  //And writes it into the dest
  while (i = my_read(fdSrc, buf, BLKSIZE))
  {
    my_write(fdDest, buf, i);
  }

  //Close our files
  close_file(fdSrc);
  close_file(fdDest);
  my_pfd("");

  printf("Done!\n");
  return;
}
