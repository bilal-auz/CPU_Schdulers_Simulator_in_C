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
    int pid;
    int brust_time;
    int arrival_time;
    int priority;
    int wait_time;
    int remaining_brust_time;
    int last_point;

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
    int Preemtive_Mode;  // 0: off, 1: on
    float Avg_Wait_Time; // average wating time for all jobs
    int total_brust;     // total brust time for processes
};

struct File
{
    FILE *file;
    char const *file_name;
};

FILE *openFile(const char fileName[], const char *mode);

int countInputs(struct InputChar *input_head);

struct Process *readProcesses(FILE *InputFile);

struct Process *init_the_inputs(int argc, char const *argv[], struct File *InputFile, struct File *OuputFile);

struct Process *sortProcessesByPID(struct Process *process_head);

// binary sorting the processes based on arrival time
struct Process *sortProcesses(struct Process *process_head);

struct Process *sort_processes_short_brust(struct Process *process_head);

struct Process *sort_burst_time(struct Process *process_head, int progress);

struct Process *sort_burst_time_Preemptive(struct Process *process_head, int progress);

struct Process *sort_arrival_and_priority(struct Process *process_head);

struct Process *sort_priority_time(struct Process *process_head, int progress);

struct Process *remove_process(struct Process *process_head, int pid);

const char *get_scheduling_method_name(ScheduleMethod method);

int has_next(struct Process *process_head, int progress);

ScheduleMethod Set_Scheduling_Method();

int Set_Preemtive_Mode(ScheduleMethod currentMethod);

// caculate the brust time of all processes
int calculate_brust_time(struct Process *process_head);

int calculate_current_brust_time(struct Process *process_head);

// caculate the avg wating time for all the processes(called after going through the scheduler)
float calculate_avg_wating_time(struct Process *process_head);

// FCFS main scheduler function
void First_Come_First_Served(struct Run *run, struct Process *process_head);

// SJF main schedulers
void Short_Job_First(struct Run *run, struct Process *process_head);
void Short_Job_First_Preemptive(struct Run *run, struct Process *process_head);

// PS main schedulers
void Priority_Scheduler(struct Run *run, struct Process *process_head);
void Priority_Scheduler_Preemptive(struct Run *run, struct Process *process_head);

// RR
void Round_Robin_Scheduler(struct Run *run, struct Process *process_head);

// Option 3: show results. Return 0 if problem, and 1 if successfully finished
int Show_Results(struct Run *run, struct Process *process_head);

// Option 4: Save results to Output File
int Save_Results_To_File(struct Run *run, struct File *OutputFile, struct Process *process_head);

int main(int argc, char const *argv[])
{
    char choice; // user choice
    char *tempChars;
    tempChars = (char *)malloc(10 * sizeof(char));

    struct File *InputFile = (struct File *)malloc(sizeof(struct File)); //-f input file
    struct File *OuputFile = (struct File *)malloc(sizeof(struct File)); //-o output file

    struct Process *process_head;    // store head of processes
    struct Process *process_current; // stores current process

    if (argc <= 1) // check if any command arguments passed
    {
        printf("No input & output files");
        return -1;
    }

    // read the passed files, and create linkedList of processes in input.txt
    process_head = init_the_inputs(argc, argv, InputFile, OuputFile);

    struct Run *run = (struct Run *)malloc(sizeof(struct Run));
    run->Scheduling_Method = NONE; // default  NONE
    run->Preemtive_Mode = 0;       // default 0

    while (1)
    {
        system("clear");

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
            run->Scheduling_Method = Set_Scheduling_Method();

            run->Scheduling_Method == FCFS ? run->Preemtive_Mode = 0 : 1;
            run->Scheduling_Method == RRS ? run->Preemtive_Mode = 1 : 0;

            break;
        case '2':
            run->Preemtive_Mode = Set_Preemtive_Mode(run->Scheduling_Method);
            break;
        case '3':
            if (Show_Results(run, process_head) == 0)
            {
                printf("Choose a scheduling method!");
            }

            printf("\n\nPress any key to go back to menu..");
            getchar();
            getchar();
            break;
        case '4':
            // if error (return 0) break, otherwise write the results to file
            if (Show_Results(run, process_head) == 0)
            {
                printf("Choose a scheduling method!");
                printf("\n\nPress any key to go back to menu..");
                getchar();
                getchar();
                break;
            }

            Save_Results_To_File(run, OuputFile, process_head);
            // no break because after this option exit program(option q);
        case 'q':
            exit(0);
            break;
        default:
            printf("Wrong Option");
            break;
        }

        process_head = init_the_inputs(argc, argv, InputFile, OuputFile);
    }

    // free the allocated memory
    free(process_head);

    // close files
    fclose(InputFile->file);
    fclose(OuputFile->file);
    return 0;
}

