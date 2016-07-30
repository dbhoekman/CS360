#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct my_Node
{
	char name[64];
	char type;
	struct Node *childPtr, *siblingPtr, *parentPtr;

}Node;

//Global Variables 
Node *root, *cwd;
char line[128];
char command[16], pathname[64];
char dirname[64], basename[64];
int run = 1;
//Node *temp = malloc(sizeof(Node));

void initialize()
{
	root = malloc(sizeof(Node));
	strcpy(root->name, "/");
	root->type = 'D';
	root->childPtr = NULL;
	root->siblingPtr = NULL;
	root->parentPtr = NULL;

	cwd = root;
}

void init_tree()
{
	Node *child = malloc(sizeof(Node));
	Node *child2 = malloc(sizeof(Node));
	Node *child3 = malloc(sizeof(Node));
	child->type = 'D';
	strcpy(child->name, "a");
	child->childPtr = NULL;
	child->siblingPtr = NULL;
	child->parentPtr = root;
	root->childPtr = child;

	child2->type = 'D';
	strcpy(child2->name, "b");
	child2->childPtr = NULL;
	child2->siblingPtr = NULL;

	child2->parentPtr = child;
	child->childPtr = child2;

	child3->type = 'D';
	strcpy(child3->name, "c");
	child3->childPtr = NULL;
	child3->siblingPtr = NULL;
	child3->parentPtr = child2;
	child2->childPtr = child3;
}

int findCommand(char* command)
{
	//This compares the given command and the commands we can run
	//Returns -1 if the given command was invalid
	if (strcmp(command, "menu") == 0)
		return 0;
	else if (strcmp(command, "mkdir") == 0)
		return 1;
	else if (strcmp(command, "rmdir") == 0)
		return 2;
	else if (strcmp(command, "cd") == 0)
		return 3;
	else if (strcmp(command, "ls") == 0)
		return 4;
	else if (strcmp(command, "pwd") == 0)
		return 5;
	else if (strcmp(command, "creat") == 0)
		return 6;
	else if (strcmp(command, "rm") == 0)
		return 7;
	else if (strcmp(command, "save") == 0)
		return 8;
	else if (strcmp(command, "reload") == 0)
		return 9;
	else if (strcmp(command, "quit") == 0)
		return 10;

	return -1;
}

void menu()
{
	printf("command was menu\n");
	printf("Available commands: menu, mkdir, rmdir, cd, ls, pwd, creat, rm, save, reload, quit\n");
}

int absolute_check()
{
	if (pathname[0] == '/')
		return 1;
	else
		return 0;
	return -1;
}

//This sets up our global variables
void dirSetup(int abs)
{
	int i = 0;
	int j = 0;
	int num = 0;
	int counter = 0;
	if (abs == 1)
	{
		for (i = 0; i < strlen(pathname); i++)
		{
			if (pathname[i] == '/')
				num++;
		}

		for (i = 0; i < strlen(pathname); i++)
		{
			if (pathname[i] == '/')
			{
				counter++;
			}
			if (counter != num)
			{
				dirname[i] = pathname[i];
				j++;
			}
			else
				basename[i - j] = pathname[i + 1];
		}
	}
	else
	{
		for (i = 0; i < strlen(pathname); i++)
		{
			if (pathname[i] == '/')
				num++;
		}

		for (i = 0; i < strlen(pathname); i++)
		{
			if (pathname[i] == '/')
			{
				counter++;
			}
			if (counter != num)
			{
				dirname[i] = pathname[i];
				j++;
			}
			else
				basename[i - j] = pathname[i + 1];
		}
	}
}

void addDir(Node* parentnode, char typ)
{
	Node *tempnode = malloc(sizeof(Node));
	tempnode->type = typ;
	strcpy(tempnode->name, basename);
	tempnode->childPtr = NULL;
	tempnode->siblingPtr = NULL;
	tempnode->parentPtr = parentnode;
	if (parentnode->childPtr == NULL) //For the case when the given pathname has nothing in it
	{
		parentnode->childPtr = tempnode;
	}
	else
	{
		Node *tempnode2 = malloc(sizeof(Node));
		tempnode2 = parentnode->childPtr;
		if ((strcmp(tempnode2->name, basename)) == 0) //If only one is same as add
		{
			printf("The directory already exists\n");
			return;
		}
		while (tempnode2->siblingPtr != NULL)
		{
			tempnode2 = tempnode2->siblingPtr;
			if ((strcmp(tempnode2->name, basename)) == 0)
			{
				printf("The directory already exists\n");
				return;
			}
		}
		tempnode2->siblingPtr = tempnode;
	}

}

