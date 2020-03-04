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
    struct dirent *de;
    chdir(namedir);
    int var2=0;
    DIR *dr = opendir(".");
    if(dr==NULL)
    {
        printf("could not open current");
        return 0;
    }
    struct stat sb;
    char *path2 =(char *)calloc(10000,sizeof(char));
    while((de=readdir(dr))!=NULL)
    {
        stat(de->d_name, &sb);
        if(de->d_type == DT_DIR)
        {
            if((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0))
            {
                var2 = cal(de->d_name,0);
                var = var + var2;
            }
        }
        else if (de->d_type == DT_REG)
        {
            var = var + sb.st_size;
        }
    }
    chdir("..");
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
        printf("could not open current");
        return 0;
    }
    pid_t forkid2;
    struct stat sb;
    int fd2[2];
    if (pipe(fd2) == -1)
    {
         perror("fd2");
         exit(EXIT_FAILURE);
    }
    forkid2 = fork();
    if (forkid2 == -1)
    {
        perror("fork2");
        exit(EXIT_FAILURE);
    }
    else if((forkid2)==0)
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
        for(int i=strlen(path3)-1;i>=0;i--)
        {
            if(path3[i]== '/')
            {
                flag=1;
                wh = i;
                break;
            }
        }
        if(flag==1)
        {
            int j=0;
            for(int i=wh+1;i<=strlen(path3)-1;i++)
            {
                path4[j] = path3[i];
                j++;
            }
        }
        else
        {
            strcpy(path4,path3);
        }
        close(1);
        dup(fd2[1]);
        close(fd2[0]);
        close(fd2[1]);
        printf("%s ",path4);
        return 0;
    }
    else
    {
        wait(NULL);
    }
    char *path2 =(char *)calloc(10000,sizeof(char));;
    while((de=readdir(dr))!=NULL)
    {
        //printf("%d--%s\n", de->d_type,de->d_name);
        stat(de->d_name, &sb);
        if(de->d_type == DT_DIR)
        {
            if((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0))
            {
                //printf("%s %d\n",de->d_name,x);
                forkid2 = fork();
                if (forkid2 == -1)
                {
                    perror("fork2");
                    exit(EXIT_FAILURE);
                }
                else if((forkid2)==0)
                {
                    x = cal(de->d_name,0);
                     close(fd2[0]);
                    // printf("%d ",x);
                    close(1);
                    dup(fd2[1]);
                    close(fd2[1]);
                    printf("%s %d ",de->d_name,x);
                    return 0;
                }
                else
                {
                    wait(NULL);
                }
            }
        }
        else if (de->d_type == DT_REG)
        {
            ans = ans + sb.st_size;
        }
    }
    //only initial parent reaches here
    close(fd2[1]);
    char *in2 = (char*)calloc(100000,sizeof(char));
    char *t = (char*)calloc(100000,sizeof(char));
    close(0);
    dup(fd2[0]);
    close(fd2[0]);
    scanf("%s ",t);
    while(scanf("%s %d ",in2 ,&x)!=EOF)
    {
        printf("%s %d\n",in2,x);
        ans = ans + x;
    }
    printf("%s %d\n",t,ans);
    closedir(dr);
    return 0;
}
