// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 12     // Mav shell only supports five arguments

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  
  char *history_list[15];
  int count_history = 0;

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
	
	if(count_history<15)
	{
		history_list[count_history] = strndup( cmd_str, MAX_COMMAND_SIZE );
		count_history++;
	}
	
	

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
		//printf("arg_ptr = %s\n",arg_ptr);
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
	token[11] = NULL;

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }
	
	int no_of_child = 0;
	if(token[0]!=NULL)
	{
		if((strcmp(token[0],"exit")==0) || (strcmp(token[0],"quit")==0))
		{
			printf("in exit block\n");
			//exit(0);
			return 0;
		}
		else if(strcmp(token[0],"history")==0)
		{
			
			if(count_history>15)
			{
				int index = 0;
				printf("count_history = %d\n",count_history);
				for (index = 0; index < 15; index++)
				{
					if(index == 14)
					{
						history_list[index]= strndup( cmd_str, MAX_COMMAND_SIZE );
					}
					else
					{
						history_list[index]=history_list[index+1];
					}
					
				}
				count_history=15;
			}
			int history_index = 0;
			
			for( history_index = 0; history_index < count_history; history_index++ ) 
			{
				printf("%d: %s\n", history_index, history_list[history_index] ); 	
			}
			
		}
	  
		pid_t pid = fork( );
		
		if( pid == 0 && (strcmp(token[0],"cd")!=0))
		{
		// Notice you can add as many NULLs on the end as you want
		int ret = execvp(token[0], token);  
		if( ret == -1 )
		{
		  printf("%s: Command not found.\n",token[0]);
		  //perror("execl failed: ");
		  return 0;
		}
	    }
	    else 
	    {
		  printf("in parent\n");
		  //To change the directory if "CD .." command is entered.
			if(strcmp(token[0],"cd")==0)
			{
				//(strcmp(token[1],"..")==0)
				printf("in parent before chdir function\n");
				int ret_cd = chdir(token[1]);
				if(ret_cd == 0)
				{
					printf(" chdir worked successfully\n");
					
				}
				else if (ret_cd == -1)
				{	
					printf(" value -1 is returned for chdir function and directory is unchanged./n");
				}
				else
				{
					printf("in else block after execution of chdir function ");
				}
			}
			int status;
			printf("status = %d",status);
			wait( & status );
			printf("after wait in parent\n");
	    }
		
	}
		
		

	  

		free( working_root );

  }
  return 0;
}
