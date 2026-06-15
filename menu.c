#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "menu.h"
#include "file_ops.h"

#define ADMIN 1
#define USER 2

void admin_menu()
{
    int choice;

    while (1)
    {
        printf("\n--- Admin Menu ---\n");

        printf("1. View Piece List\n");
        printf("2. Create Piece\n");
        printf("3. Choose Piece\n");
        printf("4. Delete Piece\n");
        printf("5. Exit\n");

        printf("\nEnter Number to Proceed: ");
        
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
            case 1:
                view_piece_list();
                break;
            case 2:
                add_piece();
                break;
            case 3:
                choose_piece(ADMIN);
                break;
            case 4:
                delete_piece();
                break;
            case 5:
                printf("Exiting... \n");
                exit(0);
            default:
                printf("\nEnter a Number Between 1 and 5.\n");
        }
    }
}

void user_menu()
{   
    int choice;

    while (1)
    {
        printf("\n--- Main Menu ---\n");

        printf("1. View Piece List\n");
        printf("2. Choose Piece\n");
        printf("3. Exit\n");

        printf("\nEnter Number to Proceed: ");

        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
            case 1:
                view_piece_list();
                break;
            case 2:
                choose_piece(USER);
                break;
            case 3:
                printf("Exiting... \n");
                exit(0);
            default:
                printf("\nEnter a Number Between 1 and 3.\n");
        }
    }
}


void piece_menu(char* piece_name, int role) 
{
    int choice;

    while(1)
    {
        printf("\n--- %s ---\n", piece_name);

        if (role == ADMIN)
        {
            printf("1. View Part List\n");
            printf("2. Create Part\n");
            printf("3. View Part\n");
            printf("4. Edit Part (Append Note)\n");
            printf("5. Delete Part\n");
            printf("6. Start Rehearsal\n");
            printf("7. Stop Rehearsal\n");
            printf("8. Back\n");
        }
        else
        {
            printf("1. View Part List\n");
            printf("2. View Part\n");
            printf("3. Edit Part (Append Note)\n");
            printf("4. Join Rehearsal\n");
            printf("5. Back\n");
        }

        printf("\nEnter Number to Proceed: ");
        scanf("%d", &choice);
        getchar();

        if (role == ADMIN)
        {
            switch (choice) 
            {
                case 1: 
                    view_part_list(piece_name);
                    break;
                case 2: 
                    add_part(piece_name); 
                    break;
                case 3: 
                    view_part(piece_name);
                    break;
                case 4: 
                    append_note(piece_name);
                    break;
                case 5: 
                    delete_part(piece_name);
                    break;
                case 6: 
                    start_rehearsal(piece_name);
                    break;
                case 7: 
                    stop_rehearsal(piece_name);
                    break;
                case 8: 
                    return;
                default:
                    printf("\nEnter a Number Between 1 and 8\n");
            }
        }

        else
        {
            switch (choice) 
            {
                case 1: 
                    view_part_list(piece_name);
                    break;
                case 2: 
                    view_part(piece_name);
                    break;
                case 3: 
                    append_note(piece_name);
                    break;
                case 4: 
                    join_rehearsal(piece_name);
                    break;
                case 5: 
                    return;
                default:
                    printf("\nEnter a Number Between 1 and 5\n");
            }
        }
    }
}