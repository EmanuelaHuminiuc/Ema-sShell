#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#define MAXLINE 128

char **splitCmmd(char *s,char delim1,char delim2,int *arguments) {
  int i,a,length;
  int weAreInWord;   
  char **args;
  weAreInWord=0;
  length=strlen(s); 
  a=0;
  args=(char **)malloc(sizeof(char*));  
  for(i=0; i<=length; i++)
  {
      if(s[i]!=delim1 && s[i]!=delim2 && s[i]!=0)
      {
        if(!weAreInWord)
        {
          args[a++]=&s[i];
          args=(char **)realloc(args,(a+1)*sizeof(char*));  
        }
        weAreInWord=1;
      }
      else
      {
        if(weAreInWord)
          s[i]=0;
        weAreInWord=0;
      }
  }
  args[a]=NULL; 
  *arguments=a;
  return args;
}


char *deleteWhitespace(char *s) {
  char *p;
 
  while(s[strlen(s)-1]==' ' || s[strlen(s)-1]=='\t')
    s[strlen(s)-1]=0;
  p=s;
  while(*p && (*p==' ' || *p=='\t'))
    p++;
  return p;
}


char *readLineFromUser() {
    int a,i;
    char o,*buffer;
    i=0;
    buffer=(char*)malloc(512);
    while(i<512)    
    {
        a=read(STDIN_FILENO,&o,1);  
        if(a==1 && o!='\n')
          buffer[i++]=o;
        else if(a<=0)
        {
          free(buffer);
          return NULL;
        }
        else
          break;
    }
    buffer[i]=0;  
    return buffer;
}


char readCharFromUser() {
    int m;
    char n,o;
    o=0;
    while(1)   
    {
        m=read(STDIN_FILENO,&n,1); 
        if(m==1 && n!='\n')
        {
          if(n==0)
            o=n;
        }
        else if(m<=0)
          return 0;
        else
          break;
    }
    return o;
}


void printStrings(char *s) {
  int i;
  for(i=0; i<strlen(s); i++)
    write(STDOUT_FILENO,&s[i],1); 
}

void myVersionCmmd() {
  printf("Author name: ");
  printf("EMANUELA - DENISA HUMINIUC\n");
  printf("Author studies: ");
  printf("Computer Science in English, Second Year\n");
  printf("Version: ");
  printf("1.0 \n");
  printf("\n");
  exit(EXIT_SUCCESS);

}


void myHelpCmmd() {
  printf("\nAvailable implemented commands:\n\n");
  printf("help - Displays the implemented commands\n");
  printf("version - Displays some informations about the project & the author\n");
  printf("exit - Exits the terminal\n");
  printf("mv + the parameters:[ -i, -t, -S (with -b)] - Moves a file in a new location\n");
  printf("cp - Copies files or group of files or directories\n");
  printf("dirname - Removes the trailing forward slashes '/' from the NAME and prints the remaining portion. If the argument NAME does not contain the forward-slash '/' then it simply prints dot '.'\n");
  printf("\n");
  exit(EXIT_SUCCESS);
}


char *getMyFileName(char *path) {
  char *p;
  p=&path[strlen(path)-1];  
  while(p!=path && *p!='/')  
    p--;
  if(*p=='/')
    p++;
  return p;
}


void myRedirectionCmmd(char *s,char **myRedirIn,char **myRedirOut) {
  int length;
  int i;
  int myRedir;
  length=strlen(s);
  myRedir=0;
  *myRedirIn=NULL;
  *myRedirOut=NULL;
  for(i=0; i<length; i++)
  {
    if(s[i]=='>' || s[i]=='<')
    {       
      myRedir=s[i];
      s[i]=0;
    }
    else if(myRedir)
    {
      if(myRedir=='>')
        *myRedirOut=&s[i];
      else 
        *myRedirIn=&s[i];
      myRedir=0;
    }
  }
  if(*myRedirIn)
    *myRedirIn=deleteWhitespace(*myRedirIn);
  if(*myRedirOut)
    *myRedirOut=deleteWhitespace(*myRedirOut);
}

char *myDirnameCmmd(char *path) {
    static const char myDot[] = ".";
    char *theLastSlashInPath;
    theLastSlashInPath = path != NULL ? strrchr (path, '/') : NULL;
    if(theLastSlashInPath == path)
    {
        ++theLastSlashInPath;
    }
    else if(theLastSlashInPath != NULL && theLastSlashInPath[1] == '\0')
    {
        theLastSlashInPath = memchr(path, theLastSlashInPath-path, '-');
    }
    if(theLastSlashInPath != NULL)
    {
        theLastSlashInPath[0] = '\0';
    }
    else
    {
        path = (char *) myDot;
    }
    printf("%s \n", path);
    return path;

}


