#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *openFile(const char fileName[])
{
    FILE *file = fopen(fileName, "r");

    if (file == NULL)
    {
        return NULL;
    }

    return file;
}

int main(int argc, char const *argv[])
{
    int i; // for iterations;
    char *tempChars;
    tempChars = (char *)malloc(10 * sizeof(char));

    FILE *InputFile; //-f input file
    FILE *OuputFile; //-o output file
    FILE ProjectFile;
    int choice;        // user choice
    float avgWaitTime; // average wating time for all jobs

    enum ScheduleMethod // selecting the scheduling method
    {
        NONE, // No Scheduling method
        FCFS, // First Come, First Served
        SJF,  // Shortest-Job-First
        PS,   // priority scheduling
        RRS   // Round-Robin scheduling
    };

    int PreemptiveMode = 0; // 1: PreemptiveMode, 0: Non-preemptiveMode

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

    if (argc <= 1) // check if any command arguments passed
    {
        printf("No input & output files");
        return -1;
    }

    for (i = 0; i < argc; i++) // loop through the passed args -f and -o
    {

        /**/
        if (strcmp(argv[i], "-f") == 0) // handel the -f file
        {
            InputFile = openFile(argv[i + 1]); // open input file

            if (InputFile == NULL) // check if not opened
            {
                printf("%s: Can't open the input file", argv[i + 1]);
                return -1;
            }

            char temp_ch; // store the character

            struct InputChar *head_input = (struct InputChar *)malloc(sizeof(struct InputChar));    // store the first input
            struct InputChar *current_input = (struct InputChar *)malloc(sizeof(struct InputChar)); // store the current input
            struct InputChar *temp_input;                                                           // store the temp input

            head_input = NULL; // the head is NULL
            int col = 0;       // the col index

            // struct Process *process_head = (struct Process *)malloc(sizeof(struct Process)); // store head of processes
            struct Process *process_current; // stores current process

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
                if (temp_ch == 10) // new line == new process
                {
                    // process_current = (struct Process *)malloc(sizeof(struct Process));
                }

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
                    temp_input = head_input;
                    if (col == 0)
                    {
                        printf("Brust time: %c\n", temp_input->value);
                    }
                    else if (col == 1)
                    {
                        printf("Arrival time: %c\n", temp_input->value);
                    }
                    else if (col == 2)
                    {
                        printf("Protiry : %c\n", temp_input->value);
                    }
                    col++;
                    head_input = NULL;

                    if (col > 2)
                    {
                        col = 0;
                        printf("\n");
                    }
                }
            }
            // free the allocated memory
            free(head_input);

            free(current_input);

            // free(temp_input);

            // free(process_head);

            // free(process_current);
        }

        if (strcmp(argv[i], "-o") == 0) // handle the -o file
        {
            printf("output: %s", argv[i + 1]);
        }
    }

    free(tempChars);
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
