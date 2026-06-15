#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "menu.h"

struct Note
{
    int time;
    char note[10];
};

void view_piece_list()
{
    system("ls pieces");
}

int piece_exists(char* name)
{
    char path[200];
    sprintf(path, "pieces/%s", name);

    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode)); // exists and is a directory
}

void add_piece()
{
    char name[100];
    printf("\nEnter Piece Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    if (piece_exists(name))
    {
        printf("\nPiece Exists.\n");
        return;
    }

    char cmd[200];
    sprintf(cmd, "mkdir -p pieces/%s", name);
    system(cmd);
}

void choose_piece(int role)
{
    printf("\nPieces to Choose From: \n");
    view_piece_list();
    
    char name[100];
    printf("\nEnter Piece Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    if (!piece_exists(name)) 
    {
        printf("\nPiece Does Not Exist.\n");
        return;
    }

    piece_menu(name, role);
}

void delete_piece()
{
    printf("\nPieces to Choose From: \n");
    view_piece_list();
    
    char name[100];
    printf("\nEnter piece name to delete: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    if (!piece_exists(name)) 
    {
        printf("\nPiece Does Not Exist.\n");
        return;
    }

    char cmd[200];
    sprintf(cmd, "rm -r pieces/%s", name);
    system(cmd);

    printf("\nPiece Deleted.\n");
}

void view_part_list(char* piece)
{
    char cmd[200];
    sprintf(cmd, "ls pieces/%s", piece);
    system(cmd);
}

int part_exists(char* piece, char* name)
{
    char path[200];
    sprintf(path, "pieces/%s/%s", piece, name);

    struct stat st;
    return (stat(path, &st) == 0 && S_ISREG(st.st_mode));
}

void add_part(char* piece)
{
    char name[100];
    printf("\nEnter part name (e.g., piano.txt): ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    if (part_exists(piece, name)) 
    {
        printf("\nPart Exists.\n");
        return;
    }

    char path[200];
    sprintf(path, "pieces/%s/%s", piece, name);

    FILE* f = fopen(path, "w");
    if (!f) 
    {
        printf("\nError Creating Part.\n");
        return;
    }
    fclose(f);
}

void view_part(char* piece)
{
    char name[100];
    printf("\nEnter Part Name (e.g., piano.txt): ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    if (!part_exists(piece, name)) 
    {
        printf("\nPart Does Not Exist.\n");
        return;
    }

    char path[200];
    sprintf(path, "pieces/%s/%s", piece, name);

    FILE* f = fopen(path, "r");
    if (!f) 
    {
        printf("\nFile Not Found\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), f))
        printf("%s", line);

    fclose(f);
}

void append_note(char* piece)
{
    printf("\nParts to Choose From: \n");
    view_part_list(piece);
    
    char part[100];
    printf("\nEnter Part Name: ");
    fgets(part, sizeof(part), stdin);
    part[strcspn(part, "\n")] = '\0';

    if (!part_exists(piece, part)) 
    {
        printf("\nPart Does Not Exist.\n");
        return;
    }

    char path[200];
    sprintf(path, "pieces/%s/%s", piece, part);

    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd < 0) 
    {
        perror("File Open Failed");
        return;
    }

    // FILE LOCKING for the append (lock before modifying)
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET; // lock from the start of the file
    lock.l_start = 0;
    lock.l_len = 0; // whole file locked
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);

    char note[100];
    printf("\nEnter Note (e.g., '3 G4'): ");
    fgets(note, sizeof(note), stdin);
    write(fd, note, strlen(note));

    // Unlock
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}

void delete_part(char* piece) 
{
    printf("\nParts to Choose From: \n");
    view_part_list(piece);
    
    char part[100];
    printf("\nEnter Part Name: ");
    fgets(part, sizeof(part), stdin);
    part[strcspn(part, "\n")] = '\0';

    if (!part_exists(piece, part)) 
    {
        printf("\nPart Does Not Exist.\n");
        return;
    }

    char path[200];
    sprintf(path, "pieces/%s/%s", piece, part);

    remove(path);
}

void start_rehearsal(char* piece) 
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server)); 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // note: local for now
    server.sin_port = htons(8080);

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) 
    {
        printf("\nConnection failed.\n");
        return;
    }

    write(sockfd, "START", 5);

    close(sockfd);
}

void stop_rehearsal(char* piece) 
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server)); 

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // note: local for now
    server.sin_port = htons(8080);

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) 
    {
        printf("\nConnection Failed.\n");
        return;
    }

    write(sockfd, "STOP", 4);
    close(sockfd);
}


int load_notes(char* path, struct Note notes[], int max_notes)
{
    FILE* f = fopen(path, "r");
    if (!f) 
    {
        printf("\nError Opening File.\n");
        return -1;
    }

    int count = 0;  
    while (count < max_notes && fscanf(f, "%d %s", &notes[count].time, notes[count].note) == 2)
        count++;

    fclose(f);

    if (count == 0) 
    {
        printf("\nNo Notes to Play.\n");
        return 0;
    }
    return count;
}

void handle_tick(struct Note notes[], int count, int tick)
{
    int printed = 0;

    for (int i = 0; i < count; i++)
    {
        if (notes[i].time == tick)
        {
            if (!printed) 
            {
                printf("[TICK %d]: ", tick);
                printed = 1;
            }
            printf("%s ", notes[i].note);
        }
    }
    
    if (printed)
        printf("\n");
}

void join_rehearsal(char* piece) 
{
    printf("\nParts to Choose From: \n");
    view_part_list(piece);

    char part[100];
    printf("\nEnter Part Name: ");
    fgets(part, sizeof(part), stdin);
    part[strcspn(part, "\n")] = '\0';

    if (!part_exists(piece, part)) 
    {
        printf("\nPart Does Not exist.\n");
        return;
    }

    // Socket connection to server (that gives ticks)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server)); 

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // note: local for now
    server.sin_port = htons(8080);

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) 
    {
        printf("\nConnection Failed.\n");
        return;
    }

    // Sends a REQUEST, server will SEND THE FILE to the client
    char request[512];
    sprintf(request, "JOIN %s %s", piece, part);
    write(sockfd, request, strlen(request));

    int size;

    if (read(sockfd, &size, sizeof(int)) <= 0) 
    {
        printf("\nServer Didn't Send Size\n");
        return;
    }

    if (size <= 0)
    {
        printf("\nInvalid File\n");
        return;
    }

    char temp_name[50];
    sprintf(temp_name, "temp_%d.txt", getpid());
    FILE* temp = fopen(temp_name, "w");

    for (int i = 0; i < size; i++)
    {
        char c;
        if (read(sockfd, &c, 1) <= 0)
        {
            printf("\nFile Transfer Interrupted\n");
            return;
        }
        fputc(c, temp);
    }
    
    fclose(temp);
    printf("\nReady.\n");

    struct Note notes[300];
    int count = load_notes(temp_name, notes, 300);
    if (count <= 0) 
        return;

    int max_tick = 0;
    for (int i = 0; i < count; i++)
    {
        if (notes[i].time > max_tick)
            max_tick = notes[i].time;
    }

    int tick;
    while (read(sockfd, &tick, sizeof(int)) > 0)
    {
        handle_tick(notes, count, tick);
        if (tick > max_tick)
            break;
    }
    printf("\nRehearsal done\n");
    close(sockfd);
    remove(temp_name);
}