int myMvParameters(char **args,int arguments,int *activeVar,char *final,char *exVar) {
  int i;
  *activeVar=0;
  final[0]=0;
  exVar[0]=0;
  for(i=1; i<arguments; i++)
  {
    if(!strcmp(args[i],"-i")) 
    {
      *activeVar=1;
      args[i][0]=0;
    } 
    else if(!strcmp(args[i],"-t")) 
    {
      if(i+1>=arguments)
      {
        fprintf(stderr,"mv command: missing directory name for -t option.\n");
        return 0;
      }      
      strcpy(final,args[i+1]);
      args[i][0]=0;
      args[i+1][0]=0;
      i++;
    } 
    else if(!strcmp(args[i],"-S"))
    {
      if(i+1>=arguments)
      {
        fprintf(stderr,"mv command: missing suffix for -S option.\n");
        return 0;
      }      
      strcpy(exVar,args[i+1]);
      args[i][0]=0;
      args[i+1][0]=0;
      i++; 
    }
  }
  return 1;
}

void myMvDirectory(char *,char *,int ,char *);


void myMvFile(char *init,char *final,int activeVar,char *exVar) {
  char buffer[512];
  struct stat entryStat,initStat;  
  char o;
  
	if (stat(init,&initStat)==-1)
	{
    fprintf(stderr,"mv command: not such file or directory '%s'\n",init);
    return;
	}
  if(S_ISDIR(initStat.st_mode)) 
  {
    myMvDirectory(init,final,activeVar,exVar);  
    return;
  }
	if (stat(final,&entryStat)!=-1)
  {
    if(activeVar)
    {
      printStrings("Overwrite file '");
      printStrings(final);
      printStrings("'? (y/n) ");
      o=readCharFromUser();
      if(o!='y' && o!='Y')
        return;
    }
    if(exVar[0])
    {
      sprintf(buffer,"%s%s",final,exVar);   

    	if (stat(buffer,&entryStat)!=-1)
    	{
        if(unlink(buffer)==-1)
        {
          fprintf(stderr,"mv command: unable to remove file '%s'\n",buffer);
          return;
        }
      }
      if(link(final,buffer)==-1)      {
        fprintf(stderr,"mv command: unable to create backup '%s'\n",buffer);
        return;
      }
    }
    if(unlink(final)==-1)
    {
      fprintf(stderr,"mv command: unable to overwrite file '%s'\n",final);
      return;
    }
  }  
  if(link(init,final)==-1)
    fprintf(stderr,"mv command: unable to move file '%s' to '%s'\n",init,final);
  else if(unlink(init)==-1)
    fprintf(stderr,"mv command: unable to remove file '%s'\n",init);
}


void myMvDirectory(char *init,char *final,int activeVar,char *exVar) {
  char initpath[512],dstpath[512];
  struct stat entryStat,initStat;  
  DIR * dir;
  struct dirent* dir_entry;
  char o;
  
	if (stat(init,&initStat)==-1)
	{
    fprintf(stderr,"mv command: not such file or directory '%s'\n",init);
    return;
	}
  
	if (stat(final,&entryStat)!=-1)
  {
    if(activeVar)
    {
      printStrings("Overwrite '");
      printStrings(final);
      printStrings("'? (y/n) ");
      o=readCharFromUser();
      if(o!='y' && o!='Y')
        return;
    }
    if(rmdir(final)==-1)
    {
      fprintf(stderr,"mv command: directory '%s' is not empty\n",final);
      return;
    }
  }  	
  if(mkdir(final,initStat.st_mode)==-1)
    fprintf(stderr,"mv command: unable to move file '%s' to '%s'\n",init,final);
  else
  {
    dir = opendir(init); 
    if(dir==NULL)
      fprintf(stderr,"mv command: unable to open directory '%s'\n",init);
    else
    {
	    while((dir_entry=readdir(dir))!=NULL) 
		    if(strcmp(dir_entry->d_name,".") && strcmp(dir_entry->d_name,"..")) 
		    {
          sprintf(initpath,"%s/%s",init,dir_entry->d_name);
          sprintf(dstpath,"%s/%s",final,dir_entry->d_name);
          myMvFile(initpath,dstpath,activeVar,exVar);
        }
      if(rmdir(init)==-1)
        fprintf(stderr,"mv command: directory '%s' is not empty\n",init);
    }
  }
}



