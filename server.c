#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <signal.h>
#include "header.h"
#include<pthread.h>

#define PORT 587
#define BACKLOG 5
#define BUFF_SIZE 100
#define MAIL_MODE 10
#define MAIL_ID_MAX 16
#define MAIL_SUBJECT_MAX 500
#define MAIL_BODY_MAX 2000
#define BUFFER_SIZE 1024

int main() {
    int sfd, cfd, binding, size;
    struct sockaddr_in saddr, caddr;
    pid_t p;
    char buff[BUFF_SIZE];
    int ch = 0;
    int value = 1;
    char cwd[200];
    char mail_id[16];
    char mail_subject[500];
    char mail_body[2000];

    // Set up signal handler to avoid zombie processes
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    // Create socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        perror("Error in socket");
        return 1;
    }

    // Set socket options
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

    // Set up address structure
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

    // Bind socket
    binding = bind(sfd, (const struct sockaddr *)&saddr, sizeof(saddr));
    if (binding < 0) {
        perror("Error in binding");
        close(sfd);
        return 1;
    }

    // Listen for incoming connections
    if (listen(sfd, BACKLOG) < 0) {
        perror("Error in listen");
        close(sfd);
        return 1;
    }
    size = sizeof(caddr);

    while (1) {
        printf("\nWaiting for a connection...\n");

        // Accept connection
        cfd = accept(sfd, (struct sockaddr *)&caddr, &size);
        if (cfd < 0) {
            perror("Error in accept");
            continue; // Continue to accept next connection
        }
        printf("\nAccepted connection from %s\n", inet_ntoa(caddr.sin_addr));

        // Create a directory for the client based on their IP address
        char dir_name[BUFF_SIZE];
        snprintf(dir_name, sizeof(dir_name), "%s", inet_ntoa(caddr.sin_addr));

        if (directory_exists(dir_name)) {
            printf("Directory '%s' already exists.\n", dir_name);
               // getcwd(cwd, sizeof(cwd));
            //    strcat(cwd,"/dir_name");
              //  chdir(cwd);


        } else {
            if (mkdir(dir_name, 0777) == 0) {
                printf("Directory '%s' created successfully.\n", dir_name);
        
            } else {
                perror("Error creating directory");
            }
        }

        // Fork a new process to handle client communication
        p = fork();
        if (p == 0) {
            // In child process
            close(sfd); // Close listening socket in child
            while (1) {
                pthread_t threadsend,threadrecv,thread1;
                memset(buff, '\0', sizeof(buff));
                snprintf(buff, sizeof(buff), "\n1 to write\n2 to check\n3 to exit\nEnter your choice: ");
                write(cfd, buff, strlen(buff));

                // Read choice from client
                memset(buff, '\0', sizeof(buff));
                ssize_t bytes_read = read(cfd, buff, sizeof(buff) - 1);
                if (bytes_read <= 0) {
                    if (bytes_read == 0) {
                        printf("Client %s disconnected.\n", inet_ntoa(caddr.sin_addr));
                    } else {
                        perror("Error in read");
                    }
                    break;
                }
                buff[bytes_read] = '\0'; // Null-terminate the string
                ch = atoi(buff); // Convert input string to integer

                if(ch==1)
                {
                    memset(buff, '\0', sizeof(buff));   
                    snprintf(buff, sizeof(buff), "Enter the ID to whom you want to send MAIL : ");  
                    send(cfd, buff, strlen(buff),0);    
                    recv(cfd, mail_id, sizeof(mail_id),0);  
                     
                    memset(buff, '\0', sizeof(buff));
                    snprintf(buff, sizeof(buff), "Write Subject  : ");
                    send(cfd, buff, strlen(buff),0);
                    recv(cfd, mail_subject, sizeof(mail_subject),0);
            
                    memset(buff, '\0', sizeof(buff));
                    snprintf(buff, sizeof(buff), "Write the Mail : ");
                    send(cfd, buff, strlen(buff),0);
                    recv(cfd, mail_body, sizeof(mail_body),0);
            
                    printf("\nUser %s \tMail id is %s\n",inet_ntoa(caddr.sin_addr),mail_id);
                    printf("\nUser %s \tMail subject is %s\n",inet_ntoa(caddr.sin_addr),mail_subject);
                    printf("\nUser %s \tMail body is %s\n",inet_ntoa(caddr.sin_addr), mail_body);
                  

                    if (directory_exists(mail_id)) {
                        printf("Directory '%s' already exists.\n", mail_id);
                    
                    } else {
                        if (mkdir(mail_id, 0777) == 0) {
                            printf("Directory '%s' created successfully.\n", mail_id);
                    
                        } else {
                            perror("Error creating directory");
                        }
                    }
                }
                else if(ch==3)
                {
                    printf("Client %s is disconnected\n", inet_ntoa(caddr.sin_addr));
                }

            }
            close(cfd); // Close client socket in child
            exit(0);    // Exit child process
        } else if (p > 0) {
            // In parent process
            close(cfd); // Close client socket in parent
        } else {
            perror("Fork failed");
            close(cfd);
        }
    }

    close(sfd); // Close listening socket
    return 0;
}