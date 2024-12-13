#include<stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
        int roll_number;
        char name[50];
} Student;

Student List_student[10];
int student_count = 0;

void initialize_students() {
     List_student[0] = (Student){1, "Nayan"};
     List_student[1] = (Student){2, "Egypta"};
     List_student[2] = (Student){3, "Minhaz"};
     List_student[3] = (Student){4, "Dibesh"};
     List_student[4] = (Student){5, "Sanjukta"};
     List_student[5] = (Student){6, "Priya"};
     List_student[6] = (Student){7, "Akhim"};
     List_student[7] = (Student){8, "Partha"};
     List_student[8] = (Student){9, "Ashish"};
     List_student[9] = (Student){10,"Subrojyoti"};
                         
     student_count = 10;
}

void disp(){
    int i;
    for(i=0; i<student_count; i++){
        printf("%d",List_student[i].roll_number);
        printf("\n");
    }
}    
void delete_student(int roll_number) {
     int found = 0;
     int i,j;
     for ( i = 0; i < student_count; i++) {
         if (List_student[i].roll_number == roll_number) {
             found =1;
             for ( j = i; j < student_count - 1; j++) {
                 List_student[j] = List_student[j + 1];
            }
            student_count--;
            break;
        }
     }
     if (found) {
         printf("Deleted student with roll number %d\n", roll_number);
     } else {
         printf("Student with roll number %d not found\n", roll_number);
     }
}
int main() {
        int server_fd, new_socket;
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        char buffer[BUFFER_SIZE] = {0};

        initialize_students();

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            printf("Socket failed");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("Bind failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0) {
              perror("Listen failed");
              close(server_fd);
              exit(EXIT_FAILURE);
        }
        printf("Server is running and waiting for connections...\n");

       while (1) {
           if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
               perror("Accept failed");
               continue;
           }
           read(new_socket, buffer, BUFFER_SIZE);
           int roll_number = atoi(buffer);
           printf("Students :\n");disp();
           printf("Received roll number: %d\n", roll_number);
           delete_student(roll_number);
           printf("Students :\n"); disp(); 
           close(new_socket); 
      }
      return 0;
}