void myMvCmmd(char **args,int arguments) {
  struct stat entryStat;  
  char final[100];
  char exVar[100];
  char path[512];
  int activeVar;
  char **files,*myFileName;
  int i,a;

  if(!myMvParameters(args,arguments,&activeVar,final,exVar))
    exit(EXIT_FAILURE);
  a=0;
  files=(char**)malloc(arguments*sizeof(char*));
  for(i=1; i<arguments; i++)
  {
    if(args[i][0]!=0)
      files[a++]=args[i];
  }
  if(final[0])
  {
    if(a<1)
    {
      fprintf(stderr,"mv command: not enough parameters\n");
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    if(a<2)
    {
      fprintf(stderr,"mv command: not enough parameters\n");
      exit(EXIT_FAILURE);
    }
  }
 
  if(final[0]) 
  {
	  if(stat(final, &entryStat)==-1)
	    fprintf(stderr,"mv command: unable to open directory '%s'\n",final);
    else if(S_ISDIR(entryStat.st_mode)) 
    {
      for(i=0; i<a; i++)
      {
        myFileName=getMyFileName(files[i]);  
        sprintf(path,"%s/%s",final,myFileName);
        myMvFile(files[i],path,activeVar,exVar);
      }
    }
    else
      fprintf(stderr,"mv command: the destination '%s' is not a directory\n",final);
  }
  else if(a>2)
  {
	  if(stat(files[a-1], &entryStat)==-1)
	    fprintf(stderr,"mv command: unable to open directory '%s'\n",final);
    else if(S_ISDIR(entryStat.st_mode)) 
    {
      for(i=0; i<a-1; i++)
      {
        myFileName=getMyFileName(files[i]);  
        sprintf(path,"%s/%s",files[a-1],myFileName);
        myMvFile(files[i],path,activeVar,exVar);
      }
    }
    else
      fprintf(stderr,"mv command: the destination '%s' is not a directory\n",final);
  }
  else
  {
	  if(stat(files[1], &entryStat)==-1) 
      myMvFile(files[0],files[1],activeVar,exVar);
    else if(S_ISDIR(entryStat.st_mode)) 
    {
      myFileName=getMyFileName(files[0]); 
      sprintf(path,"%s/%s",files[1],myFileName);
      myMvFile(files[0],path,activeVar,exVar);
    }
    else if(stat(files[0], &entryStat)!=-1) 
    {
      if(!S_ISDIR(entryStat.st_mode)) 
        myMvFile(files[0],files[1],activeVar,exVar);
      else
        fprintf(stderr,"mv command: the destination '%s' is not a directory\n",files[1]);
    }
    else
      fprintf(stderr,"mv command: not such file or directory '%s'\n",files[0]);
  }
  exit(EXIT_SUCCESS);
}

int cmmdLength(char** cmmd) {
	int count = 0;
	while (cmmd[count] != NULL)
		count++;
	return count;
}

char buffer[MAXLINE];

int myCpParameters(char **args,int arguments,int *activeVar,char *final,char *exVar) {
  int i;
  *activeVar=0;
  final[0]=0;
  exVar[0]=0;
  char o;
  for(i=1; i<arguments; i++)
  {
    if(!strcmp(args[i],"-i"))
    {
          if(activeVar)
    {
	      printStrings("Overwrite file '");
	      printStrings(args[3]);
	      printStrings("'? (y/n) ");
	      o=readCharFromUser();
	      if(o!='y' && o!='Y')
		return 0;
    }
      *activeVar=1;
      args[i][0]=0;

    } 
    else if(!strcmp(args[i],"-t"))
    {
      if(i+1>=arguments)
      {
        fprintf(stderr,"cp command: missing directory name for -t option.\n");
        return 0;
      }      
      strcpy(final,args[i+1]);
      args[i][0]=0;
      args[i+1][0]=0;
      i++; 
    } 
  }
  return 1;
}

void myCpCmmd(char** cmmd) {
	int arguments = cmmdLength(cmmd);
	char **args = cmmd;
	int file1, file2, count;
	char final[100];
	char exVar[100];
	char path[512];
	int activeVar;
    if(!myCpParameters(args,arguments,&activeVar,final,exVar))
    	exit(EXIT_FAILURE);
    if (arguments != 3) {
   		printf("cp command: number of arguments is invalid\n");
     		return;
    }

	if (!access(args[1], F_OK)) {
		file1 = open(args[1], O_RDONLY);
		if (file1 > 0) {
			file2 = open(args[2], O_CREAT | O_WRONLY, 0777);
			if (file2 > 0) {
				while ((count = read(file1, buffer, sizeof(buffer))) > 0)
					write(file2, buffer, count);
				close(file2);
				close(file1);
			}
			else {
				perror("open");
			}
		}
		else {
			perror("open");
		}
	}
	else {
		perror("access");
	}

}

int myShell(char **args,int arguments,int myPipeInput,int myPipeOutput) {
  pid_t myPipeId;
  int status;

  if(!strcmp(args[0],"exit"))
    return 1;
  myPipeId=fork(); 
  if(myPipeId<0)
  {
    fprintf(stderr,"error: fork failed\n");
    exit(EXIT_FAILURE);
  }
  else if(myPipeId==0) 
  {
    if(myPipeInput!=-1)
    {
      dup2(myPipeInput,STDIN_FILENO);
      close(myPipeInput);      
    }
    if(myPipeOutput!=-1)
    {
      dup2(myPipeOutput,STDOUT_FILENO);
      close(myPipeOutput);
    }
    if(!strcmp(args[0],"help"))
      myHelpCmmd(args,arguments);
    else if(!strcmp(args[0],"version"))
      myVersionCmmd(args,arguments);
    else if(!strcmp(args[0],"dirname"))
      myDirnameCmmd(args[1]);
    else if(!strcmp(args[0],"mv"))
      myMvCmmd(args,arguments);
    else if(!strcmp(args[0],"cp"))
      myCpCmmd(args);
    else if(execvp(args[0],args)==-1)
    {
      fprintf(stderr,"error: invalid command: %s\n",args[0]);
      exit(EXIT_FAILURE);
    }
  }
  wait(&status);
  return 0;
}

int myShellCmmd(char *cmmd,int myPipeInput,int myPipeOutput) {
  char **args;
  char *infile,*outfile;
  int arguments;
  int Exit;
  Exit=0;
  myRedirectionCmmd(cmmd,&infile,&outfile);
  if(infile!=NULL)
  {
    if(myPipeInput!=-1)
      close(myPipeInput);
    myPipeInput=open(infile,O_RDONLY);
  }   
  if(outfile!=NULL)
  {
    if(myPipeOutput!=-1)
      close(myPipeOutput);
    myPipeOutput=open(outfile,O_WRONLY | O_CREAT | O_TRUNC,0666);
  }
  args=splitCmmd(cmmd,' ','\t',&arguments); 
  if(arguments>0)
      Exit=myShell(args,arguments,myPipeInput,myPipeOutput);
  if(myPipeInput!=-1)
    close(myPipeInput);
  if(myPipeOutput!=-1)
    close(myPipeOutput);
  free(args);
  return Exit;
}


int myShellCmmdLine(char *cmmdLine) {
  char **args;
  int arguments;
  int i;
  int Exit;
  int **pipes;
  Exit=0;
  args=splitCmmd(cmmdLine,'|','|',&arguments); 
  if(arguments>0)
  {
    if(arguments==1)  
      Exit=myShellCmmd(args[0],-1,-1);
    else
    {
      pipes = (int**)malloc((arguments-1)*sizeof(int*));
      for(i=0; i<arguments-1; i++)
      {
        pipes[i]=(int*)malloc(2*sizeof(int));
        pipe(pipes[i]);
      }
      for(i=0; i<arguments && !Exit; i++)
      {       
          if(i==0)
            Exit=myShellCmmd(args[i],-1,pipes[i][1]);
          else if(i==arguments-1)
            Exit=myShellCmmd(args[i],pipes[i-1][0],-1);
          else
            Exit=myShellCmmd(args[i],pipes[i-1][0],pipes[i][1]);
      } 
      for(i=0; i<arguments-1; i++)
        free(pipes[i]);
      free(pipes);
    }
  } 
  free(args);
  return Exit;  
}


int main(int argc,char **argv) {
    int closeMyTerminal=0;
    char *cmmdLine;
    printf("Input your desired command or type 'exit'.\n");
    while(!closeMyTerminal)   
    {
        printStrings(">_ ");  
        if((cmmdLine=readLineFromUser())!=NULL) 
        {
          closeMyTerminal=myShellCmmdLine(cmmdLine);
          free(cmmdLine); 
        }
        else 
          closeMyTerminal=1;
    }
    return (0);
}