FILE *openFile(const char fileName[], const char *mode)
{
    FILE *file = fopen(fileName, mode);

    if (file == NULL)
    {
        return NULL;
    }

    return file;
}

int countInputs(struct InputChar *input_head)
{
    int count = 0;
    struct InputChar *input_current = input_head;

    while (input_current != NULL)
    {
        count++;
        input_current = input_current->next;
    }

    return count;
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
    int count = 0;     // counter to assign pid values

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
                process_temp = (struct Process *)malloc(sizeof(struct Process)); // new process
                process_temp->pid = ++count;                                     // assing number to the process
                process_temp->wait_time = 0;
                process_temp->next = NULL;
            }

            int size = countInputs(head_input);

            char *one_number = (char *)malloc(size * sizeof(char));

            temp_input = head_input;

            int count = 0;
            while (temp_input != NULL)
            {
                one_number[count++] = temp_input->value;

                temp_input = temp_input->next;
            }

            if (col == 0)
            {
                process_temp->brust_time = atoi(one_number);
                process_temp->remaining_brust_time = atoi(one_number);
            }
            else if (col == 1)
            {
                process_temp->arrival_time = atoi(one_number);
                process_temp->last_point = atoi(one_number);
            }
            else if (col == 2)
            {
                process_temp->priority = atoi(one_number);

                if (process_head == NULL)
                {
                    process_head = process_temp;
                    process_current = process_head;
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
            }
        }
    }

    free(head_input);
    free(current_input);

    return process_head;
}

struct Process *init_the_inputs(int argc, char const *argv[], struct File *InputFile, struct File *OuputFile)
{
    int i;
    struct Process *process_head = (struct Process *)malloc(sizeof(struct Process)); // store head of processes

    for (i = 0; i < argc; i++) // loop through the passed args -f and -o
    {
        if (strcmp(argv[i], "-f") == 0) // handel the -f file
        {
            InputFile->file_name = argv[i + 1];
            InputFile->file = openFile(argv[i + 1], "r"); // open input file

            if (InputFile->file == NULL) // check if not opened
            {
                printf("%s: Can't open the input file", argv[i + 1]);
                exit(-1);
            }

            process_head = readProcesses(InputFile->file);
        }

        if (strcmp(argv[i], "-o") == 0) // handle the -o file
        {
            OuputFile->file_name = argv[i + 1];
        }
    }

    fclose(InputFile->file);
    return process_head;
}

struct Process *sortProcessesByPID(struct Process *process_head)
{
    struct Process *process_current;
    struct Process *process_next;

    process_current = process_head;

    while (process_current != NULL)
    {
        process_next = process_current->next;
        while (process_next != NULL)
        {
            if (process_current->pid > process_next->pid)
            {
                int pid, brust_time, arrival_time, priority, wait_time, remaining_brust_time, last_point;

                pid = process_current->pid;
                brust_time = process_current->brust_time;
                arrival_time = process_current->arrival_time;
                priority = process_current->priority;
                wait_time = process_current->wait_time;
                remaining_brust_time = process_current->remaining_brust_time;
                last_point = process_current->last_point;

                process_current->pid = process_next->pid;
                process_current->arrival_time = process_next->arrival_time;
                process_current->brust_time = process_next->brust_time;
                process_current->priority = process_next->priority;
                process_current->wait_time = process_next->wait_time;
                process_current->remaining_brust_time = process_next->remaining_brust_time;
                process_current->last_point = process_next->last_point;

                process_next->pid = pid;
                process_next->arrival_time = arrival_time;
                process_next->brust_time = brust_time;
                process_next->priority = priority;
                process_next->wait_time = wait_time;
                process_next->remaining_brust_time = remaining_brust_time;
                process_next->last_point = last_point;
            }
            process_next = process_next->next;
        }
        process_current = process_current->next;
    }

