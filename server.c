#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <wait.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_INSTRUMENTALISTS 20 // hardcoded 20 instrumentalists

int main(void)
{
    signal(SIGPIPE, SIG_IGN); // Ignores the broken pipe exception at the end. Allows infinite running
    
    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(server));
    int sockfd, size, new_sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    size = sizeof(client);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    if (bind(sockfd, (struct sockaddr*) (&server), sizeof(server)) < 0)
    {
        perror("bind fail"); // server's listening socket
        return -1;
    }
    listen(sockfd, 128);

    int pipefd[2];
    if (pipe(pipefd) < 0) 
    {
        perror("pipe failed");
        exit(1);
    }
    
    int instrumentalists[MAX_INSTRUMENTALISTS];
    int instrumentalist_count = 0, tick = 0, rehearsal_running = 0;


    if (!fork())
    {
        close(pipefd[1]); // child does not write

        int child_tick;

        while (1) 
        {
            if (read(pipefd[0], &child_tick, sizeof(int)) > 0)
                printf("Tick received: %d\n", child_tick);
        }
    }

    close(pipefd[0]); // parent does not read

    while (1)
    {
        int new_sockfd = accept(sockfd, (struct sockaddr*) &client, &size);

        // 1. HANDLE NEW CONNECTIONS (if any)
        if (new_sockfd >= 0)
        {
            char line[512];
            int n = read(new_sockfd, line, sizeof(line)); 
            if (n <= 0)
            {
                close(new_sockfd);
                continue;
            }
            line[n] = '\0'; // null-terminate the end of strings

            char* token = strtok(line, " \n\r");
            if (!token)
            {
                close(new_sockfd);
                continue;
            }

            // Instrumentalist
            if (strcmp(token, "JOIN") == 0)
            {
                // Send file contents
                char* piece = strtok(NULL, " ");
                char* part = strtok(NULL, " \r\n");
                char path[1024];
                sprintf(path, "pieces/%s/%s", piece, part);

                FILE* f = fopen(path, "r");
                if (!f) 
                {
                    int err = -1;
                    write(new_sockfd, &err, sizeof(int));
                    close(new_sockfd);
                    continue;
                }

                fseek(f, 0, SEEK_END);
                int file_size = ftell(f);
                rewind(f);

                write(new_sockfd, &file_size, sizeof(int)); // response of the file size (for the loop to read chars)

                char c;
                while ((c = fgetc(f)) != EOF)
                {
                    if (write(new_sockfd, &c, 1) <= 0) // RESPONSE with file
                    {
                        printf("\nClient Disconnected during File Transfer\n");
                        break;
                    }
                }
                fclose(f);
                
                // Add to the list
                if (instrumentalist_count >= MAX_INSTRUMENTALISTS)
                {
                    close(new_sockfd);
                    printf("\nMaximum Number of Instrumentalists Reached...\n");
                    continue;
                }    
                instrumentalists[instrumentalist_count++] = new_sockfd;
            }

            // Admin
            else if (strcmp(token, "START") == 0)
            {
                // Start Rehearsal
                rehearsal_running = 1;
                tick = 0;
                close(new_sockfd);
            }

            // Admin
            else if (strcmp(token, "STOP") == 0)
            {
                // Stop Rehearsal
                for (int i = 0; i < instrumentalist_count; i++)
                    close(instrumentalists[i]);

                close(new_sockfd);
                instrumentalist_count = 0;
                tick = 0;
                rehearsal_running = 0;
            }
        }

        // 2. RUN THE TICKING
        if (rehearsal_running && instrumentalist_count > 0)
        {        
            write(pipefd[1], &tick, sizeof(int));
            // send tick to all instrumentalists
            for (int i = 0; i < instrumentalist_count; i++)
            {   
                // client disconnected -> remove it
                if (write(instrumentalists[i], &tick, sizeof(int)) <= 0)
                {
                    close(instrumentalists[i]);

                    for (int j = i; j < instrumentalist_count - 1; j++) // left shift
                        instrumentalists[j] = instrumentalists[j + 1];

                    instrumentalist_count--;
                    i--;
                }
            }
        
            printf("[SERVER] TICK %d sent to %d Instrumentalist(s)\n", tick, instrumentalist_count);
            tick++;
            sleep(1);
        }
        else
            sleep(1);
    }
}