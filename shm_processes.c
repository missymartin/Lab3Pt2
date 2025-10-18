#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ChildProcess(int []); //child process

int  main(int  argc, char *argv[]) // parent process
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;
     int looptyloops = 25;

     /*
     if (argc != 5) {
          printf("Use: %s #1 #2 #3 #4\n", argv[0]);
          exit(1);
     }
     */
     ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (parent) ***\n");
          exit(1);
     }
     printf("Dad has received a shared memory of two integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (parent) ***\n");
          exit(1);
     }
     printf("Parent has attached the shared memory...\n");

     ShmPTR[0] = 0;
     ShmPTR[1] = 0;
     //ShmPTR[2] = atoi(argv[3]);
     //ShmPTR[3] = atoi(argv[4]);
     printf("Parent has filled instialized Bank Account ($%d) and Turn (%d) in shared memory...\n",
            ShmPTR[0], ShmPTR[1]/*, ShmPTR[2], ShmPTR[3]*/);

     printf("Server is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (parent) ***\n");
          exit(1);
     }
     else if (pid == 0) {
          ChildProcess(ShmPTR);
          exit(0);
     }
     // dear old dad
     int i;
     for (i= 0; i < looptyloops; i++) {
          // step 1
          sleep(rand() %6);
          //step 2
          int account = ShmPTR[0];
          //step 3
          while(ShmPTR[1]!=0);

          if(account <= 100){
               int balance = (rand() %100) + 1;
               if (balance % 2 == 0){
                    account += balance;
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);}
               else{
                    printf("Dear old Dad: Doesn't have any money to give\n");
               }
          }
          else{
               printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
          }
          //step3
          while(ShmPTR[1] != 0); //busy wait
          //step 4 
          ShmPTR[0] = account;
          //step 5
          ShmPTR[1] = 1;
     }


     wait(&status);
     printf("Parent has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Parent has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Parent has removed its shared memory...\n");
     printf("Parent exits...\n");
     exit(0);
}

void  ChildProcess(int  SharedMem[])
{
     int loopthat = 25;

     //printf("   Client process started\n");
     int i;
     for (i = 0; i<loopthat; i++){
          //step1
          sleep(rand()%6);
          //step 2
          int account = SharedMem[0];
          //step3
          while(SharedMem[1] !=1);
          //step 4
          int needmoney = (rand()%50) + 1;
          printf("Poor Student needs $%d\n", needmoney);

          if(needmoney <= account){
               account -= needmoney;
               printf("Poor Student: Withdraws $%d / Balance = $%d\n", needmoney, account);
          }
          else{
              printf("Poor Student: Not Enough Cash ($%d)\n", account ); 
          }
          SharedMem[0] = account;
          SharedMem[1]= 0;

     }
     //printf("   Client found %d %d %d %d in shared memory\n",
                //SharedMem[0], SharedMem[1], SharedMem[2], SharedMem[3]);
     printf("   Child is about to exit\n");
}