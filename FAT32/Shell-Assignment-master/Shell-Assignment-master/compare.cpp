// The MIT License (MIT)
// 
// Copyright (c) 2019 Trevor Bakker 
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main()
{
  char IMG_Name[12] = "FOO     TXT";
  
  printf("IMG_Name %s\n",IMG_Name);

  char input[8] = "foo.txt";
 printf("input %s\n",input);

  char expanded_name[12];
  printf("expanded_name %s\n",expanded_name);
  
  
  memset( expanded_name, ' ', 12 );
  printf("expanded_name %s\n",expanded_name);

  char *token = strtok( input, "." );
  printf("token %s\n",token);

  strncpy( expanded_name, token, strlen( token ) );
  printf("expanded_name %s\n",expanded_name);

  token = strtok( NULL, "." );
  printf("token %s\n",token);

  if( token )
  {
    strncpy( (char*)(expanded_name+8), token, strlen(token ) );
    printf("expanded_name %s\n",expanded_name);
  }

  expanded_name[11] = '\0';
  printf("expanded_name %s\n",expanded_name);

  int i;
  for( i = 0; i < 11; i++ )
  {
    expanded_name[i] = toupper( expanded_name[i] );
    printf("expanded_name %s\n",expanded_name);
  }

  if( strncmp( expanded_name, IMG_Name, 11 ) == 0 )
  {
    printf("They matched\n");
  }

  return 0;
}
