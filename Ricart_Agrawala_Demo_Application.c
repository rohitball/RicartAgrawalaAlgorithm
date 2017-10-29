///************** Ricart–Agrawala Algorithm**************///
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PROCESS (3)

typedef struct
{
  int clock[ NUM_PROCESS ]; //Receive Message Timestamp
  int RD[ NUM_PROCESS ];      //Request Deffereed Array
  int reply[ NUM_PROCESS ];
  int cs_flag;
}PROCESS;

PROCESS processes[ NUM_PROCESS ] = {0};

//Local functions
static void GetTimestamp(int *timeStamp);
static ProcessRequest( int processId, int reqProcessId );
static void SendReply( int processId, int reqProcessId );
static void ExecuteCriticalSection( int processId );

//APIs
void CS_RequestCriticalSection( int processId, int *clock );
void CS_ReceivedRequest( int processId, int reqProcessId, int clock);
void CS_ReceivedReply( int processId, int fromProcessId );

int main( void )
{
  int timeStamp0 = 0;
  int timeStamp1 = 0;

  printf("\n\n\t************** Ricart Agrawala Algorithm **************\t\n\n");
  printf("The distributed system has 3 Processes with Ids 0, 1, and 2\n\n");
  //Process 0 makes a request for the CS
  CS_RequestCriticalSection( 0, &timeStamp0 );

  sleep(1);

  //Process 1 makes a request the CS
  CS_RequestCriticalSection( 1, &timeStamp1 );

  //Process 2 receives request from 1
  CS_ReceivedRequest( 2, 1, timeStamp1 );

  sleep(2);

  //Process 0 receives request from 1
  CS_ReceivedRequest( 0, 1, timeStamp1 );

  sleep(2);

  //Process 1 receives request from 0
  CS_ReceivedRequest( 1, 0, timeStamp0 );

  sleep(2);

  //Process 0 receives reply from 1
  CS_ReceivedReply( 0, 1);

  sleep(2);

  //Process 1 receives reply from 2
  CS_ReceivedReply( 1, 2 );

  sleep(2);

  //Process 0 receives reply from 2
  CS_ReceivedReply( 0, 2 );

  sleep(2);

  //Process 1 receives reply from 0
  CS_ReceivedReply( 1, 0 );

  sleep(5);
}

void CS_RequestCriticalSection( int processId, int *clock )
{
  GetTimestamp( clock );

  //Update requesting process's clock
  processes[ processId ].clock[ processId ] = *clock;

  printf( "Process [%d] at Timestamp [%d] Broadcasting REQUEST message\n", processId, *clock );
}

void CS_ReceivedRequest( int processId, int reqProcessId, int clock)
{
  printf("Process [%d] received REQUEST from Process [%d] with Timestamp [%d]\n", processId, reqProcessId, clock );

  //Updae clock
  processes[ processId ].clock[ reqProcessId ] = clock;

  //Process Request
  ProcessRequest( processId, reqProcessId );
}

void CS_ReceivedReply( int processId, int fromProcessId )
{
  int exe_cs = 1;

  //Set Reply flag
  processes[ processId ].reply[ fromProcessId ] = 1;

  printf("Process [%d] received REPLY from process [%d]\n", processId, fromProcessId);

  //Check reply from all the other processes
  for( int i = 0; i < NUM_PROCESS; i++ )
  {
	if( ( i != processId ) && ( processes[ processId ].reply[ i ] != 1 ) )
	{
	  exe_cs = 0;
	}
  }

  if( exe_cs == 1 )
  {
	printf("Process [%d] received REPLY from All the processes\n", processId);
	ExecuteCriticalSection( processId );
  }
}

static ProcessRequest( int processId, int reqProcessId )
{
  if( ( processes[ processId ].cs_flag == 1 ) || (processes[ processId ].clock[ processId ] != 0 ) )
  {
	if( processes[ processId ].clock[ reqProcessId ] < processes[ processId ].clock[ processId ] )
	{
	  //Send REPLY
	  SendReply( processId, reqProcessId);
	}
	else
	{
	  //Set request deffered flag for requesting process, at receiving process
	  processes[ processId ].RD[ reqProcessId ] = 1;
	  printf( "Process [%d] REQUEST from Process [%d] is DEFFERED\n", processId, reqProcessId );
	}
  }
}

static void SendReply( int processId, int reqProcessId )
{
  /* Reset requesting process's clock and RD flag at process receiving process */
  processes[ processId ].clock[ reqProcessId ] = 0;
  processes[ processId ].RD[ reqProcessId ] = 0;

  printf( "Process [%d] sending REPLY to Process [%d]\n", processId, reqProcessId );
}


static void ExecuteCriticalSection( int processId )
{
  //Entering critical section
  processes[ processId ].cs_flag = 1;
  printf( "Process [%d] enters critical section\n", processId );

  //Executing critical section
  sleep(1);
  printf( "Process [%d] is executing critical section\n", processId );
  sleep(1);

  //Exiting critical section
  printf( "Process [%d] exits critical section\n", processId );

  processes[ processId ].cs_flag = 0;

  //Send reply to deffered processes
  for( int i = 0; i < NUM_PROCESS; i++ )
  {
	if( ( i != processId ) && ( processes[ processId ].RD[i] != 0 ) )
	{
	  SendReply( processId, processes[ processId ].RD[i]);
	  processes[ processId ].RD[i] = 0;
	  processes[ processId ].reply[ i ] = 0;
	}
  }
}

static void GetTimestamp(int *clock)
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  *clock = tm.tm_hour * 60 * 60 + tm.tm_min * 60 + tm.tm_sec; //In seconds
}


