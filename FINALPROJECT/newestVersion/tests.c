/*****************************************
CptS 360 Final Project

tests

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 11/23/15
*****************************************/
int my_search(char *path)
{
	int i, ino;
	char buf[1024];
	char *cp;
	DIR *dp;
	MINODE *mip;
	INODE *ip;

	mip = root;

	ip = &mip->INODE;
	get_block(dev, ip->i_block[0], buf);

	cp = buf;
	dp = (DIR*)cp;

	while(cp < buf + 1024)
	{
		//printf("name is %s\n", dp->name);
		dp->name[strlen(dp->name)] = 0;
		if(strcmp(dp->name, path) == 0)
		{
			//printf("found %s at %d\n", path, dp->inode);
			return dp->inode;
		}

		cp += dp->rec_len;
		dp = (DIR*)cp;
	}
}

int lsTestCase(char givenpath[128], char expected[1024])
{
  printf("Testing >ls %s\n", givenpath);
  printf("Actual  : ");
  strcpy(pathname, givenpath);
  ls(pathname);
  printf("Expected: %s\n", expected);
  printf("TESTSTRING: %s\n", teststr);
  if (!strcmp(expected, teststr))
  {
      printf("TEST PASSED!\n\n");
      strcpy(teststr, "");
      return 1;
  }
  else
  {
    printf("TEST FAILED!\n\n");
    strcpy(teststr, "");
    return 0;
  }
}

//This tests the ls function.
//returns 0 if the expected ls print does not match the actual ls print.
int lsTest()
{
  char expected5[1024] = ". .. hugefile ";
  char expected6[1024] = "tiny is not a directory!";
  char expected7[1024] = "name hi does not exist";
  int porf = 1;

  printf("\n\n-------- TESTING LS FUNCTION --------\n");
  porf = porf & lsTestCase("Z", expected5);
  porf = porf & lsTestCase("tiny", expected6);
  porf = porf & lsTestCase("Z/hi", expected7);


  printf("While in directory /Y\n");
  porf = porf & lsTestCase("/Z", expected5);

  if (porf)
  {
    printf("\nALL LS TESTS PASSED!\n\n\n");
  }
}

//This tests the cd function.
//It compares the running inode after a cd call to an expected inode.
//If the running ino does not match the expected, returns 0.
int cdTest()
{
	MINODE *test_mip = running->cwd;

	printf("\n\n-------- TESTING CD FUNCTION --------\n");

	printf("\ntesting cd into bad path /fail\n");
	//test cd into bad pathname
	strcpy(pathname, "/fail");
	cd(pathname);

	//if the cwd did not change, it passed
	if(running->cwd == test_mip)
	{
		//did not cd into bad path
		printf("\nTEST PASSED\n");
	}
	else
	{
		//cd into a bad path, test failed
		printf("\nTEST FAILED\n");
		return 1;
	}

	printf("\ntesting cd into file /tiny\n");
	//test cd into file
	strcpy(pathname, "/tiny");
	cd(pathname);

	//if the cwd did not change, it passed
	if(running->cwd == test_mip)
	{
		//did not cd into file
		printf("\nTEST PASSED\n");
	}
	else
	{
		//cd into a file, test failed
		printf("\nTEST FAILED\n");
		return 1;
	}

	printf("\ntesting cd into directory /Y\n");
	//test cd into /Y
	strcpy(pathname, "/Y");
	cd(pathname);

	if(running->cwd->ino == 20)
	{
		printf("\nTEST PASSED\n");
	}
	else
	{
		//
		printf("\nTEST FAILED\n");
		return 1;
	}

	printf("\ntesting relative cd into Z\n");
	//test cd into Z relative
	strcpy(pathname, "Z");
	cd(pathname);

	if(running->cwd->ino == 20)
	{
		//
		printf("\nTEST PASSED\n");
	}
	else
	{
		//
		printf("\nTEST FAILED\n");
		return 1;
	}

	printf("\ntesting cd into /Z from /Y\n");
	//test cd into /Y from /Z
	strcpy(pathname, "/Z");
	cd(pathname);

	if(running->cwd->ino == 22)
	{
		//
		printf("\nTEST PASSED\n");
	}
	else
	{
		//
		printf("\nTEST FAILED\n");
		return 1;
	}

	printf("\ntesting cd into root (empty pathname)\n");
	//test cd into root
	strcpy(pathname, "");
	cd(pathname);

	if(running->cwd == proc[0].cwd)
	{
		//
		printf("\nTEST PASSED\n");
	}
	else
	{
		//
		printf("\nTEST FAILED\n");
		return 1;
	}

	printf("\nALL CD TESTS PASSED\n\n\n");
	return 0;
}

