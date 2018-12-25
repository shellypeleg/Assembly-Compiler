/*
 * =========================================================================================
 * name        : Assembler/secondScan.c
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "instructions.h"
#include "outputFiles.h"


/* This method scans the assembly file for the second time and converts each instruction to its memory words
 * Input: fp - a pointer to the start of the assembly file
 *        IC - the instructions counter
 * Output: 0 - compiling failed.
 *         1 - success compiling.
 */

int SecondScan(FILE *fp, int IC)
{
    char tempRow[MAX_ROW_LENGTH]; /*will hold the current row we read from the file*/
    char *currRow; /*A pointer to the start of the current row*/
    int rowNumber = 0; /*rows counter*/
    instructionFields cmdInfo; /*will hold information about a single command*/
    int inst_offset = 0; /*will hold how many extra memory words the command takes*/
    int hasError = false; /*Error flag*/
    symbol *sym; /*A pointer to symbol struct*/
    int commandCode; /*the binary code*/
    int address; /*address by IC*/
    int isDestination; /*isDestination flag - if an operand is a dst operand*/
    char *token, dst_op[MAX_ROW_LENGTH], src_op[MAX_ROW_LENGTH]; /*will hold the source & destination operands*/
    commandType cmd; /*holds the current command*/


    /*Do on each row in the assembly input file*/
    while(fgets(tempRow, MAX_ROW_LENGTH, fp)!= NULL)
    {
        rowNumber++;
        ResetCommandInfo(&cmdInfo); /*Reset the command info struct*/

        /*reset the following variables to null*/
        memset(src_op, '\0', sizeof(src_op));
        memset(dst_op, '\0', sizeof(dst_op));

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
            
            /*Go to the next argument - ignoring labels in the second scan*/
            token = strtok(NULL,whitespaceDelim);
        }

        /*Check if the argument is a directive reference .data/.string/.struct/.entry/.extern */
        if(*token == '.')
        {
            /*if the element is .entry we need to save the label to the entry output file*/
            if(!strcmp(token,DOT_ENTRY))
            {
                /*find the label from the symbol table*/
                token = strtok(NULL,whitespaceDelim);
                /*Get the next argument with no whitespaces - if it's a valid label, should be only the label name*/
                token = TrimCommand(token,rowNumber);
                sym = FindSymbol(token); /*if found, sym will hold all the information about the label*/
                /*if label doesn't exist - error*/
                if(!sym)
                {
                    hasError = true;
                    fprintf(stderr,"Error: label \"%s\" was not declared. Row number: %d.\n",token,rowNumber);
                    continue;
                }

                else
                {
                    /*write the label in entry output file only if it's not extern label*/
                    if(sym->symLinkType != LINK_TYPE_E)
                    {
                        EntryWriteLine(sym->label,sym->symAddress);
                    }

                    else
                    {
                        hasError = true;
                        fprintf(stderr,"Error: label \"%s\" is external and cannot be entry. Row number: %d.\n",token, rowNumber);
                    }
                }
            }
        }

        /*the argument is a command name*/
        else
        {
            cmd = GetCommandByName(token);
            /*invalid command name*/
            if(cmd.type == instructionTypeError)
            {
                hasError = true;
                fprintf(stderr,"Error: Invalid command- %s. Row number: %d.\n",token, rowNumber);
                continue;
            }
            /*update information (command name) in the instruction info struct*/
            cmdInfo.commandName = cmd.type;
            /*get the next arguments = operands*/
            token = strtok(NULL,whitespaceDelim);
            

            /*if the command gets 2 operands*/
            if(cmd.operandsNum == 2)
            {
                
                /*get the next of the row without any whitespaces*/
                char *trimmedCommand = TrimCommand(token,rowNumber);
                
                /*check if the trimmed command contatins a comma*/
                if(trimmedCommand && (strchr(trimmedCommand,commaDelimeter[0]) != NULL))
                {
                    
                    /*if the two operands are registers, save the info in the instrution info struct*/
                    if(sscanf(trimmedCommand, "%2[r0-7]%*[,]%2[r0-7]",src_op,dst_op) == 2)
                    {
                        
                        cmdInfo.srcType = Register_op; /*opreand type*/
                        cmdInfo.srcReg = atoi(&(src_op[1])); /*save the register num*/
                        cmdInfo.dstType = Register_op; /*operand type*/
                        cmdInfo.dstReg = atoi(&(dst_op[1]));

                        /*create a mutual memory word for both registers without ARE bits*/
                        commandCode = cmdInfo.srcReg << 4; /*move left 4 bits*/
                        commandCode += cmdInfo.dstReg;
                        address = IC;

                        

                        /*Add instruction to the instruction words array*/
                        if(!AddInstruction(LINK_TYPE_A, &address, commandCode, trimmedCommand))
                        {
                            hasError = true;
                            fprintf(stderr,"Error: Can't add memory word for %s command. Row number: %d\n",cmd.cmd_str, rowNumber);
                        }

                        /*this command takes 2 memory words - one for the command itself and one for registers numbers*/
                        inst_offset = 2;

                    }

                    /*If the 2 operands are not registers, each opreand should be parsed seperately*/
                    else 
                    {
                        /*split operands*/
                        char param[MAX_ROW_LENGTH];

                        strcpy(param,strtok(trimmedCommand, commaDelimeter));
                        strcpy(src_op,param); /*src_op will get the firs operand*/
                        strcpy(param,strtok(NULL,commaDelimeter));
                        strcpy(dst_op,param); /*dst_op will get the second operand*/

                        address = IC; /*set address back to the current IC address*/
                        
                        /*parse the source operand*/
                        isDestination = false;
                        if(!ParseOperand(src_op, &(cmdInfo.srcType), &(cmdInfo.srcReg), &address, rowNumber, isDestination))
                        {
                            hasError = true;
                            fprintf(stderr,"Error: error found in source operand (%s) parsing. Row number: %d.\n",src_op, rowNumber);
                        }
                        
                        /*parse the destination operand*/
                        isDestination = true;
                        if(!ParseOperand(dst_op, &(cmdInfo.dstType), &(cmdInfo.dstReg), &address, rowNumber, isDestination))
                        {
                            hasError = true;
                            fprintf(stderr,"Error: error found in destination operand (%s) parsing. Row number: %d.\n",dst_op, rowNumber);
                        }
                        
                        /*Calculate how many memory words this command takes*/
                        inst_offset = CalculateInstructionOffset(src_op, dst_op, rowNumber, cmd.type);
                        
                    }
                }

                else /*no commad found or nothing in the trimmed command*/
                {
                    hasError = true;
                    fprintf(stderr,"Error: Invalid arguments in row %d.\n",rowNumber);
                }
            }

            /*if the command gets 1 operand*/
            else if (cmd.operandsNum == 1)
            {
                address = IC; 
                strcpy(dst_op,token); /*dst_op gets the only argument after the command name*/

                /*parse the destination operand*/
                isDestination = true;
                if(!ParseOperand(dst_op, &(cmdInfo.dstType), &(cmdInfo.dstReg), &address, rowNumber, isDestination))
                {
                    hasError = true;
                    fprintf(stderr,"Error: error found in destination operand (%s) parsing. Row number: %d.\n",dst_op, rowNumber);
                }

                else /*Calculate how many memory words this command takes*/
                    inst_offset = CalculateInstructionOffset(src_op, dst_op, rowNumber, cmd.type);
            }

            /*command with no arguments*/
            else if(cmd.operandsNum == 0 && (cmd.type == stop || cmd.type == rts))
            {   
                /*those commands takes only one memory words*/
                inst_offset = 1;
            }


            if(inst_offset == 0)
            {
                hasError = true;
                fprintf(stderr,"Error: Cannot calculate how many memory words the instruction takes. Row Number: %d.\n",rowNumber);
            }

            /*Save the basic memory word for the instruction*/
            StoreMemWord(cmdInfo, IC);

            IC += inst_offset; /*enlarge IC by the number of memory words*/
        }
    }

    return !hasError;
}
