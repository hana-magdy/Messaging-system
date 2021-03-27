#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


sem_t CounterLock,BufferFull,BufferEmpty,BufferLock;
int Counter;
int* Buffer;
int ReadIndex;
int WriteIndex;
int BufferSize=3;

void *CounterFunction(void* data){
  while(1){

   sleep(rand() % 20);
   int* ThreadId = malloc(sizeof(int*));
   ThreadId =(int*) data;
   printf("Counter Thread %d : received a message \n",ThreadId);
   printf("Counter Thread %d : waiting to write \n",ThreadId);

   //critical section
   sem_wait(&CounterLock);
   Counter++;
   printf("Counter Thread %d : now adding to counter, counter value= %d \n",ThreadId,Counter);
   sem_post(&CounterLock);
  }

}

void *MonitorFunction(){
  while(1){
     sleep(rand()%20);

     int testval,TempCount;
     sem_getvalue(&BufferEmpty, &testval);
     if(testval==0){
      printf("Monitor thread: BUFFER FULL \n");
      WriteIndex=ReadIndex;
     }

     else{

      printf("Monitor thread: waiting to read counter \n");
      sem_wait(&CounterLock);
      TempCount=Counter;
      printf("Monitor thread: reading a count value of: %d \n",Counter);
      Counter=0;
      sem_post(&CounterLock);

     }

     sem_wait(&BufferEmpty);
     sem_wait(&BufferLock);

     printf("Monitor thread: Writing to Buffer Position: %d \n",WriteIndex+1);
     Buffer[WriteIndex]=TempCount;
     WriteIndex++;

     sem_post(&BufferLock);
     sem_post(&BufferFull);

     if(WriteIndex==BufferSize)
       WriteIndex=0;

  }
}

void *CollectorFunction(){
   while(1){
     sleep(rand()%20);

     int testvalue;
     sem_getvalue(&BufferFull, &testvalue);
     if(testvalue==0){
      printf("Collector thread: nothing is in the buffer \n");
      WriteIndex=0;
      ReadIndex=0;
     }
     else{
     sem_wait(&BufferLock);
     sem_wait(&BufferFull);
     printf("Collector thread: reading from Buffer Position: %d \n",ReadIndex+1);
     Buffer[ReadIndex]=0000;
     ReadIndex++;
     sem_post(&BufferEmpty);
     sem_post(&BufferLock);

     }


     if(ReadIndex==BufferSize)
       ReadIndex=0;
    sem_getvalue(&BufferFull, &testvalue);
     if(testvalue==0){
      WriteIndex=0;
      ReadIndex=0;
     }



   }
}



int main()
{
    int ThreadNumber=5;
    //printf("Enter the number of threads \n");
    //scanf("%d",& ThreadNumber);
    //printf("Enter the size of buffer \n");
    //scanf("%d",& BufferSize);

    Buffer=malloc(sizeof(sizeof(int) * BufferSize));
    ReadIndex=0;
    WriteIndex=0;

    sem_init(&CounterLock,0,1);
    sem_init(&BufferEmpty,0,BufferSize);
    sem_init(&BufferFull,0,0);
    sem_init(&BufferLock,0,1);

    pthread_t CounterThreads[ThreadNumber];

    for(int i=0;i<ThreadNumber;i++){

      int* ThreadId =malloc(sizeof(int));
      ThreadId=i+1;
      pthread_create(&CounterThreads[i],NULL,CounterFunction,(void*)ThreadId);
    }

    pthread_t  MonitorThread,CollectorThread;
    pthread_create(&MonitorThread,NULL,MonitorFunction,NULL);
    pthread_create(&CollectorThread,NULL,CollectorFunction,NULL);



    for(int i=0;i<ThreadNumber;i++)
      pthread_join(&CounterThreads[i],NULL);

    pthread_join(&MonitorThread,NULL);
    pthread_join(&CollectorThread,NULL);




    return 0;
}