    return process_head;
}

struct Process *sortProcesses(struct Process *process_head)
{
    struct Process *process_current;
    struct Process *process_next;

    process_current = process_head;

    while (process_current != NULL)
    {
        process_next = process_current->next;
        while (process_next != NULL)
        {
            if (process_current->arrival_time > process_next->arrival_time)
            {
                int pid, brust_time, arrival_time, priority, wait_time, remaining_brust_time, last_point;

                pid = process_current->pid;
                brust_time = process_current->brust_time;
                arrival_time = process_current->arrival_time;
                priority = process_current->priority;
                wait_time = process_current->wait_time;
                remaining_brust_time = process_current->remaining_brust_time;
                last_point = process_current->last_point;

                process_current->pid = process_next->pid;
                process_current->arrival_time = process_next->arrival_time;
                process_current->brust_time = process_next->brust_time;
                process_current->priority = process_next->priority;
                process_current->wait_time = process_next->wait_time;
                process_current->remaining_brust_time = process_next->remaining_brust_time;
                process_current->last_point = process_next->last_point;

                process_next->pid = pid;
                process_next->arrival_time = arrival_time;
                process_next->brust_time = brust_time;
                process_next->priority = priority;
                process_next->wait_time = wait_time;
                process_next->remaining_brust_time = remaining_brust_time;
                process_next->last_point = last_point;
            }
            process_next = process_next->next;
        }
        process_current = process_current->next;
    }

    return process_head;
}

struct Process *sort_processes_short_brust(struct Process *process_head)
{
    struct Process *process_current;
    struct Process *process_next;

    process_current = process_head;

    while (process_current != NULL)
    {
        process_next = process_current->next;
        while (process_next != NULL)
        {
            if ((process_current->arrival_time > process_next->arrival_time) ||
                (process_current->arrival_time == process_next->arrival_time && process_current->brust_time > process_next->brust_time))
            {
                int pid, brust_time, arrival_time, priority, wait_time, remaining_brust_time, last_point;

                pid = process_current->pid;
                brust_time = process_current->brust_time;
                arrival_time = process_current->arrival_time;
                priority = process_current->priority;
                wait_time = process_current->wait_time;
                remaining_brust_time = process_current->remaining_brust_time;
                last_point = process_current->last_point;

                process_current->pid = process_next->pid;
                process_current->arrival_time = process_next->arrival_time;
                process_current->brust_time = process_next->brust_time;
                process_current->priority = process_next->priority;
                process_current->wait_time = process_next->wait_time;
                process_current->remaining_brust_time = process_next->remaining_brust_time;
                process_current->last_point = process_next->last_point;

                process_next->pid = pid;
                process_next->arrival_time = arrival_time;
                process_next->brust_time = brust_time;
                process_next->priority = priority;
                process_next->wait_time = wait_time;
                process_next->remaining_brust_time = remaining_brust_time;
                process_next->last_point = last_point;
            }
            process_next = process_next->next;
        }
        process_current = process_current->next;
    }

    return process_head;
}

struct Process *sort_burst_time(struct Process *process_head, int progress)
{
    struct Process *process_current;
    struct Process *process_next;

    process_current = process_head;

