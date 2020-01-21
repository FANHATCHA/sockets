// Cwk2: server.c - multi-threaded server using readn() and writen()

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "rdwrn.h"

#include <sys/utsname.h>
#include <dirent.h> 
#include <sys/stat.h> 
#include <signal.h>
#include <sys/time.h> 

// thread function
void *client_handler(void *);
void transferStudentInfo(int);
void transferFiveRandomNum(int);
void transferServerUnameInfo(int);
void transferFiles(int);
void transferFileCopyFromServer(int);

char * ipAddress;
struct timeval startTime, endTime; 

static void handler()
{
    //Termination of the program
    if (gettimeofday(&endTime, NULL) == -1) {
        perror("Error from gettimeofday, can't set up endTime");
        exit(EXIT_FAILURE);
    }
    
    printf("\nStopping the server\n");
    //Printing in seconds maximum execution time
    printf("\nTotal execution time = %f seconds\n",
       (double) (endTime.tv_usec - startTime.tv_usec) / 1000000 +
       (double) (endTime.tv_sec - startTime.tv_sec));
    
    exit(EXIT_SUCCESS);
}

// you shouldn't need to change main() in the server except the port number
int main(void)
{
    /********************* Start | SIGNAL HANDLER ********************************/
    //Get the start time
    if (gettimeofday(&startTime, NULL) == -1) {
        perror("Error from gettimeofday, can't set up endTime");
        exit(EXIT_FAILURE);
    }
    
    //Set up signal handler for SIGINT
    struct sigaction act;

    memset(&act, '\0', sizeof(act));

    //This will index the handler function as soon as it gets the SIGINT signal
    act.sa_sigaction = &handler;
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("Couldn't set up signal handler");
        exit(EXIT_FAILURE);
    }

   /********************** End | SIGNAL HANDLER ****************************/ 

    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(50031);

    ipAddress = inet_ntoa(serv_addr.sin_addr);



    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (listen(listenfd, 10) == -1) {
    perror("Failed to listen");
    exit(EXIT_FAILURE);
    }
    // end socket setup

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    while (1) {
    printf("Waiting for a client to connect...\n");
    connfd =
        accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
    printf("Connection accepted...\n");

    pthread_t sniffer_thread;
        // third parameter is a pointer to the thread function, fourth is its actual parameter
    if (pthread_create
        (&sniffer_thread, NULL, client_handler,
         (void *) &connfd) < 0) {
        perror("could not create thread");
        exit(EXIT_FAILURE);
    }
    //Now join the thread , so that we dont terminate before the thread
    //pthread_join( sniffer_thread , NULL);
    printf("Handler assigned\n");
    }

    // never reached...
    // ** should include a signal handler to clean up
    exit(EXIT_SUCCESS);
} // end main()

// thread function - one instance of each for each connected client
// this is where the do-while loop will go
void *client_handler(void *socket_desc)
{
    //Get the socket descriptor
    int connfd = *(int *) socket_desc;

    char userInput;

    do{

     char valueEntered[10];
     size_t n;

     readn(connfd, (unsigned char *) &n, sizeof(size_t));
     readn(connfd, (unsigned char *) valueEntered, n);

     userInput = valueEntered[0];

     switch(userInput){

        case '1':
            transferStudentInfo(connfd);
            break;
        case '2':
            transferFiveRandomNum(connfd);
            break;
        case '3':
            transferServerUnameInfo(connfd);
            break;
        case '4':
            transferFiles(connfd);
            break;
        case '5':
            transferFileCopyFromServer(connfd);
            break;
        case '6':
            printf("End of thread ! Good Bye !");
            break;
        default:
            break;
     }


      }while(userInput != '6');

    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    printf("Thread %lu exiting\n", (unsigned long) pthread_self());

    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    return 0;
}  // end client_handler()

void transferStudentInfo(connfd){
 
    char studentInfoAndIp[256];
    char studentNameAndId[] = "\n-Name: Kone Fanhatcha\n-ID: S1803435\n";
    
    strcpy(studentInfoAndIp, studentNameAndId);
    strcat(studentInfoAndIp, "-IP adress: ");
    strcat(studentInfoAndIp, ipAddress);

    size_t n = strlen(studentInfoAndIp) + 1;
    writen(connfd, (unsigned char *) &n, sizeof(size_t));   
    writen(connfd, (unsigned char *) studentInfoAndIp, n);
}
void transferFiveRandomNum(connfd){
    char numbers[40];
    strcpy(numbers, "\n");
    int counter = 1;
    int number;
    while (counter <= 5) {
        number = rand() % 1000 + 1;
        
        char numberToString[3];
        
        strcat(numbers, "- ");
        sprintf(numberToString, "%d", number);
        strcat(numbers, numberToString);
        strcat(numbers, "\n");

        counter++;
    }
    
    size_t n = strlen(numbers) + 1;
    writen(connfd, (unsigned char *) &n, sizeof(size_t));   
    writen(connfd, (unsigned char *) numbers, n);
}

void transferServerUnameInfo(connfd){

    struct utsname * uts = (struct utsname *) malloc(sizeof(struct utsname));

    size_t payload_length;

    size_t n =
    readn(connfd, (unsigned char *) &payload_length, sizeof(size_t));
    n = readn(connfd, (unsigned char *) uts, payload_length);

    if (uname(uts) == -1) {
        perror("uname error");
        exit(EXIT_FAILURE);
    }

    writen(connfd, (unsigned char *) &payload_length, sizeof(size_t));
    writen(connfd, (unsigned char *) uts, payload_length);
    
    printf("Payload length is: %zu (%zu bytes)\n", payload_length, n);
}

void transferFiles(connfd){
    char fileNames[128];
    
    struct dirent **directoryEntries;
    int scanner;
    
    if ((scanner = scandir("./upload", &directoryEntries, NULL, alphasort)) != -1){
        while (scanner--) {
            char pathname[128] = "./upload/";
            strcat(pathname, directoryEntries[scanner]->d_name);
            
            struct stat sb;
            if (stat(pathname, &sb) == -1) {
                perror("Error stating file");
                exit(EXIT_FAILURE);
            }

            if((sb.st_mode & S_IFMT) == S_IFREG){
                strcat(fileNames, directoryEntries[scanner]->d_name);
                strcat(fileNames, ", ");
            }

            free(directoryEntries[scanner]);   
        }

        free(directoryEntries); 
        
    } else {
        perror("Error while scanning directory");
    }
    
    size_t n = strlen(fileNames) + 1;
    writen(connfd, (unsigned char *) &n, sizeof(size_t));   
    writen(connfd, (unsigned char *) fileNames, n);
}
void transferFileCopyFromServer(connfd){
    char fileName[128];
    size_t k;

    readn(connfd, (unsigned char *) &k, sizeof(size_t));    
    readn(connfd, (unsigned char *) fileName, k);
    
    struct stat buffer;
    char filePath[192] = "./upload/";
    strcat(filePath, fileName);

    if(stat(filePath, &buffer) < 0){
        char fileNotFound[32] = "";
        
        size_t n = strlen(fileNotFound) + 1;
        writen(connfd, (unsigned char *) &n, sizeof(size_t));   
        writen(connfd, (unsigned char *) fileNotFound, n);
    } else {
        FILE *fileToCopy;
        fileToCopy = fopen(filePath, "rb");
        char fileContent[BUFSIZ];
        size_t t = fread(fileContent, sizeof(unsigned char), BUFSIZ, fileToCopy);
        writen(connfd, (unsigned char *) &t, sizeof(size_t));   
        writen(connfd, (unsigned char *) fileContent, t);
        
        fclose(fileToCopy);

    }
}

