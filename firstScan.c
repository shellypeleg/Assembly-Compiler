/*
 * =========================================================================================
 * name        : Assembler/firstScan.c
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "instructions.h"
#include "dataTable.h"
#include "firstScan.h"

/*Max and min values for number - 8 bits*/
#define NUM_MAX 127
#define NUM_MIN -128

char *legalCommands[CMD_MAX_NUM] = {"mov","cmp","add","sub","not","clr","lea","inc","dec",
                                    "jmp","bne","red","prn","jsr","rts","stop"};

/* This method scans the assembly file for the first time and counts how many instruction memory words each row takes
 * Input: fp - a pointer to the start of the assembly file
 *        IC - the instructions counter
 * Output: 0 - compiling failed.
 *         1 - success compiling.
 */

int FirstScan(FILE *fp, int *IC)
{
    char tempRow[MAX_CHAR_TO_READ]; /*will hold the current row we read from the file*/
    char *currRow = NULL; /*a pointer to the start of the current row*/
    char label[MAX_LABEL_LENGTH]; /*label name*/
    int rowNumber = 0; /*rows counter*/
    char *token = NULL; /*will hold the temporary argument to parse*/
    char dst_op[MAX_ROW_LENGTH]; /*for destination operand in command statement*/
    char src_op[MAX_ROW_LENGTH]; /*for source operand in command statement*/
    commandType cmd; /*holds the current command*/
    int tempOffset; /*will hold a command offset*/
    int hasError = false;

    /*Do on each row in the assembly input file*/
    while(fgets(tempRow,MAX_CHAR_TO_READ, fp) != NULL)
    {
        rowNumber++;

        /*reset the following variables to null*/
        memset(label, '\0', sizeof(label));
        memset(src_op, '\0', sizeof(src_op));
        memset(dst_op, '\0', sizeof(dst_op));

        /*Check if the current row is valid*/
        if(strlen(tempRow) > MAX_ROW_LENGTH)
        {
            hasError = true;
            fprintf(stderr,"Error: Row length is not valid. Row number: %d\n",rowNumber);
        }

        currRow = tempRow;

        /*remove prefix whitespaces*/
        while(currRow[0] == spaceDelimeter[0] || currRow[0] == tabDelimeter[0])
        {
            currRow++;
        }

        /*remove the break from the end of the row if exists*/
        if(currRow[strlen(currRow)-1]== '\n')
            currRow[strlen(currRow)-1] = '\0';

        /*if the row is empty or is a comment - continue to the next row*/
        if(currRow[0] == ';' || IsRowEmpty(currRow))
            continue;

        /*split the row to tokens by spaces to get each argument separately*/
        /*allocate memory for the token*/
        token = (char *)malloc(MAX_ROW_LENGTH);

        /*memory allocation failed*/
        if(!token)
        {
            fprintf(stderr,"Error: Cannot allocate memory for parsing row number %d. Exiting program.\n",rowNumber);
            exit(1);
        }

        token = strtok(currRow, whitespaceDelim);

        /*Check if the first argument is a label. A label ends with ':' */
        if(token[strlen(token)-1] == ':')
        {
            /*remove the ':' from the end of the label and add null character to the end*/
            strncpy(label,token, strlen(token)-1);
            label[strlen(token)-1] = '\0';
            
            if(!IsValidLabel(label,rowNumber))
            {
                hasError = true;
            }

            if(IsSavedWord(label))
            {
                fprintf(stderr,"Error: Label name cannot be a saved word - %s. Row number: %d.\n",label,rowNumber);
                hasError = true;
            }

            /*get the next argument after the label*/
            token = strtok(NULL,whitespaceDelim);

            /*if the next argument is NULL or empty - the statement is not valid*/
            if(!token || token[0] == '\n' || IsRowEmpty(token))
            {
                hasError = true;
                fprintf(stderr,"Error: row cannot be a label without any command after it. Row number: %d.\n",rowNumber);
            }
        }

        /*Check if the first character of the argument is dot, which means it's a directive reference*/
        if(token != NULL) /*in case we got only a label*/
        {
            if(*token == '.')
            {
                if(!DirectiveReference(token, label, IC, rowNumber))
                {
                    hasError = true;
                }

                
            }
            
            /*command statements*/
            else 
            {
                /*if there's a label before the command - we need to save it*/
                if(!IsRowEmpty(label))
                {
                    if(!AddToSymbolTable(label,*IC, false, true))
                    {
                        hasError = true;
                    }

                }
                /*get the command type*/
                cmd = GetCommandByName(token);
                /*check if the given command is valid*/
                if(cmd.type == instructionTypeError)
                {
                    hasError = true;
                    fprintf(stderr,"Error: unknown command - %s. Row number: %d.\n",token,rowNumber);
                }

                /*get the next argument*/
                
                token = strtok(NULL,whitespaceDelim);
                /*if the command gets 2 operands*/
                if(cmd.operandsNum == 2)
                {
                    char *trimmedCommand = TrimCommand(token,rowNumber); /*will hold an instruction after the command name without any whitespaces*/
                    int count = 0; /*the number of commas in the instruction*/
                    char *checkCommas = trimmedCommand;

                    while(*checkCommas != '\0') /*count how many commas the instruction has*/
                    {
                        if(*(checkCommas++) == commaDelimeter[0])
                            ++count;
                    }

                    if(count > 1)
                    {
                        hasError = true;
                        fprintf(stderr, "Error: too many commas in row %d.\n", rowNumber);
                    }

                    /*if the instruction contains commas, get the source (pre comma) and destination (post comma) operands*/
                    if (trimmedCommand && (strchr(trimmedCommand,commaDelimeter[0])!= NULL))
                    {
                        if(strcmp(trimmedCommand,commaDelimeter)!=0) /*check if not only a comma without operands*/
                        {
                            /*get the source operand*/
                            char *operand = strtok(trimmedCommand,commaDelimeter);
                            strcpy(src_op,operand);

                            /*get the destination operand*/
                            operand = strtok(NULL,commaDelimeter);
                            strcpy(dst_op, operand);
                            tempOffset = CalculateInstructionOffset(src_op, dst_op, rowNumber, cmd.type);
                            if(tempOffset == 0)
                            {
                                hasError = true;
                                fprintf(stderr,"Error: Error found in calculating offset for row- %d.\n",rowNumber);
                            }
                            else
                                *IC += tempOffset;

                            /*check if there's another argument = error*/
                            if (strtok(NULL, commaDelimeter) != NULL)
                            {
                                hasError = true;
                                fprintf(stderr,"Error: too many operands in row %d.\n",rowNumber);
                            }
                        }

                        else
                        {
                            hasError = true;
                            fprintf(stderr,"Error: command can't get only a comma. row %d.\n",rowNumber);
                        }
                    }

                    else /*Not a single comma found - error*/
                    {
                        hasError = true;
                        fprintf(stderr,"Error: invalid number of arguments in row %d.\n",rowNumber);
                    }

                }

                /*if the command gets one operand*/
                else if (cmd.operandsNum == 1)
                {
                    if(token != NULL)
                    {
                        strcpy(dst_op, token);
                        tempOffset =  CalculateInstructionOffset(NULL, dst_op, rowNumber, cmd.type);
                        if(tempOffset == 0)
                        {
                            hasError = true;
                            fprintf(stderr,"Error: Error found in calculating offset for row- %d.\n",rowNumber);
                        }
                        else
                            *IC += tempOffset;
                    }

                    else
                    {
                        hasError = true;
                        fprintf(stderr,"Error: invalid number of arguments in row %d. Must get one operand.\n",rowNumber);
                    }
                }

                /*if the command gets 0 operands*/
                else if (cmd.operandsNum == 0)
                {
                    (*IC)++; /*this command takes only one instruction word in the memory*/
                }
            }
        }
    }

    return !hasError;
}


