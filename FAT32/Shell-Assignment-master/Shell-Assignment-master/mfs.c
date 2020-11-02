//to do
//add clsuter number in stat
//add cd../<folder name> 
//to do


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
#include <stdint.h>


#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments
	
#define CLUSTER_SIZE 512		//default clsuter size in FAT32 file system

//adding initial declerations
uint16_t BPB_BytesPerSec;
uint8_t BPB_SecPerClus;
uint16_t BPB_RsvdSecCnt;
uint8_t BPB_NumFATS;
uint16_t BPB_RootEntCnt;
uint32_t BPB_FATSz32;

//adding DirectoryEntry structure to stimulate useful fields of FAT
struct __attribute__((__packed__)) DirectoryEntry
{
        char DIR_Name[11];
        uint8_t DIR_Attr;
        uint8_t Unused1[8];
        uint16_t DIR_FirstCLusterHigh;
        uint8_t Unused2[4];
        uint16_t DIR_FirstCLusterLow;
        uint32_t DIR_FileSize;
};

struct DirectoryEntry dir[16];


//Get Function \\This function takes the input aurgment from the user and compares with files in currect directory 
//and places the file into current working directory
int compare_input_get(char input_string[],FILE *fp)
{
	int flag=0; 
	int cluster_number;//cluster number
	int size_of_file;//size of file in fat32.img
	FILE *fp_get;//file pointer to point new file at current working directory
	int offset_read;//address of file in cluster
	//compare block to convert input string to upper case and 11 characters
	char expanded_name[12];
	memset( expanded_name, ' ', 12 );
	char *token_c = strtok( input_string, "." );
	strncpy( expanded_name, token_c, strlen( token_c ) );
	token_c = strtok( NULL, "." );

	if( token_c )
	{
    strncpy( (char*)(expanded_name+8), token_c, strlen(token_c ) );
	}

	expanded_name[12] = '\0';

	int i;
	for( i = 0; i < 11; i++ )
	{
    expanded_name[i] = toupper( expanded_name[i] );
	}
	
		for(i=0;i<16;i++)////compare each file in current directory and modified input aurgment
		{
			if( dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20 )//display existing files only,eliminate deleted files
			{
			if((strncmp(expanded_name,dir[i].DIR_Name,11)==0))//compare each file in current directory and modified input aurgment
			{
				cluster_number = dir[i].DIR_FirstCLusterLow;//copy cluster number
				size_of_file = dir[i].DIR_FileSize;//copy size of file
				flag=1;//if file found then flag=1
				break;
			}
			}
		}
		if(flag == 1)//file found if flag = 1
		{
			fp_get = fopen(input_string,"wa");//create file in current working directory with write and append 
			char value1[CLUSTER_SIZE];//buffer array to store data of file
			int16_t val = 0;//variable to check next cluster value
			int check = size_of_file;//to avoid unwanted data
			int read_bytes;//input aurgment to read number of files
			while( val != -1 )//break when next cluster value is -1
			{
					if( check >= CLUSTER_SIZE )//divide file into size of 512 blocks chunks and read,write each chunk
					{
						read_bytes = CLUSTER_SIZE;//each chunk
					}
					else
					{
						read_bytes=check;//when file size is < 512
					}
					//calculate address of file in cluster
					offset_read = (( cluster_number - 2 ) * BPB_BytesPerSec) + (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec);
					fseek(fp,offset_read,SEEK_SET);//point to offset_read address in file 
					fread(&value1,sizeof(char),read_bytes, fp);//read into variabe array
					fwrite(&value1,sizeof(char),read_bytes, fp_get);//write into variabe array
					uint32_t FATAddress = (BPB_RsvdSecCnt * BPB_BytesPerSec) + (cluster_number * 4);//calculate FATAddress
					fseek(fp,FATAddress,SEEK_SET);//point to FATAddress address in file 
					fread(&val,2,1,fp);//to check file exists in other FAT,if no val =-1
					cluster_number=val;//move cluster number for loop use
					check=check-CLUSTER_SIZE;
			}
			fclose(fp_get);//close new file after complete write done
		}
		else
		{
			printf("Error: File not found.\n");//if input aurgment is not found.
		}
		
}
//GET function

