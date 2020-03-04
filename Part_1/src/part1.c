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

void dirtree(char *namedir,char *path, char *check)
{
    struct dirent *de;
    chdir(namedir);
    DIR *dr = opendir(".");
    if(dr==NULL)
    {
        printf("could not open current");
        return;
    }
    struct stat sb;
    //int fd[100];
    char *path2 =(char *)calloc(1000000,sizeof(char));
    char *path3 =(char *)calloc(1000000,sizeof(char));
    while((de=readdir(dr))!=NULL)
    {
        stat(de->d_name, &sb);
        if(de->d_type == DT_DIR)//to check type is of directory
        {
            if((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0))
            {
                strcpy(path2,path);
                if(path2[strlen(path2)-1]!='/')//if path last char is not '/' then add to path
                    strcat(path2,"/");
                strcat(path2,de->d_name);
                dirtree(de->d_name,path2,check);
             }
        }
        else if (de->d_type == DT_REG)//to check type is of file
        {
            strcpy(path2,path);
            if(path2[strlen(path2)-1]!='/')//if path last char is not '/' then add to path
                strcat(path2,"/");
            strcat(path2,de->d_name);
            int fd = open(de->d_name,O_RDONLY);
            if(fd>=0)
            {
               int si = (int)sb.st_size;
               char *c = (char *)calloc(si+1,sizeof(char));
               int arr = read(fd,c,si);
               c[arr] = '\0';
               int i=0;
               int j=0;
               char *ans;
               ans = (char *)calloc(100000,sizeof(char));
               while(c[i]!='\0')
               {
                   if(c[i]!='\n')
                   {
                       ans[j] = c[i];
                   }
                   else//here extract lines from file one-by-one in buffer ans.
                   {
                       if(strstr(ans,check)!=NULL)//this line should come in output as it contains the required string
                       {
                           //for the grep output format
                           strcpy(path3,path2);
                           strcat(path3,":");
                           strcat(path3,ans);
                           strcat(path3,"\n");
                           write(1,path3,strlen(path3));
                           //printf("%s:%s\n",path2,ans);
                       }
                       j=-1;//reinitialize ans
                       ans = (char *)calloc(100000,sizeof(char));
                   }
                   j++;
                   i++;
               }
            }
            close(fd);
        }
    }
    closedir(dr);
    chdir("..");
    return;
}

int main(int argc, char* argv[])
{
    struct stat sb;
    stat(argv[2], &sb);
    if((sb.st_mode & S_IFMT)==S_IFREG)
    /*this means the path given to us is of a file, so no recursive call made here,
    also no path printed in output just like grep -rF*/
    {
        char *check = argv[1];
        int fd = open(argv[2],O_RDONLY);
        if(fd>=0)
        {
           int si = (int)sb.st_size;
           char *c = (char *)calloc(si+1,sizeof(char));
           int arr = read(fd,c,si);
           c[arr] = '\0';
           int i=0;
           int j=0;
           char *ans;
           ans = (char *)calloc(100000,sizeof(char));
           while(c[i]!='\0')
           {
               if(c[i]!='\n')
               {
                   ans[j] = c[i];
               }
               else//here extract lines from file one-by-one in buffer ans.
               {
                   if(strstr(ans,check)!=NULL)//this line should come in output as it contains the required string
                   {
                       //for the grep output format
                       strcat(ans,"\n");
                       write(1,ans,strlen(ans));
                       //printf("%s\n",ans);
                   }
                   j=-1;//reinitialize ans
                   ans = (char *)calloc(100000,sizeof(char));
               }
               j++;
               i++;
           }
        }
        close(fd);
    }
    else
    /*this means the path given to us is of a directory, so recursive call is made here,
    also entire path printed in output just like grep -rF*/
    {
        dirtree(argv[2], argv[2], argv[1]);
    }
    return 0;
}
