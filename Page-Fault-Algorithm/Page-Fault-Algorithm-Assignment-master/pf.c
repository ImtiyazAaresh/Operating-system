#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 1024

void optimum(int input_process[],int working_set_size,int index)
{
	/* Passed values from main check test
	int i;
	for(i=0;i<index;i++)
	{
		printf("vale=%d\n",input_process[i]);
	}
	*/
	int buffer[working_set_size];
	int buffer_index = 0;
	int i,a;
	// fill buffer with negative values
	for(i=0;i<working_set_size;i++)
	{
		buffer[i]= -1;
	}
	
	for(i=0;i<index;i++)
	{
		int buffer_flag=0;
		//for < working_set_size
		if(buffer_index < working_set_size)
		{
			for(a=0;a<working_set_size;a++)
			{
				if(buffer[a]==input_process[i])
				{
				buffer_flag = 1;
				break;
				}
			}
		}
		if(buffer_flag==0 && buffer_index < working_set_size)
		{
			buffer[buffer_index]=input_process[i];
			buffer_index++;
		}
	//for < working_set_size
	}
	int k;
	for(k=0;k<working_set_size;k++)
	{
		printf("buffer values =%d\n",buffer[k]);
	}
	
	printf("Page Fault for optimum is %d",buffer_index);
}

int main( int argc, char * argv[] ) 
{
  char * line = NULL;
  size_t line_length = MAX_LINE;
  char * filename;
  FILE * file;

  if( argc < 2 )
  {
    printf("Error: You must provide a datafile as an argument.\n");
    printf("Example: ./fp datafile.txt\n");
    exit( EXIT_FAILURE );
  }

  filename = ( char * ) malloc( strlen( argv[1] ) + 1 );
  line     = ( char * ) malloc( MAX_LINE );

  memset( filename, 0, strlen( argv[1] + 1 ) );
  strncpy( filename, argv[1], strlen( argv[1] ) );

  printf("Opening file %s\n", filename );
  file = fopen( filename , "r");

  if ( file ) 
  {
    while ( fgets( line, line_length, file ) )
    {
      char * token;
      int input_process[line_length];
	int index=0;
      token = strtok( line, " ");
      int working_set_size = atoi( token );

      printf("\nWorking set size: %d\n", working_set_size );
 
      while( token != NULL )
      {
      	
        token = strtok( NULL, " " );

        if( token != NULL )
        {
           	printf("Request: %d\n", atoi( token ) ); 
            //input into string
        	input_process[index]= atoi( token );
        	index++;//
        }
      }

	optimum(input_process,working_set_size,index);
      printf("\n");
    }

    free( line );
    fclose(file);
  }

  return 0;
}