/*---------------------------------------------------------------*/

/*
    This function receives a label and checks if it is a saved word.
    The function returns 0 if the label is NOT a saved word.
    The function returns 1 if the label IS a saved word.
*/
int IsSavedWord(char *label)
{
    int i;
    for (i=0; i<CMD_MAX_NUM; i++)
    {
        if ((strcmp(label,(legalCommands[i])) == 0))
        {
            return true;
        }
    }

    if (strcmp(label,DOT_ENTRY) == 0)
    {
        return true;
    }
    
    if (strcmp(label,DOT_EXTERN) == 0)
    {
        return true;
    }

    if (strcmp(label,DOT_STRING) == 0)
    {
        return true;
    }

    if (strcmp(label,DOT_DATA) == 0)
    {
        return true;
    }

    if (strcmp(label,DOT_STRUCT) == 0)
    {
        return true;
    }

    return false;
}

/*---------------------------------------------------------------*/
/*This method checks if a given label is a valid label -
 * combines from only lettes and numbers and not reaches the length of 30*/

int IsValidLabel(char * label, int rowNumber)
{
    char symbol[MAX_LABEL_LENGTH];
    int hasError = 0;

    /*Check if the label length is valid*/
    if(strlen(label) > MAX_LABEL_LENGTH)
    {
        hasError = true;
        fprintf(stderr,"Error: label name is too long - %s. Row number: %d.\n",label, rowNumber);
    }

    /*Check if the label first character is not a number*/
    if(isdigit(*label))
    {
        hasError = true;
        fprintf(stderr,"Error: label cannot start with a digit - %s. Row number: %d.\n",label, rowNumber);
    }

    /*Check if the given label contains only letters and numbers*/
    if (sscanf(label,"%30[a-zA-Z0-9]",symbol) == 1)
    {
        if(strcmp(label,symbol)==0)
        {
            hasError = false;
        }
            
        else
        {
            hasError = true; 
            fprintf(stderr,"Error: Invalid label - %s, label should contain only letters and numbers. Row number - %d\n",label,rowNumber); 
        }      
    }
    
    else
    {
        hasError = true;  
        fprintf(stderr,"Error: Invalid label - %s, label should contain only letters and numbers. Row number - %d\n",label,rowNumber); 
    }
    
    return !hasError;
}
/*---------------------------------------------------------------*/