// addding compare function for CD
//This function comapres input aurgment with files in current directories and returns cluster value for match
//if input aurgment is not folder or not present in current directory, returns 0
int compare_input(char input_string[])
{
	int flag=0; //flag to check whether input aurgment exists in current directory
	int cluster_number;//cluster number of the floder
	int folder_flag=1;//check whether the input aurgment is floder or not
	//compare block to convert input string to upper case and 11 characters
	char expanded_name[12];
	memset( expanded_name, ' ', 12 );
	char *token_c = strtok( input_string, "." );
	strncpy( expanded_name, token_c, strlen( token_c ) );
	token_c = strtok( NULL, "." );

	if( token_c )
	{
    strncpy( (char*)(expanded_name+8), token_c, strlen(token_c ) );
	}

	expanded_name[12] = '\0';

	int i;
	for( i = 0; i < 11; i++ )
	{
    expanded_name[i] = toupper( expanded_name[i] );
	}
	

		for(i=0;i<16;i++)////compare each file in current directory and modified input aurgment
		{
			if( dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20)//display existing files only,eliminate deleted files
			{
			if((strncmp(expanded_name,dir[i].DIR_Name,11)==0))//compare each file in current directory and modified input aurgment
			{
				 cluster_number = dir[i].DIR_FirstCLusterLow;//copy cluster number
				 folder_flag = dir[i].DIR_FileSize;//copy size of file
				flag=1;//if file found then flag=1
				break;
			}
			}
		}
		
		if(folder_flag != 0)//check if input aurgment is folder or file
		{
			flag = 0;//if file then flag = 0
		}
		
		if( flag == 0 )
		{
		return 0;//if input aurgment is not found or not found
		}
		else
		{
			return cluster_number;//return cluster number for input folder name
		}
}
// addding compare function for CD

//String generation for READ 
//this funcction takes input aurgments and prints data in file from mentioned position to no_of_bytes number of bytes
int compare_input_read(char input_string[],int position,int no_of_bytes,FILE *fp)
{
	int flag=0; //flag to check whether input aurgment exists in current directory
	int cluster_number;//cluster number
	int size_of_file;//size of file in fat32.img
	int offset_read;//address of file in cluster
	int index=0;//index to cprint values
	//compare block to convert input string to upper case and 11 characters
	char expanded_name[12];
	memset( expanded_name, ' ', 12 );
	char *token_c = strtok( input_string, "." );
	strncpy( expanded_name, token_c, strlen( token_c ) );
	token_c = strtok( NULL, "." );

	if( token_c )
	{
    strncpy( (char*)(expanded_name+8), token_c, strlen(token_c ) );
	}

	expanded_name[12] = '\0';

	int i;
	for( i = 0; i < 11; i++ )
	{
    expanded_name[i] = toupper( expanded_name[i] );
	}
	

		for(i=0;i<16;i++)////compare each file in current directory and modified input aurgment
		{
			if( dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20 )//display existing files only,eliminate deleted files
			{
			if((strncmp(expanded_name,dir[i].DIR_Name,11)==0))//compare each file in current directory and modified input aurgment
			{
				 cluster_number = dir[i].DIR_FirstCLusterLow;//copy cluster number
				 size_of_file = dir[i].DIR_FileSize;//copy size of file
				flag=1;//if file found then flag=1
				break;
			}
			}
		}
		char value[size_of_file];//buffer array to store data of file
		// to avoid segmentation fault. if file size is less than no of bytes to read
		if(no_of_bytes > size_of_file)//divide file into size of 512 blocks chunks and read,write each chunk
		{
			no_of_bytes=size_of_file;//each chunk
		}
		int check = no_of_bytes;
		int read_bytes;
		int16_t val=0;

		
		if(flag == 1)//file found if flag = 1
		{
				while(val != -1)
				{
					if( check >= CLUSTER_SIZE )//divide file into size of 512 blocks chunks and read,write each chunk
					{
						read_bytes = CLUSTER_SIZE;//each chunk
					}
					else
					{
						read_bytes=check;//when file size is < 512
					}
					//calculate address of file in cluster
					offset_read = (( cluster_number - 2 ) * BPB_BytesPerSec) + (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec);
					offset_read = offset_read + position;//point to offset_read address in file + position
					position=0;//only add position for first time
					fseek(fp,offset_read,SEEK_SET);//read into variabe array
					fread(&value,sizeof(char),read_bytes, fp);//write into variabe array
					for(index=0;index < read_bytes;index++)//print data in value array
					{
						printf("%c",value[index]);
					}
					uint32_t FATAddress = (BPB_RsvdSecCnt * BPB_BytesPerSec) + (cluster_number * 4);
					fseek(fp,FATAddress,SEEK_SET);//point to FATAddress address in file 
					fread(&val,2,1,fp);//to check file exists in other FAT,if no val =-1
					cluster_number=val;//move cluster number for loop use
					check=check-CLUSTER_SIZE;
					if(check <= 0)
					{
						break;
					}
				}
				if(no_of_bytes<size_of_file)
				{
					printf("\n");
				}
			
		}
		else
		{
			printf("Input file not found\n");
		}
}
//String generation for READ 


