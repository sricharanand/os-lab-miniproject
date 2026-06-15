#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define ADMIN 1
#define USER 2

void admin_menu();
void user_menu();

int main(void)
{
    printf("\n--- MUSIC PERFORMANCE SIMULATOR ---\n");

    printf("\n--- Main Menu ---\n");
    
    printf("1. Conductor (admin)\n");
    printf("2. Instrumentalist (user)\n");

    printf("\nSelect Role: ");
    int role;
    scanf("%d", &role);

    if (role == ADMIN)
        admin_menu();
    else if (role == USER)
        user_menu();
}