Node* search_dir(int abs)
{
	int i = 0, j = 0;
	Node *temp = malloc(sizeof(Node));
	char tempstr[64];
	if (abs == 1)
	{
		temp = root;
		for (i = 1; i < strlen(dirname) + 1; i++)
		{
			if (dirname[i] != '/' && dirname[i] != '\0')
			{
				tempstr[j] = dirname[i];
			}
			else
			{
				tempstr[j + 1] = '\0';
				temp = temp->childPtr;
				if (temp == NULL)
				{
					return NULL;
				}
				while (temp != NULL)
				{
					if (strcmp(temp->name, tempstr) == 0 && temp->type == 'D')
					{
						for (j = 0; j < strlen(tempstr); j++)
						{
							tempstr[j] = '\0';
						}
						j = 0;
						break;
					}
					else
					{
						temp = temp->siblingPtr;
					}
				}

			}
		}
		return temp;
	}
	else if (abs == 0)
	{
		temp = cwd;
		for (i = 0; i < strlen(dirname) + 1; i++)
		{
			if (dirname[i] != '/' && dirname[i] != '\0')
			{
				tempstr[j] = dirname[i];
			}
			else
			{
				tempstr[j + 1] = '\0';
				temp = temp->childPtr;
				if (temp == NULL)
				{
					return NULL;
				}
				while (temp != NULL)
				{
					if (strcmp(temp->name, tempstr) == 0 && temp->type == 'D')
					{
						for (j = 0; j < strlen(tempstr); j++)
						{
							tempstr[j] = '\0';
						}
						j = 0;
						break;
					}
					else
					{
						temp = temp->siblingPtr;
					}
				}

			}
		}
		return temp;
	}
	return NULL;
}

void mkdir()
{
	int absolute = -1;
	int flag = 0;
	Node *tempnode = malloc(sizeof(Node));
	char basenametemp[128];
	printf("command was mkdir\n");
	printf("Pathname: %s\n", pathname);
	absolute = absolute_check();
	dirSetup(absolute);
	printf("dirname: %s\n", dirname);
	printf("basename: %s\n", basename);
	tempnode = search_dir(absolute);
	if (tempnode == NULL)
	{
		printf("The given directory does not exist!\n");
		return;
	}
	else
	{
		addDir(tempnode, 'D');
	}
}

void removeDir(Node* parentnode, char typ)
{
	Node *tempnode = malloc(sizeof(Node));
	Node *tempnode2 = malloc(sizeof(Node));

	tempnode = parentnode;
	tempnode2 = parentnode;
	tempnode = tempnode->childPtr;
	if (strcmp(tempnode->name, basename) == 0)
	{
		tempnode2->childPtr = tempnode->siblingPtr;
		strcpy(tempnode->name, "\0");
		tempnode->childPtr = NULL;
		tempnode->parentPtr == NULL;
		tempnode->siblingPtr == NULL;
		return;
	}
	tempnode2 = tempnode;
	tempnode = tempnode->siblingPtr;
	while (tempnode != NULL)
	{
		if (strcmp(tempnode->name, basename) == 0)
		{
			if (tempnode->childPtr == NULL && tempnode->type == typ)
			{
				tempnode2->siblingPtr = tempnode->siblingPtr;
				strcpy(tempnode->name, "\0");
				tempnode->childPtr = NULL;
				tempnode->parentPtr == NULL;
				tempnode->siblingPtr == NULL;
				return;
			}
		}
		tempnode2 = tempnode;
		tempnode = tempnode->siblingPtr;
	}

}

void rmdir()
{
	int absolute = -1;
	Node *tempnode = malloc(sizeof(Node));
	printf("command was rmdir\n");
	printf("Pathname: %s\n", pathname);
	absolute = absolute_check();
	dirSetup(absolute);
	printf("dirname: %s\n", dirname);
	printf("basename: %s\n", basename);
	tempnode = search_dir(absolute);
	if (tempnode == NULL)
		printf("The given directory does not exist!\n");
	else
	{
		removeDir(tempnode, 'D');
	}
}