// addding compare function for STAT
int compare_input_stat(char input_string[])
{
	int flag=0; 
	int cluster_number;
	char expanded_name[12];
	memset( expanded_name, ' ', 12 );
	char *token_s = strtok( input_string, "." );
	strncpy( expanded_name, token_s, strlen( token_s ) );
	token_s = strtok( NULL, "." );

	if( token_s )
	{
    strncpy( (char*)(expanded_name+8), token_s, strlen(token_s ) );
	}

	expanded_name[12] = '\0';

	int i;
	for( i = 0; i < 11; i++ )
	{
    expanded_name[i] = toupper( expanded_name[i] );
	}
	

		for(i=0;i<16;i++)
		{
			if( dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20 )
			{
			if((strncmp(expanded_name,dir[i].DIR_Name,11)==0))
			{
				flag=1;
				break;
			}
		
			}
		}
		
		if( flag == 0 )
		{
		return 20;
		}
		else
		{
			return i;
			
		}
}
// addding compare function for STAT



int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
	
	//adding status flag to check whether any file is already open
	int status_flag =0;
	FILE *fp;
	int root_dir;
	int cluster_number;
	int offset_cd;
	int previous_add[20];
	int address_index=0;
	
	//adding status flag to check whether any file is already open
	
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
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

   /* int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }*/
	
////OPEN CODE	
	//adding code for OPEN
	if(token[0]!=NULL)//avoid segmentation if input aurgment is null
	{
		if((strcmp(token[0],"open")==0))
		{
		if(status_flag == 1)	//check if the file system is open
		{
			printf("Error: File system image already open.\n");
		}
		else
		{
			if(token[1] != NULL)
			{
		if(strlen(token[1]) < 100)
		{
        fp = fopen(token[1], "r");				

        if(!fp)
        {
                printf("Error: File system image not found.\n");

        }
		else
		{

		
		fseek(fp,11,SEEK_SET);
        fread(&BPB_BytesPerSec,2,1,fp);
		
		fseek(fp,13,SEEK_SET);
        fread(&BPB_SecPerClus,1,1,fp);
		
		fseek(fp,14,SEEK_SET);
        fread(&BPB_RsvdSecCnt,2,1,fp);
		
		fseek(fp,16,SEEK_SET);
        fread(&BPB_NumFATS,1,1,fp);
		
		fseek(fp,36,SEEK_SET);
        fread(&BPB_FATSz32,4,1,fp);
		
		
		root_dir = (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec);//ROOT DIRETORY
		
		fseek(fp,root_dir,SEEK_SET);
		fread(&dir[0],16,sizeof( struct DirectoryEntry ), fp);
		previous_add[0]=root_dir;
		status_flag=1;
		}
		}
		else
		{
			printf("Error: File name is greater than 100 characters\n");
		}
		}
		else
		{
			printf("Error: Pass file name in below format.\nopen <filename>.\n");
		}
		}
		}
	}	
	//adding code for OPEN
