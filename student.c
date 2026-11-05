#include <stdio.h>
#include <string.h>

struct Student {
    char name[50];
    int age;
    float marks;
};
void add_student(struct Student students[], int *count) {
	if (*count >= 100) {
    		printf("Student limit reached.\n");
    		return;
	}
	printf("Enter student name: ");
	scanf(" %[^\n]", students[*count].name);

	printf("Enter your age: ");
	scanf("%d", &students[*count].age);
	
	printf("Enter student marks: ");
	scanf("%f", &students[*count].marks);

	(*count)++;

	printf("Student added successfully!\n");
}

void view_students(struct Student students[], int count) {
	if (count == 0) {
		printf("No students yet.\n");
		return;
	}
	for (int i=0 ;i < count ; i++) {
		printf("\nStudent %d\n", i + 1);
		printf("Name : %s\n", students[i].name);
		printf("Age : %d\n", students[i].age);
		printf("Marks : %.2f\n", students[i].marks);
		
		if (students[i].marks >= 40) {
			printf("Result : Passed\n");
		}
		else {
			printf("Result : Failed\n");
		}
	}
}
void search_student(struct Student students[], int count) {
    char searchName[50];

    printf("Enter student name to search: ");
    scanf(" %[^\n]", searchName);

    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].name, searchName) == 0) {
            printf("\nStudent found!\n");
            printf("Name: %s\n", students[i].name);
            printf("Age: %d\n", students[i].age);
            printf("Marks: %.2f\n", students[i].marks);

            if (students[i].marks >= 40) {
                printf("Result: Pass\n");
            } else {
                printf("Result: Fail\n");
            }

            return;
        }
    }

    printf("Student not found.\n");
}

void update_student(struct Student students[], int count) {
    char searchName[50];

    printf("Enter student name to update: ");
    scanf(" %[^\n]", searchName);

    for (int i = 0; i < count; i++) {

        if (strcmp(students[i].name, searchName) == 0) {

            printf("Enter new age: ");
            scanf("%d", &students[i].age);

            printf("Enter new marks: ");
            scanf("%f", &students[i].marks);

            printf("\nStudent updated successfully!\n");

            printf("Name : %s\n", students[i].name);
            printf("Age : %d\n", students[i].age);
            printf("Marks : %.2f\n", students[i].marks);

            if (students[i].marks >= 40) {
                printf("Result : Passed\n");
            }
            else {
                printf("Result : Failed\n");
            }

            return;
        }
    }

    printf("Student not found.\n");
}
void delete_student(struct Student students[], int *count) {
    char searchName[50];

    printf("Enter student name to delete: ");
    scanf(" %[^\n]", searchName);

    for (int i = 0; i < *count; i++) {

        if (strcmp(students[i].name, searchName) == 0) {

            for (int j = i; j < *count - 1; j++) {
                students[j] = students[j + 1];
            }

            (*count)--;

            printf("Student deleted successfully!\n");
            return;
        }
    }

    printf("Student not found.\n");
}

void save_students(struct Student students[], int count) {
    FILE *file = fopen("students.dat", "wb");

    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    fwrite(students, sizeof(struct Student), count, file);

    fclose(file);

    printf("Students saved successfully.\n");
}
void load_students(struct Student students[], int *count) {
    FILE *file = fopen("students.dat", "rb");

    if (file == NULL) {
        *count = 0;
        return;
    }

    *count = fread(students, sizeof(struct Student), 100, file);

    fclose(file);
}


int main() {
    struct Student students[100];
    int count = 0;
    int choice;
    load_students(students, &count);
    while (1) {
	    printf("\n-----STUDENT MANAGEMENT SYSTEM-------\n");
	    printf("1. Add student.\n");
	    printf("2. View students.\n");
	    printf("3. Search student.\n");
	    printf("4. Update student.\n");
	    printf("5. Delete student.\n");
	    printf("6. Exit.\n");



	    printf("Please make a choice from 1 to 6: ");
	    scanf("%d", &choice);

	    if (choice == 1) {
		    add_student(students, &count);
		    save_students(students, count);
	    }
	    else if (choice == 2) {
		    view_students(students, count);
	    }
	    else if (choice == 3) {
		    search_student(students, count);
	    }
	    else if (choice == 4) {
		    update_student(students, count);
		    save_students(students, count);
	    }
	    else if (choice == 5) {
		    delete_student(students, &count);
		    save_students(students, count);
	    }
	    else if (choice == 6) {
		    printf("Thank you. Please visit again.Goodbye!\n");
		    break;
	    }
	    else {
		    printf("Invalid choice. Please try again.");
	    }
    }
    return 0;

}