void cd()
{
	Node *tempnode = malloc(sizeof(Node));
	char basenametemp[128];
	int absolute = -1;
	int flag = 0;
	absolute = absolute_check();
	dirSetup(absolute);
	if (strcmp(pathname, "/") == 0)
	{
		cwd = root;
		return;
	}
	else if (strcmp(pathname, "") == 0 && cwd == root)
	{
		return;
	}
	else if (strcmp(pathname, "") == 0 && cwd != root)
	{
		cwd = cwd->parentPtr;
		return;
	}
	if (strcmp(dirname, "") == 0)
	{
		flag = 1;
		strcpy(basenametemp, pathname);
	}
	tempnode = search_dir(absolute);


	printf("command was cd\n");
	printf("Pathname: %s\n", pathname);
	printf("dirname: %s\n", dirname);
	printf("basename: %s\n", basename);
	if (tempnode == NULL)
	{
		if (flag == 1)
		{
			tempnode = cwd;
			tempnode = tempnode->childPtr;
			while (tempnode != NULL)
			{
				if (strcmp(tempnode->name, basenametemp) == 0)
				{
					cwd = tempnode;
					return;
				}
				tempnode = tempnode->siblingPtr;
			}	
		}
		printf("Not a valid direcotry\n");
		return;
	}
	tempnode = tempnode->childPtr;
	if (tempnode == NULL)
	{
		printf("Not a valid direcotry\n");
		return;
	}
	if (strcmp(tempnode->name, basename) == 0)
	{
		cwd = tempnode;
		return;
	}
	else
	{
		while (strcmp(tempnode->name, basename) != 0 && tempnode->siblingPtr != NULL)
		{
			tempnode = tempnode->siblingPtr;
			if (strcmp(tempnode->name, basename) == 0)
			{
				cwd = tempnode;
				return;
			}
		}
	}

	printf("Directory not found!\n");
}

void ls()
{
	Node *temp = malloc(sizeof(Node));
	printf("command was ls\n");
	temp = cwd;
	temp = temp->childPtr;
	while (temp != NULL)
	{
		printf(temp->name);
		printf("\n");
		temp = temp->siblingPtr;
	}

}

void pwd()
{
	Node *temp = malloc(sizeof(Node));
	char string1[128], string2[128] = "\0";
	printf("The command was pwd\n");
	temp = cwd;
	while (temp->parentPtr != NULL)
	{
		strcpy(string1, temp->name);
		strcat(string1, "/");
		strcat(string1, string2);
		strcpy(string2, string1);
		temp = temp->parentPtr;
	}
	strcpy(string1, temp->name);
	strcat(string1, string2);
	strcpy(string2, string1);
	printf("%s", string1);
	printf("\n");
}

char* rpwd()
{
	Node *temp = malloc(sizeof(Node));
	Node *temp2 = malloc(sizeof(Node));
	int flag = 0;
	char string1[128], string2[128] = "\0";
	temp = cwd;
	temp2 = cwd;
	while (temp->parentPtr != NULL)
	{
		strcpy(string1, temp->name);
		if (flag == 1)
		{
			strcat(string1, "/");
		}
		strcat(string1, string2);
		strcpy(string2, string1);
		temp = temp->parentPtr;
		flag = 1;
	}
	strcpy(string1, temp->name);
	strcat(string1, string2);
	strcpy(string2, string1);
	if (temp2->type == 'D')
	{
		strcpy(string2, "D");
		strcat(string2, "  ");
		strcat(string2, string1);
		strcpy(string1, string2);
	}
	else
	{
		strcpy(string2, "F");
		strcat(string2, "  ");
		strcat(string2, string1);
		strcpy(string1, string2);
	}
	printf("%s", string1);
	printf("\n");
	return string1;
}

void creat()
{
	int absolute = -1;
	Node *tempnode = malloc(sizeof(Node));
	printf("command was creat\n");
	printf("Pathname: %s\n", pathname);
	absolute = absolute_check();
	dirSetup(absolute);
	printf("dirname: %s\n", dirname);
	printf("basename: %s\n", basename);
	tempnode = search_dir(absolute);
	if (tempnode == NULL)
		printf("The given directory does not exist!\n");
	else
	{
		addDir(tempnode, 'F');
	}
}