////OPEN CODE

////INFO CODE	
	//adding code for INFO
	if(token[0]!=NULL)//avoid segmentation if input aurgment is null
	{
		if((strcmp(token[0],"info")==0))
		{
		if(status_flag == 0)	//check if the file system is open
		{
			printf("Error: File system image must be opened first.\n");
		}
		else
		{

		printf("BPB_BytesPerSec in decimal		:  %d\n",BPB_BytesPerSec);
        printf("BPB_BytesPerSec in hexadecimal	  	:  %x\n",BPB_BytesPerSec);

		printf("BPB_SecPerClus in decimal		:  %d\n",BPB_SecPerClus);
        printf("BPB_SecPerClus in hexadecimal	 	:  %x\n",BPB_SecPerClus);

        printf("BPB_RsvdSecCnt in decimal		:  %d\n",BPB_RsvdSecCnt);
        printf("BPB_RsvdSecCnt in hexadecimal		:  %x\n",BPB_RsvdSecCnt);

        printf("BPB_NumFATS in decimal			:  %d\n",BPB_NumFATS);
        printf("BPB_NumFATS in hexadecimal		:  %x\n",BPB_NumFATS);

        printf("BPB_FATSz32 in decimal			:  %d\n",BPB_FATSz32);
        printf("BPB_FATSz32 in hexadecimal		:  %x\n",BPB_FATSz32);

		}
		}
	}
	//adding code for INFO
////INFO CODE

////LS CODE
	//adding code for ls
	
	if(token[0]!=NULL)//avoid segmentation if input aurgment is null
	{
		if((strcmp(token[0],"ls")==0))
		{
		if(status_flag == 0)	//check if the file system is open
		{
			printf("Error: File system image must be opened first.\n");
		}
		else
		{
	
		int i;
		for(i=0;i<16;i++)
		{
			if( dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20)
			{
				if(dir[i].DIR_Name[0] != -27)
				{
			printf("FILE NAME : %.11s\n",dir[i].DIR_Name);
				}
			}
			
		}
		}	
		}
	}	
	//adding coed for ls
////LS CODE

////CD CODE
	//adding code fro cd
	
	if(token[0]!=NULL)//avoid segmentation if input aurgment is null
	{
		if((strcmp(token[0],"cd")==0))
		{
		if(status_flag == 0)	//check if the file system is open
		{
			printf("Error: File system image must be opened first.\n");
		}
		else
		{
			if(token[1]!=NULL)
			{
				if((strcmp(token[1],"..")==0))
				{
					if(address_index == 0)
					{
						printf("Error: Current driectory is root directory\n");
					}
					else
					{
						address_index = address_index - 1;
						fseek(fp,previous_add[address_index],SEEK_SET);
						fread(&dir[0],16,sizeof( struct DirectoryEntry ), fp);
					}
				}	
				else
				{
		int cluster_number = 0;
		
		cluster_number = compare_input(token[1]);
		
		if(cluster_number == 0)
		{
			printf("Error: Folder not found\n");
		}
		else
		{
		offset_cd = (( cluster_number - 2 ) * BPB_BytesPerSec) + (BPB_NumFATS * BPB_FATSz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec);

		
		fseek(fp,offset_cd,SEEK_SET);
		fread(&dir[0],16,sizeof( struct DirectoryEntry ), fp);
		address_index=address_index + 1;
		previous_add[address_index]=offset_cd;
		}
		}		
				}
		else
		{
			printf("Error: Pass directory name in below format.\nFORMAT : cd <directory name> .\n");
		}
		}
		}
	}	
	//adding coed for cd