    while (process_current != NULL)
    {
        process_next = process_current->next;
        while (process_next != NULL)
        {
            if ((process_current->brust_time > process_next->brust_time &&
                 process_current->arrival_time < process_next->arrival_time &&
                 process_next->arrival_time <= progress) ||
                (process_current->brust_time == process_next->brust_time && process_current->arrival_time > process_next->arrival_time))
            {
                int pid, brust_time, arrival_time, priority, wait_time, remaining_brust_time, last_point;

                pid = process_current->pid;
                brust_time = process_current->brust_time;
                arrival_time = process_current->arrival_time;
                priority = process_current->priority;
                wait_time = process_current->wait_time;
                remaining_brust_time = process_current->remaining_brust_time;
                last_point = process_current->last_point;

                process_current->pid = process_next->pid;
                process_current->arrival_time = process_next->arrival_time;
                process_current->brust_time = process_next->brust_time;
                process_current->priority = process_next->priority;
                process_current->wait_time = process_next->wait_time;
                process_current->remaining_brust_time = process_next->remaining_brust_time;
                process_current->last_point = process_next->last_point;

                process_next->pid = pid;
                process_next->arrival_time = arrival_time;
                process_next->brust_time = brust_time;
                process_next->priority = priority;
                process_next->wait_time = wait_time;
                process_next->remaining_brust_time = remaining_brust_time;
                process_next->last_point = last_point;
            }
            process_next = process_next->next;
        }
        process_current = process_current->next;
    }

    return process_head;
}

struct Process *sort_burst_time_Preemptive(struct Process *process_head, int progress)
{
    struct Process *process_current;
    struct Process *process_next;

    process_current = process_head;

    while (process_current != NULL)
    {
        process_next = process_current->next;
        while (process_next != NULL)
        {
            if ((process_current->remaining_brust_time > process_next->remaining_brust_time && process_next->arrival_time <= progress) ||
                (process_current->remaining_brust_time == process_next->remaining_brust_time && process_current->arrival_time > process_next->arrival_time))
            {
                int pid, brust_time, arrival_time, priority, wait_time, remaining_brust_time, last_point;

                pid = process_current->pid;
                brust_time = process_current->brust_time;
                arrival_time = process_current->arrival_time;
                priority = process_current->priority;
                wait_time = process_current->wait_time;
                remaining_brust_time = process_current->remaining_brust_time;
                last_point = process_current->last_point;

                process_current->pid = process_next->pid;
                process_current->arrival_time = process_next->arrival_time;
                process_current->brust_time = process_next->brust_time;
                process_current->priority = process_next->priority;
                process_current->wait_time = process_next->wait_time;
                process_current->remaining_brust_time = process_next->remaining_brust_time;
                process_current->last_point = process_next->last_point;

                process_next->pid = pid;
                process_next->arrival_time = arrival_time;
                process_next->brust_time = brust_time;
                process_next->priority = priority;
                process_next->wait_time = wait_time;
                process_next->remaining_brust_time = remaining_brust_time;
                process_next->last_point = last_point;
            }
            process_next = process_next->next;
        }
        process_current = process_current->next;
    }

    return process_head;
}

struct Process *sort_arrival_and_priority(struct Process *process_head)
{
    struct Process *process_current;
    struct Process *process_next;

    process_current = process_head;

    while (process_current != NULL)
    {
        process_next = process_current->next;
        while (process_next != NULL)
        {
            if ((process_current->arrival_time > process_next->arrival_time) ||
                (process_current->arrival_time == process_next->arrival_time && process_current->priority > process_next->priority))
            {
                int pid, brust_time, arrival_time, priority, wait_time, remaining_brust_time, last_point;

                pid = process_current->pid;
                brust_time = process_current->brust_time;
                arrival_time = process_current->arrival_time;
                priority = process_current->priority;
                wait_time = process_current->wait_time;
                remaining_brust_time = process_current->remaining_brust_time;
                last_point = process_current->last_point;

                process_current->pid = process_next->pid;
                process_current->arrival_time = process_next->arrival_time;
                process_current->brust_time = process_next->brust_time;
                process_current->priority = process_next->priority;
                process_current->wait_time = process_next->wait_time;
                process_current->remaining_brust_time = process_next->remaining_brust_time;
                process_current->last_point = process_next->last_point;

                process_next->pid = pid;
                process_next->arrival_time = arrival_time;
                process_next->brust_time = brust_time;
                process_next->priority = priority;
                process_next->wait_time = wait_time;
                process_next->remaining_brust_time = remaining_brust_time;
                process_next->last_point = last_point;
            }
            process_next = process_next->next;
        }
        process_current = process_current->next;
    }