/*A method for parsing Directive references: .data, .string, .struct, .extern, .entry
 * Input:
 *      directiveName   - .string / .data / .struct / .extern / .entry
 *      label           - label name
 *      IC              - instruction counter
 *      rowNumber       - the number of the row from which we got the directive reference
 * Output: 
 *      0 - Compiling failed.
 *      1 - Success compiling. 
 * */

bool DirectiveReference(char *directiveName, char *label, int *IC, int rowNumber)
{
    /*auxiliary variables*/
    int hasError = false; /*Error flag*/
    char *token = NULL;
    char *data_num; /*will hold the numbers from .data directive*/

    /*auxilary variables for trimming commands*/
    char *start; 
    char *end;
    char trimmedDecleration[MAX_ROW_LENGTH];
    int numTester; /*to check if the given parameter in .struct is a number*/
   
    /*Check which directive was sent to the function*/
    /*.data*/
    if(!strcmp(directiveName, DOT_DATA))
    {
       if(label && !IsRowEmpty(label))
       {
           /*insert the label to the symbol table*/
           if(!AddToSymbolTable(label, GetDC(), false, false))
           {
               fprintf(stderr, "Error:Cannot insert label into the symbol table - %s. Row number: %d\n",label, rowNumber);
               hasError = true;
           }
       }

       /*Split each number comes after .data*/
       data_num = strtok(NULL, commaDelimeter);

       /*Add each number to the data table*/
       while(data_num)
       {
            if(SaveNumToDataTable(data_num))
                data_num = strtok(NULL, commaDelimeter);
            else
            {
                /*error in inserting the number into the data table*/
                hasError = true;
                fprintf(stderr, "Error: Invalid .data directive reference. Row number: %d\n",rowNumber);
                break;
            }
       }
    }

    /*.string*/
    else if (!strcmp(directiveName,DOT_STRING))
    {

        if(label && !IsRowEmpty(label))
        {
           /*insert the label to the symbol table*/
           if(!AddToSymbolTable(label, GetDC(), false, false))
           {
               fprintf(stderr, "Error:Cannot insert label into the symbol table - %s. Row number: %d\n",label, rowNumber);
               hasError = true;
           }
           
           /*recieve the next token after .string - should be a string*/
           token = strtok(NULL,whitespaceDelim);

           /*save the string to the data table*/
           if(!SaveStringToDataTable(token))
           {
               hasError = true;
               fprintf(stderr,"Error: Invalid .string directive reference. Row number: %d\n",rowNumber);
           }
        }
    }

    /*.extern*/
    else if(!strcmp(directiveName,DOT_EXTERN))
    {
        /*if there's a label before .extern - we should ignore it*/
        if(!label)
        {
            fprintf(stderr,"Warning: Ignored label before .extern statement - %s. Row number: %d\n",label,rowNumber);
        }

        /*get the label after the .extern statement*/
        token = strtok(NULL, whitespaceDelim);

        /*insert the label to the symbol table*/
        if(!AddToSymbolTable(token, 0, true, false))
        {
            fprintf(stderr, "Error:Cannot insert label into the symbol table - %s. Row number: %d\n",label, rowNumber);
            hasError = true;
        }
        
        /*get the next token - if it's not NULL, the .extern statement is invalid*/
        token = strtok(NULL,whitespaceDelim);
        if (token)
        {
            hasError = true;
            fprintf(stderr,"Error: Invalid .extern directive reference. Row number: %d\n",rowNumber);
        }
    }

    /*.entry*/
    else if(!strcmp(directiveName,DOT_ENTRY))
    {
        /*if there's a label before .extern - we should ignore it*/
        if(!label)
        {
            fprintf(stderr,"Warning: Ignored label before .entry statement - %s. Row number: %d\n",label,rowNumber);
        }

    }

    /*.struct*/
    else if(!strcmp(directiveName, DOT_STRUCT))
    {
        /*Check for label - there's must be a label before struct decleration */
        if(!label || IsRowEmpty(label))
        {
            hasError = true;
            fprintf(stderr,"Error: There's must be a label before struct decleration. Row number: %d.\n",rowNumber);
        }

        else
        {
            hasError = !AddToSymbolTable(label, GetDC(), false, false);

            /*get the next argument in the row*/
            token = strtok(NULL,whitespaceDelim);

            /*trim the command after the struct decleration*/
            while (token)
            {
                /*skip whitespaces from the left*/
                while(isspace(*token))
                    token++;

                start = token;
                end = start+strlen(token);

                /*trim whitespaces from the right*/
                while(--end >= start)
                {
                    if(!isspace(*end))
                        break;
                }
                *(++end) = '\0';

                strcat(trimmedDecleration, token);
                token = strtok(NULL,whitespaceDelim);
            }
            
            /*check if the trimmed decleration has comma (seperating the number and the string)*/
            if(!IsRowEmpty(trimmedDecleration) && (strchr(trimmedDecleration, commaDelimeter[0])!= NULL))
            {

                /*get the first element - should be a number*/
                token = strtok(trimmedDecleration, commaDelimeter);
                numTester = (*token) - '0';
                /*if the first argument is a number, save it to the data table*/
                if(((numTester <=NUM_MAX && numTester >= NUM_MIN) ) && SaveNumToDataTable(token))
                {
                    /*get the next parameter -should be a string */
                    token = strtok(NULL,commaDelimeter);
                    if(!SaveStringToDataTable(token))
                    {
                        hasError = true;
                        fprintf(stderr,"Error: Invalid second parameter in struct decleration. Row number: %d.\n",rowNumber);
                    }
                }

                else /*first argument is not a valid number or adding to data table failed*/
                {
                    hasError = true;
                    fprintf(stderr,"Error: Invalid first parameter in struct decleration. Row number: %d.\n",rowNumber);

                }
            }
            else
            {
                hasError = true;
                fprintf(stderr,"Error: Struct decleration invalid. Row number: %d.\n", rowNumber);
            }
        }
    }
    else
    {
        fprintf(stderr,"Error: Invalid directive reference. Row number: %d\n",rowNumber);
        hasError = true;
    }
    return !hasError;
}

/*---------------------------------------------------------------*/