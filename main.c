#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum // selecting the scheduling method
{
    NONE, // No Scheduling method
    FCFS, // First Come, First Served
    SJF,  // Shortest-Job-First
    PS,   // priority scheduling
    RRS   // Round-Robin scheduling
} ScheduleMethod;

struct Process
{
    int brust_time;
    int arrival_time;
    int priority;
    int wait_time;

    struct Process *next; // pointer to next job.
};

struct InputChar
{
    char value;
    struct InputChar *next;
};

struct Run
{
    ScheduleMethod Scheduling_Method;
    int Preemtive_Mode;
    float Avg_Wait_Time; // average wating time for all jobs
};
FILE *openFile(const char fileName[])
{
    FILE *file = fopen(fileName, "r");

    if (file == NULL)
    {
        return NULL;
    }

    return file;
}
struct Process *readProcesses(FILE *InputFile)
{
    struct InputChar *head_input = (struct InputChar *)malloc(sizeof(struct InputChar));    // store the first input
    struct InputChar *current_input = (struct InputChar *)malloc(sizeof(struct InputChar)); // store the current input

    struct Process *process_head = (struct Process *)malloc(sizeof(struct Process)); // store head of processes
    struct Process *process_current;                                                 // stores current process
    struct Process *process_temp = NULL;

    char temp_ch; // store the character

    struct InputChar *temp_input; // store the temp input

    head_input = NULL; // the head is NULL
    int col = 0;       // the col index

    process_head = NULL;
    // start reading the Input file and extract data
    /*
     * It reads each character from file.
     * The file is divided by columns in this way:-
     * Example P1 = 5:0:3
     * col 1(Brust Time): 5
     * col 2(Arrival Time): 0
     * col 3(Priority): 3
     */
    for (temp_ch = fgetc(InputFile); (temp_ch != EOF || col >= 2); temp_ch = fgetc(InputFile))
    {
        // if (temp_ch == 10) // new line == new process
        // {
        //     process_current = process_head;
        // }

        // if the char is not a delmiter && not a '\n' && not EOF(it could be EOF if the last col in file)
        if (temp_ch != 58 && temp_ch != 10 && temp_ch != EOF) // 58 == ':' && 10 == '\n'
        {
            if (head_input == NULL) // if first letter
            {
                head_input = (struct InputChar *)malloc(sizeof(struct InputChar));
                head_input->value = temp_ch;
                head_input->next = NULL;
                current_input = head_input;
            }
            else
            {
                temp_input = (struct InputChar *)malloc(sizeof(struct InputChar));
                temp_input->value = temp_ch;
                temp_input->next = NULL;

                current_input->next = temp_input;
                current_input = temp_input;
            }
        }
        else if (temp_ch == 58 || temp_ch == 10 || temp_ch == EOF) // if delimter || \n || EOF means the col is finshed
        {
            if (process_temp == NULL)
            {
                process_temp = (struct Process *)malloc(sizeof(struct Process));
                process_temp->next = NULL;
            }

            temp_input = head_input;

            if (col == 0)
            {
                process_temp->brust_time = temp_input->value - '0';
                // printf("Brust time: %d\n", process_temp->brust_time);
            }
            else if (col == 1)
            {
                // printf("Arrival time: %c\n", temp_input->value);
                process_temp->arrival_time = temp_input->value - '0';
            }
            else if (col == 2)
            {
                // printf("Protiry : %c\n", temp_input->value);
                process_temp->priority = temp_input->value - '0';

                // printf("%d", process_temp->brust_time);
                if (process_head == NULL)
                {
                    process_head = process_temp;
                    process_current = process_head;
                    // printf("Brust time: %d\n", process_temp->brust_time);
                }
                else
                {
                    process_current->next = process_temp;
                    process_current = process_temp;
                }

                process_temp = NULL;
            }
            col++;
            head_input = NULL;

            if (col > 2)
            {
                col = 0;
                // printf("\n");
            }
        }
    }

    return process_head;
}
const char *get_scheduling_method_name(ScheduleMethod method)
{
    SJF,    // Shortest-Job-First
        PS, // priority scheduling
        RRS;
    switch (method)
    {
    case NONE:
        return "None";
        break;
    case FCFS:
        return "FCFS";
        break;
    case SJF:
        return "SJF";
        break;
    case PS:
        return "PS";
        break;
    case RRS:
        return "RRS";
        break;
    default:
        break;
    }
}

int main(int argc, char const *argv[])
{
    int i; // for iterations;
    char *tempChars;
    tempChars = (char *)malloc(10 * sizeof(char));

    FILE *InputFile; //-f input file
    FILE *OuputFile; //-o output file

    int choice; // user choice

    struct Process *process_head = (struct Process *)malloc(sizeof(struct Process)); // store head of processes
    struct Process *process_current;                                                 // stores current process
    int PreemptiveMode = 0;                                                          // 1: PreemptiveMode, 0: Non-preemptiveMode

    if (argc <= 1) // check if any command arguments passed
    {
        printf("No input & output files");
        return -1;
    }

    // read the passed files, and create linkedList of processes in input.txt
    for (i = 0; i < argc; i++) // loop through the passed args -f and -o
    {
        if (strcmp(argv[i], "-f") == 0) // handel the -f file
        {
            InputFile = openFile(argv[i + 1]); // open input file

            if (InputFile == NULL) // check if not opened
            {
                printf("%s: Can't open the input file", argv[i + 1]);
                return -1;
            }

            process_head = readProcesses(InputFile);
        }

        if (strcmp(argv[i], "-o") == 0) // handle the -o file
        {
            // printf("output: %s", argv[i + 1]);
        }
    }

    struct Run *run = (struct Run *)malloc(sizeof(struct Run));
    run->Scheduling_Method = FCFS;
    run->Preemtive_Mode = 0;

    while (1)
    {
        system("cls");

        printf("\t\t-CPU Scheduler Simulator-\t\t\n");
        printf("1) Scheduling Method (%s)\n", get_scheduling_method_name(run->Scheduling_Method));
        printf("2) Preemptive Mode (%s)\n", (run->Preemtive_Mode == 0 ? "Off" : "On"));
        printf("3) Show Result \n");
        printf("4) End Program \n");
        printf("Option (q to exit)> ");
        scanf(" %c", &choice);

        switch (choice)
        {
        case '1':
            printf("sch");
            // run->Scheduling_Method = Set_Scheduling_Method();
            break;
        case '2':
            printf("pree");
            // run->Preemptive = Set_Preemtive_Mode();
            break;
        case '3':
            printf("Show");
            // Show_Result();
            break;
        case '4':
            printf("end");
            // End_Program();
            break;
        case 'q':
            exit(0);
            break;
        default:
            printf("Wrong Option");
            // choice = 0;
            break;
        }

        scanf("%s");
    }

    // free the allocated memory
    free(tempChars);
    // free(head_input);

    // free(current_input);

    // free(temp_input);

    free(process_head);

    // free(process_current);
    // free(head_input);
    // free(current_input);
    // free(temp_input);
    // free(process_head);
    // free(process_current);
    // free(head_input);
    // free(temp_input);
    fclose(InputFile);
    return 0;
}