//This tests the chmod function.
//Changes the permissions of a given file, and compares it to the expected new permissions
//If the permissions do not mathc the expected, returns 0.
int chmodTest()
{
  MINODE *test_mip = NULL;
  INODE *ip = NULL;
  int ino = 0;
  int expected1 = 33279;
  int expected2 = 33188;
  int expected3 = 33233;

  strcpy(pathname, "");
	cd(pathname);
  test_mip = running->cwd;

	printf("\n\n-------- TESTING CHMOD FUNCTION --------\n\n");
  strcpy(pathname, "tiny");
  ino = getino(test_mip, pathname);
  test_mip = iget(dev, ino);

  ip = &test_mip->INODE;
  printf("tiny's current imode = %d\n", ip->i_mode);
  printf("Testing >chmod tiny 777\n");
  strcpy(third, "777");
  chmod_file(pathname);
  printf("\ntiny's new imode = %d\n", ip->i_mode);
  printf("tiny's expected imode = %d\n", expected1);
  if (ip->i_mode != expected1)
  {
    printf("TEST FAILED!\n\n");
    return 0;
  }
  printf("TEST PASSED!\n\n");

  ls("");

  printf("Testing >chmod tiny 644\n");
  strcpy(third, "644");
  chmod_file(pathname);
  printf("\ntiny's new imode = %d\n", ip->i_mode);
  printf("tiny's expected imode = %d\n", expected2);
  if (ip->i_mode != expected2)
  {
    printf("TEST FAILED!\n\n");
    return 0;
  }
  printf("TEST PASSED!\n\n");

  ls("");

  printf("Showing ls for Y\n");
  strcpy(pathname, "Y");
  ls(pathname);


  strcpy(pathname, "");
	cd(pathname);
  test_mip = running->cwd;

  strcpy(pathname, "Y/bigfile");
  ino = getino(test_mip, pathname);
  test_mip = iget(dev, ino);

  ip = &test_mip->INODE;
  printf("bigfiles's current imode = %d\n", ip->i_mode);
  printf("Testing >chmod Y/bigfile 721\n");
  strcpy(third, "721");
  strcpy(pathname, "Y/bigfile");
  chmod_file(pathname);
  printf("\nbigfile's new imode = %d\n", ip->i_mode);
  printf("bigfile's expected imode = %d\n", expected3);
  if (ip->i_mode != expected3)
  {
    printf("TEST FAILED!\n\n");
    return 0;
  }
  printf("TEST PASSED!\n\n");

  printf("CLEANNING UP BELOW!\n");
  strcpy(third, "644");
  strcpy(pathname, "Y/bigfile");
  chmod_file(pathname);
  cd("");

  printf("ALL CHMOD TESTS PASSED!\n\n\n");
  return 1;
}

