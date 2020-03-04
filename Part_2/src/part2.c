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

int main(int argc, char* argv[])
{
    int fd[2];
    int a[2];
    int b[2];
    int c[2];
    pid_t forkid;
    pid_t forkid1;
    pid_t forkid2;
    pid_t forkid3;
    pid_t forkid4;
    int fdfile;
    if(((strcmp(argv[1],"@"))==0)&&(argc==4))
    //we need to use one pipe to do this task: grep -rF Kanpur[e.g.] IITK[e.g.] | wc -l
    {
        if (pipe(fd) == -1)//create pipe
        {
             perror("fd");
             exit(EXIT_FAILURE);
        }
        forkid = fork();//forked the process
        if (forkid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if((forkid)==0)//child
        {
            close(1);//close STDOUT
            dup(fd[1]);
            //output of grep will go into pipe and not stdout
            close(fd[0]);
            close(fd[1]);
            execl("/bin/grep","grep","-rF",argv[2],argv[3],(char*)NULL);
            perror("execl");
        }
        else//parent
        {
            wait(NULL);//wait for child to complete, remove any uncertainty
            //so child has put in the grep output into the pipe, now we read it
            close(0);//close STDIN
            dup(fd[0]);
            //wc will read input from pipe not stdin
            close(fd[1]);
            close(fd[0]);
            execl("/usr/bin/wc","wc","-l",(char*)NULL);
            perror("execl");
        }
    }
    else if((strcmp(argv[1],"$"))==0)
    {
    //we need to use two pipes to do this task: [e.g.] grep -rF Kanpur IITK |tee output.txt | wc -l
    //here we will fork 2 times.
    //here we will have total 3 processes, lets call them p,q,r
    /* IMPLEMENATION: p will write grep output in pipe a, q will read from pipe a, then q will write this
    to file and write to pipe b, r will read from pipe b and execute the given command and output to STDOUT*/
        if (pipe(a) == -1)//create pipe a
        {
             perror("a");
             exit(EXIT_FAILURE);
        }
        if (pipe(b) == -1)//create pipe b
        {
             perror("b");
             exit(EXIT_FAILURE);
        }
        fdfile = open(argv[4],O_CREAT|O_WRONLY|O_TRUNC);
        forkid1 = fork();//forked first time
        if (forkid1 == -1)
        {
            perror("fork1");
            exit(EXIT_FAILURE);
        }
        else if((forkid1)==0)//child 1
        {
            close(1);//close STDOUT
            dup(a[1]);
            //output of grep will go into pipe a and not stdout
            close(a[0]);
            close(a[1]);
            close(b[0]);
            close(b[1]);
            execl("/bin/grep","grep","-rF",argv[2],argv[3],(char*)NULL);
            perror("execl");
        }
        else//parent
        {
            wait(NULL);
            forkid2 = fork();//forked second time
            if (forkid2 == -1)
            {
                perror("fork2");
                exit(EXIT_FAILURE);
            }
            else if(forkid2 == 0)//child 2
            {
                //tee implementation done by this child process
                close(0);//close STDIN
                dup(a[0]);//read from pipe a
                close(1);//close STDOUT
                dup(b[1]);//write to pipe b
                close(a[0]);
                close(a[1]);
                close(b[0]);
                close(b[1]);
                char *c = (char *)calloc(100000,sizeof(char));//buffer
                while((read(0,c,1))>0)//reading done from pipe a, one character at a time
                {
                    write(fdfile,c,strlen(c)); //writing to given file
                    write(1,c,strlen(c)); // writing to pipe b
                }
                return 0;
            }
            else//parent
            {
                close(0);//close STDIN
                dup(b[0]);//read from pipe b
                close(a[0]);
                close(a[1]);
                close(b[0]);
                close(b[1]);
                char **arguments;
                arguments = (char **)calloc(10000,sizeof(char *));
                int j=0;
                for(int i=5; i<argc; i++)//command arguments start from argv[5]
                //doing this because there might be commands with multiple space separated strings
                {
                    arguments[j] = argv[i];
                    j++;
                }
                arguments[j+1] = (char*)NULL;//last argument must be NULL
                execvp(arguments[0],arguments);
                perror("execvp");
            }
        }
        close(fdfile);
    }
    return 0;
}
