#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

void
main(int argc, char* argv[]){
 if(argc < 3){
  fprintf(stderr,"%s INPUT_FILE... OUTPUT_FILE\n", argv[0]);
  _exit(-1);
 }
 for(int iteration = 1; iteration <= argc; ++iteration){
  char buffer[1024];
  int hex_fd = open(argv[iteration], 102, 0664);
  if(!hex_fd) {
   perror("hex_fd open(): ");
   _exit(-1);
  }
  int c = read(hex_fd,buffer,1024);
  if(close(hex_fd)) {
   perror("close(hex_fd): ");
   _exit(-1);
  }
  int i = 0;
  char r[512];
  int n = 0;
first_repeat:
  if( (c-i) <= 0 ) goto end;
  switch(*(buffer+(i))){
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
    r[n] = (*(buffer+i)-48)*16;
    break;
   case 'A':
   case 'B':
   case 'C':
   case 'D':
   case 'E':
   case 'F':
    r[n] = (*(buffer+i)-55)*16;
    break;
   case 'a':
   case 'b':
   case 'c':
   case 'd':
   case 'e':
   case 'f':
    r[n] = (*(buffer+i)-87)*16;
    break;
   case 32:
   case 10:
    ++i;
    goto first_repeat;
   case ';':
    while(*(buffer+(++i)) != '\n');
    goto first_repeat;
   default:
    printf(\
      "\e[91;1mERROR:\e[0m character \e[93m\'%c\'\e[0m is not valid. At byte \e[93m%d\e[0m.\n",\
      *(buffer+i), i\
      );
    _exit(0);
  }
second_repeat:
  if( (c-(i++)) <= 0 ) goto end;
  switch(*(buffer+i)){
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
    r[n] += *(buffer+i)-48;
    break;
   case 'A':
   case 'B':
   case 'C':
   case 'D':
   case 'E':
   case 'F':
    r[n] += *(buffer+i)-55;
    break;
   case 'a':
   case 'b':
   case 'c':
   case 'd':
   case 'e':
   case 'f':
    r[n] += *(buffer+i)-87;
    break;
   case 10:
    printf(\
      "\e[91;1mERROR:\e[0m newline character instead of valid hexadecimal number \e[93m\'%c\\n\'\e[0m. At byte \e[93m%d\e[0m.\n",\
      *(buffer+(i-1)), i\
      );
    _exit(0);
   default:
    printf(\
      "\e[91;1mERROR:\e[0m \e[93m\'%c%c\'\e[0m is not valid hexadecimal number. At byte \e[93m%d\e[0m\n",\
      *(buffer+(i-1)), *(buffer+i), i\
      );
    _exit(0);
  }++n;++i;
  goto first_repeat;
end:
  i=0;
  int binary_fd = open( argv[argc-1], 102 | O_APPEND, 0664);
  if(!binary_fd) {
   perror("binary_fd open(): ");
   _exit(-1);
  }
  while((n-i) != 0) write(binary_fd,(r+(i++)),1);
  if(close(binary_fd)) {
   perror("close(binary_fd): ");
   _exit(-1);
  }
 }
 _exit(0);
}
