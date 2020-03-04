//170192
//Ayush Hitesh Soneria

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int cal(char *namedir,int var)
{
/*this is the recursive function which looks at the sizes of all the files and sub directories*/
//var will be returned as total size of sub-directory namedir
    struct dirent *de;
    chdir(namedir);
    int var2=0;
    DIR *dr = opendir(".");
    if(dr==NULL)
    {
        printf("could not open current directory");
        return 0;
    }
    struct stat sb;
    char *path2 =(char *)calloc(10000,sizeof(char));
    while((de=readdir(dr))!=NULL)
    {
        stat(de->d_name, &sb);
        if(de->d_type == DT_DIR)//checks to see if Directory
        {
            if((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0))//ignore these directories
            {
                var2 = cal(de->d_name,0); //to compute total size of this sub-directory
                var = var + var2;
            }
        }
        else if (de->d_type == DT_REG)//checks to see if regular file
        {
            var = var + sb.st_size;
        }
    }
    chdir("..");//when returning we need to switch directory command back to its previous directory
    closedir(dr);
    return var;
}

int main(int argc, char *argv[])
{
    struct dirent *de;
    chdir(argv[1]);
    int ans=0;
    int x=0;
    DIR *dr = opendir(".");
    if(dr==NULL)
    {
        printf("could not open current Directory");
        return 0;
    }
    pid_t forkid2;
    struct stat sb;
    int fd2[2];
    if (pipe(fd2) == -1)//makes one pipe
    {
         perror("fd2");
         exit(EXIT_FAILURE);
    }
    forkid2 = fork();
    /* this fork is done to send the name of this directory into pipe*/
    if (forkid2 == -1)
    {
        perror("fork2");
        exit(EXIT_FAILURE);
    }
    else if((forkid2)==0)//child
    {
        x = cal(de->d_name,0);
        //here we need to only send name of directory and not entire path
        char *path3 =(char *)calloc(10000,sizeof(char));
        char *path4 =(char *)calloc(10000,sizeof(char));
        strcpy(path3,argv[1]);
        if(path3[strlen(path3)-1]=='/')/*if last char of path is '/' then we remove it */
            path3[strlen(path3)-1] = '\0';
        int flag=0;
        int wh = 0;
        for(int i=strlen(path3)-1;i>=0;i--)//we need to find first occurence of / from behind
        {
            if(path3[i]== '/')//path given contains /
            {
                flag=1;
                wh = i;
                break;
            }
        }
        if(flag==1)//path given contains /
        {
            int j=0;
            for(int i=wh+1;i<=strlen(path3)-1;i++)//needed part is only after the '/' entry
            {
                path4[j] = path3[i];
                j++;
            }
        }
        else//in this case the path given is the name of directory
        {
            strcpy(path4,path3);
        }
        close(1);
        dup(fd2[1]);
        close(fd2[0]);
        close(fd2[1]);
        printf("%s ",path4); //sent into pipe
        return 0;
    }
    else//parent
    {
        wait(NULL);
    }
    char *path2 =(char *)calloc(10000,sizeof(char));;
    while((de=readdir(dr))!=NULL)//now we go inside the given directory
    {
        stat(de->d_name, &sb);
        if(de->d_type == DT_DIR)//checks to see if Directory
        {
            if((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0))//ignore these directories
            {
                forkid2 = fork();//forking done for all immediate child subdirectories
                if (forkid2 == -1)
                {
                    perror("fork2");
                    exit(EXIT_FAILURE);
                }
                else if((forkid2)==0)//child
                {
                    x = cal(de->d_name,0);//recursive call made inside child of forked immediate subdirectory
                    close(fd2[0]);
                    close(1);
                    dup(fd2[1]);
                    close(fd2[1]);
                    //here we will insert the name of immediate subdirectory and its size into the pipe
                    printf("%s %d ",de->d_name,x);
                    return 0;
                }
                else//parent
                {
                    wait(NULL);
                }
            }
        }
        else if (de->d_type == DT_REG)//checks to see if regular file
        {
            ans = ans + sb.st_size;//ans will eventually contain total size of given directory
        }
    }
    //only initial parent reaches here
    close(fd2[1]);
    char *in2 = (char*)calloc(100000,sizeof(char));
    char *t = (char*)calloc(100000,sizeof(char));
    close(0);
    dup(fd2[0]);
    close(fd2[0]);
    //here we will read from pipe.
    //the sequence of reading will be FIFO, meaning what was put in first will be read first
    scanf("%s ",t); //recall the first thing we put into pipe: this will be the extracted name of directory from given path
    while(scanf("%s %d ",in2 ,&x)!=EOF)//now we will keep reading name of immediate subdirectory and its size from pipe
    {
        printf("%s %d\n",in2,x);//print to STDOUT
        ans = ans + x;//ans will have the total size of given directory
    }
    printf("%s %d\n",t,ans);//print to STDOUT
    closedir(dr);
    return 0;
}