////CD CODE

////STAT CODE
//adding coed for stat
	
	if(token[0]!=NULL)//avoid segmentation if input aurgment is null
	{
		if((strcmp(token[0],"stat")==0))
		{		
		if(status_flag == 0)	//check if the file system is open
		{
			printf("Error: File system image must be opened first.\n");
		}
		else
		{
			if(token[1]!=NULL)
			{
				int index_stat=20;
		
		index_stat = compare_input_stat(token[1]);
		
		if(index_stat == 20)
		{
			printf("Error: File not found\n");
		}
		else
		{
			printf("FILE NAME 		: %.11s\n",dir[index_stat].DIR_Name);
			printf("Cluster Number 		: %d\n",dir[index_stat].DIR_FirstCLusterLow);
			printf("ATTRIBUTE VALUE		: %d\n",dir[index_stat].DIR_Attr);
			printf("FirstCLusterHigh	: %d\n",dir[index_stat].DIR_FirstCLusterHigh);
			printf("Unused1 		: %d\n",dir[index_stat].Unused1);
			printf("Unused2 		: %d\n",dir[index_stat].Unused2);
			printf("FileSize 		: %d\n",dir[index_stat].DIR_FileSize);
		}
			}
			else
			{
				printf("Error: Pass filename or directory name in below format.\nFORMAT : stat <filename> or <directory name> .\n");
			}
		}
		}
	}
	//adding coed for stat
////STAT CODE

////READ CODE
	//adding coed for read
	if(token[0]!=NULL)//avoid segmentation if input aurgment is null
	{
		if((strcmp(token[0],"read")==0))
		{
		if(status_flag == 0)	//check if the file system is open
		{
			printf("Error: File system image must be opened first.\n");
		}
		else
		{
			if(token[1]!=NULL && token[2]!=NULL && token[3]!=NULL)
			{
				
		int position = atoi(token[2]);
		int no_of_bytes= atoi(token[3]);
		compare_input_read(token[1],position,no_of_bytes,fp);
		
		}		
		else
		{
			printf("Error: Pass File name in below format.\nFORMAT : read <filename> <position> <number of bytes>.\n");
		}
		}
		}
	}	
	//adding coed for read
////READ CODE


////GET CODE
	//adding code for get
	if(token[0]!=NULL)//avoid segmentation if input aurgment is null
	{
		if((strcmp(token[0],"get")==0))
		{
		if(status_flag == 0)	//check if the file system is open
		{
			printf("Error: File system image must be opened first.\n");
		}
		else
		{
			if(token[1] != NULL)
		{
			compare_input_get(token[1],fp);
		}
		else
		{
			printf("Error: Pass File name in below format.\nFORMAT : get <filename>.\n");
		}
		}
		}
	}
	//adding code for get
////GET CODE



////CLOSE CODE	
	//adding code for CLOSE
	if(token[0]!=NULL)//avoid segmentation if input aurgment is null
	{
		if((strcmp(token[0],"close")==0))//if input aurgment is close,it close file system if already open
		{
		if(status_flag == 0)	//check if the file system is open
		{
			printf("Error: File system not open.\n");
		}
		else
		{
		fclose(fp);

		status_flag=0;
		}
		}
	}	
	//adding code for CLOSE
////CLOSE CODE

////EXIT CODE	
	//adding code for EXIT
	if(token[0]!=NULL)//avoid segmentation if input aurgment is null
	{
		//if the input aurgment is exit or quit, program exits from simulation
		if((strcmp(token[0],"exit")==0) || (strcmp(token[0],"quit")==0))
		{
		if(status_flag == 1)	//if the filesystem is open, then close file system and exit
		{
			fclose(fp);		//close file system
			break;
		}
		else
		{
		break;//if the filesystem is not open, exit directtly
		}
		}
	}	
	//adding code for EXIT
////EXIT CODE

    free( working_root );//free heap memory

  }
  return 0;
}