void rm()
{
	int absolute = -1;
	Node *tempnode = malloc(sizeof(Node));
	printf("command was rmdir\n");
	printf("Pathname: %s\n", pathname);
	absolute = absolute_check();
	dirSetup(absolute);
	printf("dirname: %s\n", dirname);
	printf("basename: %s\n", basename);
	tempnode = search_dir(absolute);
	if (tempnode == NULL)
		printf("The given directory does not exist!\n");
	else
	{
		removeDir(tempnode, 'F');
	}
}

void save()
{
	Node *tempnode = malloc(sizeof(Node));
	FILE *f = fopen(pathname, "w+");
	char string1[128];
	int flag = 0;
	if (strcmp(pathname, "") == 0) //Checks for a file name
		printf("Must give a file name.\n");
	if (f == NULL) //Makes sure the file can be opened
	{
		printf("Error opening file!\n");
		return;
	}
	tempnode = cwd;
	cwd = root;
	while (cwd != NULL) //This loop writes everything needed to the file
	{
		if (cwd->childPtr != NULL && flag == 0)
		{
			strcpy(string1, rpwd());
			fprintf(f, "%s\n", string1);
			cwd = cwd->childPtr;
		}
		else if (cwd->siblingPtr != NULL)
		{
			if (flag == 0)
			{
				strcpy(string1, rpwd());
				fprintf(f, "%s\n", string1);
			}
			cwd = cwd->siblingPtr;
			flag = 0;
		}
		else
		{
			if (flag == 0)
			{
				strcpy(string1, rpwd());
				fprintf(f, "%s\n", string1);
			}
			cwd = cwd->parentPtr;
			flag = 1;
		}
	}
	fclose(f);
	cwd = tempnode;
}

void reload()
{
	FILE *f = fopen(pathname, "r");
	char string1[128];
	int flag = 0;
	if (strcmp(pathname, "") == 0) //Checks for a file name
		printf("Must give a file name.\n");
	if (f == NULL) //Makes sure the file can be opened
	{
		printf("Error opening file!\n");
		return;
	}
	while (!feof(f))
	{
		memset(line, 0, strlen(line));
		memset(command, 0, strlen(command));
		memset(pathname, 0, strlen(pathname));
		memset(dirname, 0, strlen(dirname));
		memset(basename, 0, strlen(basename));
		fscanf(f, "%s", string1);
		if (strcmp(string1, "") == 0)
			return;
		if (strcmp(string1, "D") == 0)
		{
			fscanf(f, "%s", string1);
			if (strcmp(string1, "/") != 0)
			{
				strcpy(pathname, string1);
				mkdir();
			}
		}
		else if (strcmp(string1, "F") == 0)
		{
			fscanf(f, "%s", string1);
			strcpy(pathname, string1);
			creat();
		}
		printf("\n");
	}

	fclose(f);
}

void quit()
{
	run = 0;
}

main()
{
	int ID = -1;
	int num = 0;
	int i = 0;
	initialize();      /* initialize the / DIR of the tree */
	//init_tree();
	while (run)
	{
		i = 0;
		//These lines clear the globals
		memset(line, 0, strlen(line));
		memset(command, 0, strlen(command));
		memset(pathname, 0, strlen(pathname));
		memset(dirname, 0, strlen(dirname));
		memset(basename, 0, strlen(basename));

		printf("input a commad : "); //Get command from user 
		fgets(line, sizeof line, stdin); //Store it in global line

		//This line parses the input to set up command and pathname
		while (line[i])
		{
			//We want to ignore the newline characters that fgets gives
			if (line[i] == '\n')
				i++;
			if (line[i] != ' ')
				command[i] = line[i];
			else
			{
				num = i + 1;
				i++;
				while (line[i] != '\0')
				{
					if (line[i] == '\n')
						i++;
					else
					{
						pathname[i - num] = line[i];
						i++;
					}
				}
				break;
			}
			i++;
		}

		//Find which command was given, and run that command
		ID = findCommand(command);
		switch (ID){
		case 0: menu();     break;
		case 1: mkdir();    break;
		case 2: rmdir();    break;
		case 3: cd();       break;
		case 4: ls();       break;
		case 5: pwd();      break;
		case 6: creat();    break;
		case 7: rm();       break;
		case 8: save();     break;
		case 9: reload();   break;
		case 10: quit();    break;
		case -1: printf("Invalid input!\n"); break;
		}
	}

	printf("Goodbye!\n");
}