//This test checks mkdir by using the mkdir function and checking the inode.
//If the inode does not match the expected inode returns 0.
int mkdirTest()
{
  MINODE *test_mip = running->cwd;
  INODE *ip = NULL;
  int ino = 0;
  int actual = 0;
  int flag = 0;
  int expected1 = 25;
  int expected2 = 26;
  int expected3 = 27;

  strcpy(pathname, "");
  cd(pathname);
  test_mip = running->cwd;
  printf("\n\n-------- TESTING MKDIR FUNCTION --------\n\n");

  printf("Testing >mkdir Testdir1\n");
  strcpy(pathname, "Testdir1");
  make_dir(pathname);
  printf("Getting ino on Testdir1\n");
  strcpy(pathname, "Testdir1");
  actual = getino(test_mip, pathname);
  printf("\nExpected ino = %d\n", expected1);
  printf("Actual ino = %d\n", actual);
  if (expected1 != actual)
  {
    printf("TEST FAILED!\n\n");
    flag = 1;
  }
  else
    printf("TEST PASSED!\n\n");



  printf("Testing >mkdir Happydir2\n");
  strcpy(pathname, "Happydir2");
  make_dir(pathname);
  printf("Getting ino on Happydir2\n");
  strcpy(pathname, "Happydir2");
  actual = getino(test_mip, pathname);
  printf("\nExpected ino = %d\n", expected2);
  printf("Actual ino = %d\n", actual);
  if (expected2 != actual)
  {
    printf("TEST FAILED!\n\n");
    flag = 1;
  }
  else
    printf("TEST PASSED!\n\n");


  printf("Testing >mkdir /Y/Happydir3\n");
  strcpy(pathname, "/Y/Happydir3");
  make_dir(pathname);
  printf("Getting ino on Happydir3\n");
  strcpy(pathname, "/Y/Happydir3");
  actual = getino(test_mip, pathname);
  printf("\nExpected ino = %d\n", expected3);
  printf("Actual ino = %d\n", actual);
  if (expected3 != actual)
  {
    printf("TEST FAILED!\n\n");
    flag = 1;
  }
  else
    printf("TEST PASSED!\n\n");

  if (flag)
  {
    printf("NOT ALL MKDIR TESTS PASSED!\n\n\n");
    return 0;
  }
  printf("ALL MKDIR TESTS PASSED!\n\n\n");
  return 1;
}


//This tests touch and creat
//First test makes sure the mtime changes on a file that exists.
//Returns 0 if the time isnt greater than what it was before.
//Second test touches a file that doesnt exits, and therefore makes one.
//Checks the inode of the new file, and if isn't what is expected, returns zero.
//This second test checks both if touch and creat are working.
int touchTest()
{
  MINODE *test_mip = running->cwd;
  INODE *ip = NULL;
  int ino = 0;
  int old = 0;
  int expected1 = 28;

  strcpy(pathname, "");
  cd(pathname);
  test_mip = running->cwd;
  printf("\n\n-------- TESTING TOUCH FUNCTION --------\n\n");

  strcpy(pathname, "tiny");
  ino = getino(test_mip, pathname);
  test_mip = iget(dev, ino);
  ip = &test_mip->INODE;

  old = ip->i_mtime;
  printf("tiny's current mtime = %d\n", ip->i_mtime);
  printf("Testing >touch tiny\n");
  strcpy(pathname, "tiny");
  touch_file(pathname);
  printf("tiny's new mtime = %d\n", ip->i_mtime);

  if(ip->i_mtime > old)
    printf("TEST PASSED!\n\n");
  else
  {
    printf("TEST FAILED!\n\n");
    return 0;
  }


  test_mip = running->cwd;
  printf("Testing >touch newfile\n");
  strcpy(pathname, "newfile");
  touch_file(pathname);

  strcpy(pathname, "newfile");
  ino = getino(test_mip, pathname);
  printf("Expected ino of newfile = %d\n", expected1);
  printf("Actual ino of newfile = %d\n", ino);
  if (ino != expected1)
  {
    printf("TEST FAILED!\n\n");
    return 0;
  }
  printf("TEST PASSED!\n\n");

  printf("\nALL TOUCH TESTS PASSED!\n\n\n");

}

