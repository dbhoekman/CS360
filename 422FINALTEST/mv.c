//Read: path is source, third is dest
void mv_file(char *path)
{
	int i = 0, j, ino = 0;
	int fdDest = 0;
  int fdSrc = 0;
	char buf[1024];
  char src[1024];
  char dest[1024];

	MINODE *mip;
	INODE* ip;

	printf("Moving file!\n");

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

  //Verify that src exists and get its ino
  //Returns out if the src does not exist
  ino = getino(running->cwd, path);
  if(ino == 0)
  {
    printf("Source does not exist!\n");
    return;
  }
  //Call our link function which will link src to dest
  //Ensure third is set as destination
  strcpy(third, dest);
  my_link(src);

  //Call unlink on src to unlink it to its old parent
  //This removes src from its parents directory
  //this also reduces its link count by one
  my_unlink(src);

  printf("Done!\n");
  return;
}