    return process_head;
}

struct Process *sort_priority_time(struct Process *process_head, int progress)
{
    struct Process *process_current;
    struct Process *process_next;

    process_current = process_head;

    while (process_current != NULL)
    {
        process_next = process_current->next;
        while (process_next != NULL)
        {
            if ((process_current->priority > process_next->priority && process_next->arrival_time <= progress) ||
                (process_current->arrival_time > progress) ||
                (process_current->priority == process_next->priority && process_current->arrival_time > process_next->arrival_time))
            {
                int pid, brust_time, arrival_time, priority, wait_time, remaining_brust_time, last_point;

                pid = process_current->pid;
                brust_time = process_current->brust_time;
                arrival_time = process_current->arrival_time;
                priority = process_current->priority;
                wait_time = process_current->wait_time;
                remaining_brust_time = process_current->remaining_brust_time;
                last_point = process_current->last_point;

                process_current->pid = process_next->pid;
                process_current->arrival_time = process_next->arrival_time;
                process_current->brust_time = process_next->brust_time;
                process_current->priority = process_next->priority;
                process_current->wait_time = process_next->wait_time;
                process_current->remaining_brust_time = process_next->remaining_brust_time;
                process_current->last_point = process_next->last_point;

                process_next->pid = pid;
                process_next->arrival_time = arrival_time;
                process_next->brust_time = brust_time;
                process_next->priority = priority;
                process_next->wait_time = wait_time;
                process_next->remaining_brust_time = remaining_brust_time;
                process_next->last_point = last_point;
            }
            process_next = process_next->next;
        }
        process_current = process_current->next;
    }

    return process_head;
}

struct Process *remove_process(struct Process *process_head, int pid)
{
    struct Process *process_current;
    struct Process *process_next;
    struct Process *process_prev = NULL;

    process_current = process_head;

    while (process_current != NULL)
    {
        process_next = process_current->next;

        if (process_current->pid == pid)
        {
            if (process_prev == NULL)
            {
                process_current = NULL;
                process_head = process_next;
            }
            else
            {
                process_current = NULL;
                process_prev->next = process_next;
            }

            break;
        }

        process_prev = process_current;
        process_current = process_current->next;
    }

    return process_head;
}