//This tests the link function;
int linkTest()
{
  MINODE *test_mip = running->cwd;
  INODE *ip = NULL;
  int ino = 0;
  int actual = 0;
  int expected1 = 0;
  int expected2 = 22;

  strcpy(pathname, "");
  cd(pathname);
  test_mip = running->cwd;
  printf("\n\n-------- TESTING LINK FUNCTION --------\n\n");
  printf("Testing >link tiny new\n");
  strcpy(pathname, "tiny");
  strcpy(third, "new");
  my_link(pathname);

  //strcpy(pathname, "tiny");
  //ino = my_search(pathname);
  printf("ino of tiny is: %d\n", 24);
  strcpy(pathname, "new");
  actual = my_search(pathname);
  printf("ino of new is: %d\n", actual);

  if (24 != actual)
  {
    printf("TEST FAILED!\n\n");
    return 0;
  }
  printf("TEST PASSED!\n\n");
}

//This tests the rmdir function
//The first test removes an emptry dir and checks to make sure it cannot get the ino of the deleted dirname
//If it gets the ino, it must not have been deleted and returns 0.
//The secind test atempts to remove a non empty dir. It attempts to get the ino.
//If the ino it gets is not the expected ino of the non-removed directory it returns 0;
int rmdirTest()
{
  MINODE *test_mip = running->cwd;
  INODE *ip = NULL;
  int ino = 0;
  int flag = 0;
  int actual = 0;
  int expected1 = 0;
  int expected2 = 12;

  strcpy(pathname, "");
  cd(pathname);
  test_mip = running->cwd;
  printf("\n\n-------- TESTING RMDIR FUNCTION --------\n\n");

  printf("Testing >rmdir Testdir1\n");
  strcpy(pathname, "Testdir1");
  remove_dir(pathname);
  printf("Getting ino on Testdir1\n");
  strcpy(pathname, "Testdir1");
  actual = getino(test_mip, pathname);
  printf("\nExpected ino = %d\n", expected1);
  printf("Actual ino = %d\n", actual);
  if (expected1 != actual)
  {
    printf("TEST FAILED!\n\n");
    flag = 1;
  }
  else
    printf("TEST PASSED!\n\n");


  //This remove should not happen because X is not empty.
  printf("Testing >rmdir X\n");
  strcpy(pathname, "X");
  remove_dir(pathname);
  printf("Getting ino on X\n");
  strcpy(pathname, "X");
  actual = getino(test_mip, pathname);
  printf("\nExpected ino = %d\n", expected2);
  printf("Actual ino = %d\n", actual);
  if (expected2 != actual)
  {
    printf("TEST FAILED!\n\n");
    flag = 1;
  }
  else
    printf("TEST PASSED!\n\n");

  if (flag == 1)
  {
    printf("RMDIR TESTS FAILED!\n\n\n");
  }
  else
    printf("ALL RMDIR TESTS PASSED!\n\n\n");
}

//This tests the pwd function
//It does so by cd into different directories and calling the pwd function
//If the printed pwd does not match the expected pwd, returns 0.
int pwdTest()
{
  MINODE *test_mip = running->cwd;
  INODE *ip = NULL;
  int ino = 0;
  char expected1 [128] = "/";
  char expected2 [128] = "/X";
  char expected3 [128] = "/Y";
  char expected4 [128] = "/Y/Happydir3";

  strcpy(pathname, "");
  cd(pathname);
  test_mip = running->cwd;

  printf("\n\n-------- TESTING PWD FUNCTION --------\n\n");
  printf("Testing >pwd while in root\n");
  my_pwd();
  printf("Expected print: %s", expected1);
  printf("Expecteasdfasdd print: %s", teststr);

  if(strcmp(expected1, teststr))
  {
    printf("\nTEST FAILED!\n\n");
    return 0;
  }
  printf("TEST PASSED!\n\n");
  strcpy(teststr, "");

  printf("\ncd into X\n");
  strcpy(pathname, "X");
  cd(pathname);
  printf("Testing >pwd while in X\n");

  my_pwd();
  printf("Expected print: %s", expected2);
  if(strcmp(expected2, teststr))
  {
    printf("\nTEST FAILED!\n\n");
    return 0;
  }
  printf("\nTEST PASSED!\n\n");
  strcpy(teststr, "");


  printf("\ncd into /Y\n");
  strcpy(pathname, "/Y");
  cd(pathname);
  printf("Testing >pwd while in Y\n");

  my_pwd();
  printf("Expected print: %s", expected3);
  if(strcmp(expected3, teststr))
  {
    printf("\nTEST FAILED!\n\n");
    return 0;
  }
  printf("\nTEST PASSED!\n\n");
  strcpy(teststr, "");



  printf("\ncd into /Y/Happydir3\n");
  strcpy(pathname, "/Y/Happydir3");
  cd(pathname);
  printf("Testing >pwd while in /Y/Happydir3\n");

  my_pwd();
  printf("Expected print: %s", expected4);
  if(strcmp(expected4, teststr))
  {
    printf("\nTEST FAILED!\n\n");
    return 0;
  }
  printf("\nTEST PASSED!\n\n");

  strcpy(pathname, "");
  cd(pathname);

  printf("\n\nALL PWD TESTS PASSED!\n\n\n");
  return 1;
}

//This tests the cp function
//It does so by calling cp on two files and checks their INODES size
//If the INODES sizes do not match it will return 0.
int cpTest()
{
  MINODE *test_mip = running->cwd;
  INODE *ip = NULL;
  int ino = 0;
  int tinysize = 37;
  int bigsize = 12713;
  int newsize = 0;

  strcpy(pathname, "");
  cd(pathname);
  test_mip = running->cwd;

  printf("\n\n-------- TESTING CP FUNCTION --------\n\n");
  strcpy(pathname, "tiny");
  strcpy(third, "newtiny");
  printf("Testing >cp tiny newtiny\n");
  cp_file(pathname);
  strcpy(pathname, "newtiny");
  ino = getino(test_mip, pathname);
  if(ino != 0)
    test_mip = iget(dev, ino);

  ip = &test_mip->INODE;
  newsize = ip->i_size;

  printf("Size of tiny: %d\n", tinysize);
  printf("Size of newtiny: %d\n", newsize);

  if(tinysize != newsize)
  {
    printf("TEST FAILED\n\n");
    return 0;
  }
  else
    printf("TEST PASSED\n\n");
  newsize = 0;



  strcpy(pathname, "/Y/bigfile");
  strcpy(third, "/newbig");
  printf("Testing >cp /Y/bigfile newbig\n");
  cp_file(pathname);
  strcpy(pathname, "newbig");
  ino =  my_search(pathname);
  if(ino != 0)
    test_mip = iget(dev, ino);

  ip = &test_mip->INODE;
  newsize = ip->i_size;

  printf("Size of bigfile: %d\n", bigsize);
  printf("Size of newbig: %d\n", newsize);

  if(bigsize != newsize)
  {
    printf("TEST FAILED\n\n");
    return 0;
  }
  else
    printf("TEST PASSED\n\n");


  printf("ALL CP TESTS PASSED\n\n\n");
  return 1;
}