const char *get_scheduling_method_name(ScheduleMethod method)
{
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

ScheduleMethod Set_Scheduling_Method()
{
    char choice;
    while (1)
    {
        system("clear");
        printf("1) NONE\n2)First-Come First-Served(FCFS)\n3)Shortest-Job-First(SJF)\n4)Priority Scheduling(PS)\n5)Round-Robin(RRS)\nOption >");
        scanf(" %c", &choice);

        switch (choice)
        {
        case '1':
            return NONE;
            break;
        case '2':
            return FCFS;
            break;
        case '3':
            return SJF;
            break;
        case '4':
            return PS;
            break;
        case '5':
            return RRS;
            break;
        default:
            printf("wrong option");
            break;
        }
    }
}

int Set_Preemtive_Mode(ScheduleMethod currentMethod)
{
    if (currentMethod == FCFS)
    {
        system("clear");
        printf("FCFS only supports Non-Preemprive Mode(Off)\nPress any key to go back..");
        getchar();
        getchar();
        return 0;
    }

    if (currentMethod == RRS)
    {
        system("clear");
        printf("RR only supports Preemprive Mode(On)\nPress any key to go back..");
        getchar();
        getchar();
        return 1;
    }
    system("clear");
    while (1)
    {
        char choice;
        printf("1)On\n2)Off\nOption >");
        scanf(" %c", &choice);

        switch (choice)
        {
        case '1':
            return 1;
            break;
        case '2':
            return 0;
            break;
        default:
            printf("wrong option...");
            break;
        }
    }
}

// caculate the brust time of all processes
int calculate_brust_time(struct Process *process_head)
{
    struct Process *process = process_head;
    int total_brust_time = 0;
    while (process != NULL)
    {
        total_brust_time += process->brust_time;
        process = process->next;
    }

    return total_brust_time;
}

int calculate_current_brust_time(struct Process *process_head)
{
    struct Process *process = process_head;
    int total_brust_time = 0;
    while (process != NULL)
    {
        total_brust_time += process->brust_time;
        process = process->next;
    }

    return total_brust_time;
}

// caculate the avg wating time for all the processes(called after going through the scheduler)
float calculate_avg_wating_time(struct Process *process_head)
{
    struct Process *process = process_head;

    float total_wating_time = 0;
    float count = 0;

    while (process != NULL)
    {
        count++;
        total_wating_time += process->wait_time;
        process = process->next;
    }

    return total_wating_time / count;
}

// FCFS main scheduler function
void First_Come_First_Served(struct Run *run, struct Process *process_head)
{
    struct Process *current_process = process_head;
    struct Process *process = process_head;

    int time = 0;
    int progress = 0;
    int ideal_time = 0;
    int total_burst_time = calculate_brust_time(current_process); // get the total brust time for all processes

    /**
     * FCFS scheduler algorithm works as follow
     * it uses the progress variable to track the brust progress.
     * so the process wating time will always be: the current progress - the process arrival time
     */

    while ((progress - ideal_time) < total_burst_time && current_process != NULL)
    {
        if ((progress > 0) && (progress - current_process->arrival_time) <= 0)
        {
            current_process->wait_time = 0;
        }
        else
        {
            current_process->wait_time = progress - current_process->arrival_time;
        }

        while (current_process->remaining_brust_time > 0)
        {
            current_process->remaining_brust_time--;
            progress++;
        }

        current_process = current_process->next;

        if (current_process == NULL)
            break;

        while (current_process->arrival_time > progress)
        {
            ideal_time++;
            progress++;
        }
    }

    if ((progress - ideal_time) == total_burst_time) // if no errors
    {
        current_process = process_head;

        run->Avg_Wait_Time = calculate_avg_wating_time(process_head);
    }
}

// SJF main schedulers
void Short_Job_First(struct Run *run, struct Process *process_head)
{
    struct Process *current_process = process_head;
    int progress = 0;
    int idle_time = 0;
    int total_burst_time = calculate_brust_time(current_process);

    while ((progress - idle_time) < total_burst_time && current_process != NULL)
    {
        if (current_process->brust_time == current_process->remaining_brust_time)
        {
            current_process->wait_time = (progress - current_process->arrival_time <= 0) ? 0 : (progress - current_process->arrival_time);
        }

        current_process->remaining_brust_time--;

        progress++;

        if (current_process->remaining_brust_time == 0)
        {
            if (current_process->next != NULL)
            {
                current_process = sort_burst_time(current_process->next, progress);
            }

            while (current_process->arrival_time > progress)
            {
                idle_time++;
                progress++;
            }
        }
    }

    if ((progress - idle_time) == total_burst_time)
    {
        current_process = process_head;
        run->Avg_Wait_Time = calculate_avg_wating_time(current_process);
    }
}
void Short_Job_First_Preemptive(struct Run *run, struct Process *process_head)
{
    struct Process *current_process = process_head;
    int progress = 0;
    int idle_time = 0;
    int total_burst_time = calculate_brust_time(current_process);

    while ((progress - idle_time) < total_burst_time && current_process != NULL)
    {

        current_process->wait_time += (progress - current_process->last_point);

        current_process->remaining_brust_time--;

        progress++;

        current_process->last_point = progress;

        if (current_process->remaining_brust_time <= 0)
        {
            current_process = remove_process(current_process, current_process->pid);
        }

        if (current_process != NULL)
        {
            current_process = sort_burst_time_Preemptive(current_process, progress);

            while (current_process->arrival_time > progress)
            {
                idle_time++;
                progress++;
            }
        }
    }

    if ((progress - idle_time) == total_burst_time)
    {
        current_process = process_head;
        run->Avg_Wait_Time = calculate_avg_wating_time(current_process);
    }
}

// PS main schedulers
void Priority_Scheduler(struct Run *run, struct Process *process_head)
{
    struct Process *current_process = process_head;
    int progress = 0;
    int idle_time = 0;
    int total_burst_time = calculate_brust_time(current_process);

    while ((progress - idle_time) < total_burst_time && current_process != NULL)
    {
        if (current_process->brust_time == current_process->remaining_brust_time)
        {
            current_process->wait_time = (progress - current_process->arrival_time <= 0) ? 0 : (progress - current_process->arrival_time);
        }

        current_process->remaining_brust_time--;

        progress++;

        if (current_process->remaining_brust_time == 0)
        {
            if (current_process->next != NULL)
            {
                current_process = sortProcesses(current_process->next);

                while (current_process->arrival_time > progress)
                {
                    idle_time++;
                    progress++;
                }

                current_process = sort_priority_time(current_process, progress);
            }

            while (current_process->arrival_time > progress)
            {
                idle_time++;
                progress++;
            }
        }
    }

    if ((progress - idle_time) == total_burst_time)
    {
        current_process = process_head;
        run->Avg_Wait_Time = calculate_avg_wating_time(current_process);
    }
}
void Priority_Scheduler_Preemptive(struct Run *run, struct Process *process_head)
{
    struct Process *current_process = process_head;
    int progress = 0;
    int idle_time = 0;
    int total_burst_time = calculate_brust_time(current_process);

    while ((progress - idle_time) < total_burst_time && current_process != NULL)
    {
        current_process->wait_time += (progress - current_process->last_point);

        current_process->remaining_brust_time--;

        progress++;

        current_process->last_point = progress;

        if (current_process->remaining_brust_time <= 0)
        {
            current_process = remove_process(current_process, current_process->pid);
        }

        if (current_process != NULL)
        {
            current_process = sortProcesses(current_process);

            while (current_process->arrival_time > progress)
            {
                idle_time++;
                progress++;
            }

            current_process = sort_priority_time(current_process, progress);
        }
    }

    if ((progress - idle_time) == total_burst_time)
    {
        run->Avg_Wait_Time = calculate_avg_wating_time(process_head);
    }
}

// RR
void Round_Robin_Scheduler(struct Run *run, struct Process *process_head)
{
    struct Process *current_process = process_head;
    struct Process *queue = current_process;
    struct Process *temp = current_process;

    int progress = 0;
    int total_burst_time = calculate_brust_time(current_process);

    // convert to circular linkedlist. So we can loop through processes
    while (1)
    {
        if (queue->next == NULL)
        {
            queue->next = process_head;
            break;
        }

        queue = queue->next;
    }

    current_process = process_head;

    int i = 0;
    int ideal_time = 0;

    int quantum = 0;
    printf("Enter the quantum value: ");
    scanf(" %d", &quantum);

    while ((progress - ideal_time) < total_burst_time && current_process != NULL)
    {
        current_process->wait_time += (progress - current_process->last_point);

        for (i = 0; i < quantum; i++)
        {
            if (current_process->remaining_brust_time <= 0)
            {
                break;
            }

            progress++;

            current_process->last_point = progress;

            current_process->remaining_brust_time--;
        }

        current_process = current_process->next;

        if ((progress - ideal_time) >= total_burst_time)
            break;

        // if the current(next process) finished execuating (remaining_brust_time == 0), get the next one
        // only works if progress < total burst time.
        while ((progress - ideal_time) < total_burst_time && current_process->remaining_brust_time <= 0 || current_process->arrival_time > progress)
        {
            current_process = current_process->next;

            if (current_process == process_head)
            {

                if (has_next(process_head, progress) == 0)
                {
                    progress++;
                    ideal_time++;
                }

                current_process = process_head;
            }
        }
    }

    // convert back to singly linkedlist
    queue = process_head;
    while (1)
    {
        if (queue->next == process_head)
        {
            queue->next = NULL;
            break;
        }

        queue = queue->next;
    }

    if ((progress - ideal_time) == total_burst_time)
    {
        run->Avg_Wait_Time = calculate_avg_wating_time(process_head);
    }
}

// Option 3: show results. Return 0 if problem, and 1 if successfully finished
int Show_Results(struct Run *run, struct Process *process_head)
{
    system("clear");
    if (run->Scheduling_Method == NONE) // check if scheduling method is chosen
    {
        return 0;
    }

    struct Process *sortedProcesses = sortProcesses(process_head); // sorting the processes
    struct Process *temp_process;
    int count = 0; // counter to add the number of each process

    // print the headers
    printf("Scheduling Method: %s ", get_scheduling_method_name(run->Scheduling_Method));
    printf("- Preemptive Mode: %s\n", (run->Preemtive_Mode == 0 ? " Off " : " On "));

    switch (run->Scheduling_Method)
    {
    case FCFS:
        First_Come_First_Served(run, sortedProcesses);
        break;
    case SJF:
        sortedProcesses = sort_processes_short_brust(sortedProcesses);

        if (run->Preemtive_Mode == 0) // premptive mode: off
        {
            Short_Job_First(run, sortedProcesses);
        }
        else // preemptive mode: on
        {
            Short_Job_First_Preemptive(run, sortedProcesses);
        }

        break;
    case PS:

        sortedProcesses = sort_arrival_and_priority(sortedProcesses); // the inital sorting

        if (run->Preemtive_Mode == 0) // premptive mode: off
        {
            Priority_Scheduler(run, sortedProcesses);
        }
        else // preemptive mode: on
        {
            Priority_Scheduler_Preemptive(run, sortedProcesses);
        }

        break;
    case RRS:
        Round_Robin_Scheduler(run, sortedProcesses);
        break;
    default:
        printf("\nChoose a scheduling method!");
        return 0; // means problem
        break;
    }

    sortedProcesses = sortProcessesByPID(sortedProcesses);
    temp_process = sortedProcesses;
    // printing the output after implementing the scheduling
    while (temp_process != NULL)
    {
        printf("P%d: %d ms\n", temp_process->pid, temp_process->wait_time);
        temp_process = temp_process->next;
    }

    printf("Average Waiting Time: %.1f ms\n", run->Avg_Wait_Time); // prinf the average wait time

    return 1; // means successfully finished
}

// Option 4: Save results to Output File
int Save_Results_To_File(struct Run *run, struct File *OutputFile, struct Process *process_head)
{
    OutputFile->file = openFile(OutputFile->file_name, "w");

    if (OutputFile->file == NULL)
    {
        printf("\nProblem writing results to output file: %s\n", OutputFile->file_name);
        return 0;
    }

    printf("\nWriting Results to file: %s\n", OutputFile->file_name);

    fprintf(OutputFile->file, "Scheduling Method: %s ", get_scheduling_method_name(run->Scheduling_Method));
    fprintf(OutputFile->file, "- Preemptive Mode: %s\n", (run->Preemtive_Mode == 0 ? " Off " : " On "));

    while (process_head != NULL)
    {
        fprintf(OutputFile->file, "P%d: %d ms\n", process_head->pid, process_head->wait_time);
        process_head = process_head->next;
    }

    fprintf(OutputFile->file, "Average Waiting Time: %.1f ms\n", run->Avg_Wait_Time); // prinf the average wait time

    fclose(OutputFile->file);
}

int has_next(struct Process *process_head, int progress)
{
    struct Process *temp = process_head;

    do
    {
        if (temp->arrival_time <= progress && temp->remaining_brust_time != 0)
        {
            return 1;
        }

        temp = temp->next;
    } while (temp != process_head);

    return 0;
}