//This tests the mv function
//It does so by calling mv on two files and checks to make sure the oldfile no longer eixsts and the newfile does
//If the oldfile exists or the newfile does not exist it will return 0.
int mvTest()
{
  MINODE *test_mip = running->cwd;
  INODE *ip = NULL;
  int ino = 0, newino = 0;

  strcpy(pathname, "");
  cd(pathname);
  test_mip = running->cwd;

  printf("\n\n-------- TESTING MV FUNCTION --------\n\n");
  strcpy(pathname, "tiny");
  strcpy(third, "mvtiny");
  printf("Testing >mv tiny mvtiny\n");
  mv_file(pathname);
  strcpy(pathname, "tiny");
  ino = getino(test_mip, pathname);
  strcpy(pathname, "mvtiny");
  newino = getino(test_mip, pathname);


  printf("\n\nino of tiny: %d\n", ino);
  printf("ino of mvtiny: %d\n", newino);

  if(ino != 0 || newino == 0)
  {
    printf("TEST FAILED\n\n");
    return 0;
  }
  else
    printf("TEST PASSED\n\n");


  strcpy(pathname, "mvtiny");
  strcpy(third, "/Y/testmv");
  printf("Testing >mv mvtiny /Y/testmv\n");
  mv_file(pathname);
  strcpy(pathname, "mvtiny");
  ino = getino(test_mip, pathname);
  strcpy(pathname, "/Y/testmv");
  newino = getino(test_mip, pathname);

  strcpy(pathname, "Y");
  printf(">ls /Y\n");
  ls(pathname);

  printf("\n\nino of mvtiny: %d\n", ino);
  printf("ino of /Y/testmv: %d\n", newino);

  if(ino != 0 || newino == 0)
  {
    printf("TEST FAILED\n\n");
    return 0;
  }
  else
    printf("TEST PASSED\n\n");

  printf("ALL MV TESTS PASSED\n\n\n");
  strcpy(pathname, "");
  cd(pathname);
  return 1;
}

//This tests the lseek function
//It does so by opening a file and calling lseek on the fd
//If the fd offset is not what is expected, it will return 0.
int lseekTest()
{
  MINODE *test_mip = running->cwd;
  INODE *ip = NULL;
  int newseek10 = 10, newseek20 = 20;
  OFT *oft;

  strcpy(pathname, "");
  cd(pathname);
  test_mip = running->cwd;

  printf("\n\n-------- TESTING LSEEK FUNCTION --------\n\n");
  strcpy(pathname, "new");
  strcpy(third, "0");
  printf(">open new 0\n");
  open_file(pathname);
  my_pfd("");
  oft = running->fd[0];

  printf("Beginning offset: %d\n", oft->offset);
  strcpy(pathname, "0");
  strcpy(third, "10");
  printf(">lseek 0 10\n");
  my_lseek(pathname);
  printf("New offset: %d\n", oft->offset);
  printf("Expected offset: %d\n", newseek10);
  if(newseek10 != oft->offset)
  {
    printf("TEST FAILED\n\n");
    return 0;
  }
  else
  {
    printf("TEST PASSED\n\n");
  }


  strcpy(pathname, "0");
  strcpy(third, "100");
  printf(">lseek 0 100\n");
  my_lseek(pathname);
  printf("New offset: %d\n", oft->offset);
  printf("Expected offset: %d\n", newseek10);
  if(newseek10 != oft->offset)
  {
    printf("TEST FAILED\n\n");
    return 0;
  }
  else
  {
    printf("TEST PASSED\n\n");
  }


  strcpy(pathname, "0");
  strcpy(third, "20");
  printf(">lseek 0 20\n");
  my_lseek(pathname);
  printf("New offset: %d\n", oft->offset);
  printf("Expected offset: %d\n", newseek20);
  if(newseek20 != oft->offset)
  {
    printf("TEST FAILED\n\n");
    return 0;
  }
  else
  {
    printf("TEST PASSED\n\n");
  }

  printf("ALL LSEEK TESTS PASSED\n\n\n");
  strcpy(pathname, "0");
  strcpy(third, "");
  printf(">close 0\n");
  my_close(pathname);
  strcpy(pathname, "");
  cd(pathname);
  return 1;
}

int runTests()
{
  lsTest();
  cdTest();
  chmodTest();
  mkdirTest();
  touchTest();
  linkTest();
  rmdirTest();
  pwdTest();
  cpTest();
  mvTest();
  lseekTest();
}
