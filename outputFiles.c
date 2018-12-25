/*
 * =========================================================================================
 * name        : Assembler/outputFiles.c
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "common.h"
#include "outputFiles.h"

/*Output files extensions*/
#define obExt ".ob"
#define entExt ".ent"
#define extrnExt ".ext"

#define MAX_FILES 500 /*max files the progran can create*/
#define MAX_STRANGE_BASE_SIZE 32
#define MAX_DIGIT 2 /*max num of digits in the strange base (10 bits, each digit is represented by 5 bits)*/

#define BASE_CODE_ADDRESS 100

#define MASK 1023 /*10 relevant bits for our instruction memory word*/

char strange_base_char[MAX_STRANGE_BASE_SIZE] = {'!','@','#','$','%','^','&','*','<','>','a','b','c','d','e','f',
'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v'};

int addressCounter = 0; /*for IC addresses*/
int dataCounter = 0; /*for DC addresses*/

/*Output files pointers*/
FILE *obFp;
FILE *entFp;
FILE *extFp;

char *createdFiles[MAX_FILES]; /*Will hold pointers to output files names*/
char outputFileName[MAX_FILENAME_LENGTH]; /*will hold the name of the output file (without the extension)*/
int currentFileIndex = 0; /*will hold the current index in the created files array*/

/*---------------------------------------------------------------*/

/*New string memory allocation - auxiliary function */
char *new_string(int size)
{
    char *str = (char*)malloc(sizeof(char)*size);
    if(!str)
    {
        fprintf(stderr, "Error: Failed to allocate memory for a new string. Exiting program.\n");
        exit(1);
    }

    return str;
}

/*---------------------------------------------------------------*/
/*This method gets the current file name, and stores it in an array */
void SetFilesName(char *fileName)
{
    if(fileName)
        strcpy(outputFileName, fileName);
    else
        fprintf(stderr,"Error: file name cannot be empty.\n");
}
/*---------------------------------------------------------------*/

/*This method opens the object file (.ob) which holds the main program encoding
 * Output - 1 if the opening was successfull
 *        - 0 opening failed
 */

int OpenObjectFile()
{
    int hasError = false; /*error flag*/

    /*add the .ob extension to the file name*/
    char *fullName = new_string(strlen(outputFileName)+strlen(obExt));
    strcpy(fullName, outputFileName);
    strcat(fullName,obExt);

    /*open file for writing*/
    obFp = fopen(fullName,"w+");

    if(!obFp)
        hasError = true;

    return !hasError;
}

/*---------------------------------------------------------------*/

/*This method opens the entry file (.ent) which holds entry label encoding
 * Output - 1 if the opening was successfull
 *        - 0 opening failed
 */

int OpenEntryFile()
{
    int hasError = false; /*error flag*/

    /*add the .ent extension to the file name*/
    char *fullName = new_string(strlen(outputFileName)+strlen(entExt));
    strcpy(fullName, outputFileName);
    strcat(fullName,entExt);

    /*open file for writing*/
    entFp = fopen(fullName,"w+");

    if(!entFp)
        hasError = true;

    return !hasError;
}

/*---------------------------------------------------------------*/

/*This method opens the external file (.ext) which holds external label apperance encoding
 * Output - 1 if the opening was successfull
 *        - 0 opening failed
 */

int OpenExternalFile()
{
    int hasError = false; /*error flag*/

    /*add the .ext extension to the file name*/
    char *fullName = new_string(strlen(outputFileName)+strlen(extrnExt));
    strcpy(fullName, outputFileName);
    strcat(fullName,extrnExt);

    /*open file for writing*/
    extFp = fopen(fullName,"w+");

    if(!extFp)
        hasError = true;

    return !hasError;
}

/*---------------------------------------------------------------*/
/*This method writes the rows into the object file
 *Input - address - the instruction address
 *      - encoding - the instruction encoding
 */
void ObjectWriteLine(int address, int encoding)
{
    char *instAdd; /*will hold the address after converstion to the strange base*/
    char *instCode; /*will hold the encoding after converstion to the strange base*/

    if(!obFp && !OpenObjectFile())
    {
        fprintf(stderr,"Error: Cannot create the object output file.\n");
    }

    else
    {
        /*if the file is empty, the first line should be IC and DC values, in the strange base*/
        if(ftell(obFp) == 0)
        {
            fprintf(obFp,"Base 32 Address\t\t\tBase 32 Code\n");
            instAdd = BinaryToStrange32BaseAddress(address-BASE_CODE_ADDRESS);
            instCode = BinaryToStrange32BaseCode(encoding);
            fprintf(obFp,"\t\t%s\t%s\n", instAdd, instCode);
        }

        /*else - write all instructions addresses and encodings*/
        else
        {
            instAdd = BinaryToStrange32BaseAddress(address);
            instCode = BinaryToStrange32BaseCode(encoding);
            fprintf(obFp,"%s\t\t\t\t%s\n",instAdd,instCode);
        }
    }
}

/*---------------------------------------------------------------*/

/*This method writes the rows into the entry file
 *Input - label - the label name
 *      - address - the label address
 */

void EntryWriteLine(char *label, int address)
{
    if(!entFp && !OpenEntryFile())
    {
        fprintf(stderr,"Error: Cannot create the entry output file.\n");
    }

    else
    {
        char *symAdd = BinaryToStrange32BaseAddress(address);
        fprintf(entFp,"%s\t%s\n",label,symAdd);
    }
}


/*---------------------------------------------------------------*/

/*This method writes the rows into the external output file
 *Input - label - the external label name
 *      - address - the label apperance address
 */

void ExternalWriteLine(char *label, int address)
{
    if(!extFp && !OpenExternalFile())
    {
        fprintf(stderr,"Error: Cannot create the external output file.\n");
    }

    else
    {
        char *symAdd = BinaryToStrange32BaseAddress(address);
        fprintf(extFp,"%s\t%s\n",label,symAdd);
    }
}

/*---------------------------------------------------------------*/
/*This method closes the object file (.ob) which holds the main program encoding
 * Output - 1 if the closing was successfull
 *        - 0 opening failed
 */

int CloseObjectFile()
{
    int hasError = false; 

    /*check if the file needs to be closed*/
    if(!obFp || (fclose(obFp)) == 0)
    {
        char *fullName;

        /*set the pointer to the file back to NULL*/
        obFp = NULL;

        /*add the file to the created files list*/
        fullName = new_string(strlen(outputFileName) + strlen(obExt));
        strcpy(fullName,outputFileName);
        strcat(fullName, obExt);

        createdFiles[currentFileIndex] = malloc(strlen(fullName)+ 1);
        strcpy(createdFiles[currentFileIndex],fullName);
        currentFileIndex++;
    }

    else
        hasError = true;

    return !hasError;
}
/*---------------------------------------------------------------*/

/*This method closes the entry file (.ent) which holds entry label encoding
 * Output - 1 if the closing was successfull
 *        - 0 closing failed
 */

int CloseEntryFile()
{
    int hasError = false; /*error flag*/

    /*check if the file needs to be closed*/
    if(!entFp || (fclose(entFp)) == 0)
    {
        char *fullName;

        /*set the pointer to the file back to NULL*/
        entFp = NULL;

        /*add the file to the created files list*/
        fullName = new_string(strlen(outputFileName) + strlen(entExt));
        strcpy(fullName,outputFileName);
        strcat(fullName, entExt);

        createdFiles[currentFileIndex] = malloc(strlen(fullName)+ 1);
        strcpy(createdFiles[currentFileIndex],fullName);
        currentFileIndex++;
    }

    else
        hasError = true;

    return !hasError;
}
/*---------------------------------------------------------------*/

/*This method opens the external file (.ext) which holds external label apperance encoding
 * Output - 1 if the opening was successfull
 *        - 0 opening failed
 */

int CloseExternalFile()
{
    int hasError = false; /*error flag*/

    /*check if the file needs to be closed*/
    if(!extFp || (fclose(extFp)) == 0)
    {
        char *fullName;

        /*set the pointer to the file back to NULL*/
        extFp = NULL;

        /*add the file to the created files list*/
        fullName = new_string(strlen(outputFileName) + strlen(extrnExt));
        strcpy(fullName,outputFileName);
        strcat(fullName, extrnExt);

        createdFiles[currentFileIndex] = malloc(strlen(fullName)+ 1);
        strcpy(createdFiles[currentFileIndex],fullName);
        currentFileIndex++;
    }

    else
        hasError = true;

    return !hasError;
}
/*---------------------------------------------------------------*/

/*This method closes all open files (object, entry, external)
 If closing was successfull return true, otherwisw - false*/
void CloseOpenFiles()
{

    if((CloseObjectFile()) + (CloseEntryFile()) + (CloseExternalFile()) != 3)
    {
        fprintf(stderr,"Error: cannot close output files.\n");
    }

}

/*---------------------------------------------------------------*/
/*this method deleted all the created files - in case error found while compiling the input files*/
void DeleteOutputFiles()
{
    int i; /*files array index*/

    /*error found in the first file*/
    if(currentFileIndex == 3)
    {
        for(i=0; i< currentFileIndex; i++)
        {
            remove(createdFiles[i]); /*remove function from standard library - removes the file*/
        }

    }
    else /*error found after the first file*/
    {
        for(i=currentFileIndex - 3; i< currentFileIndex; i++)
        {
            remove(createdFiles[i]); /*remove function from standard library - removes the file*/
        }
    }

}
/*---------------------------------------------------------------*/
/*---------------------------------------------------------------*/

/*This method converts a binary number (represented as an integer) to a strange 32 base code (presented in symbols and letters)
 *Input  - an address in binary base (represented as a number)
 *Output - if converting was succssfull, this method returns an array of characters in 32 strange base*/

char *BinaryToStrange32BaseAddress(int address)
{   
    int base = MAX_STRANGE_BASE_SIZE; /*The number of characters in the strange base*/
    int digit_count = MAX_DIGIT; /*Will count how many digits the new address should take*/
    int remainder; /*Will hold the remainder*/

    /*indexing auxiliary integers*/
    int i=0;
    int j=0;

    int *address_arr; /*Pointer to an array of integers*/
    char *strange_base_address; /*Pointer to an array of integers*/
    

    /*Allocate memory for addresses array*/
    address_arr = malloc(digit_count * sizeof(int));
    /*In case of memory allocation failure*/
    if(address_arr == NULL)
    {
        fprintf(stderr,"Error: Failed to allocate memory to an array which should store the addresses before converting to 32 base\n");
        return 0;
    }

    /*Convert the number to the strange 32 base*/
    while (i<digit_count)
    {
        remainder = address % base;
        address = address / base;
        address_arr[i] = remainder;
        i++;
    }

    /*Allocate memory for converted addresses to 32 base*/
    strange_base_address = malloc(digit_count * sizeof(char));
    /*In case of memory allocation failure*/
    if (strange_base_address == NULL)
    {
        fprintf(stderr,"Error: Failed to allocate memory to an array which should store the converted addresses to 32 base\n");
        return 0;
    }

    i = digit_count-1;

    /*convert each number in the address_arr to the equivilant character in strange_base_char array*/
    for (j=0; j<digit_count; j++,i--)
    {
        strange_base_address[j] = strange_base_char[address_arr[i]];
    }


    return strange_base_address;
}


/*---------------------------------------------------------------*/

/*This method converts a binary number (represented as an integer) to a strange 32 base code (presented in symbols and letters)
 *Input  - a code in binary base (represented as a number)
 *Output - if converting was succssfull, this method returns an array of characters in 32 strange base*/

char *BinaryToStrange32BaseCode(int encoding)
{

    int base = MAX_STRANGE_BASE_SIZE; /*The number of characters in the strange base*/
    int digit_count = MAX_DIGIT; /*Will count how many digits the new address should take*/
    int remainder; /*Will hold the remainder*/

    /*indexing auxiliary integers*/
    int i=0;
    int j=0;

    int *encoding_arr; /*Pointer to an array of integers*/
    char *strange_base_encoding; /*Pointer to an array of integers*/
    
    /*If the encoding is a negative number, convert the binary code with 2's complement*/
    if(encoding<0)
    {
        encoding = encoding & MASK;
    }

    /*Allocate memory for addresses array*/
    encoding_arr = malloc(digit_count * sizeof(int));
    /*In case of memory allocation failure*/
    if(encoding_arr == NULL)
    {
        fprintf(stderr,"Error: Failed to allocate memory to an array which should store the encodings before converting to 32 base\n");
        return 0;
    }

    /*Convert the number to the strange 32 base*/
    while (i<digit_count)
    {
        remainder = encoding % base;
        encoding = encoding / base;
        encoding_arr[i] = remainder;
        i++;
    }

    /*Allocate memory for converted addresses to 32 base*/
    strange_base_encoding = malloc(digit_count * sizeof(char));
    /*In case of memory allocation failure*/
    if (strange_base_encoding == NULL)
    {
        fprintf(stderr,"Error: Failed to allocate memory to an array which should store the converted encodings to 32 base\n");
        return 0;
    }

    i = digit_count-1;

    /*convert each number in the address_arr to the equivilant character in strange_base_char array*/
    for (j=0; j<digit_count; j++,i--)
    {
        strange_base_encoding[j] = strange_base_char[encoding_arr[i]];
    }

    return strange_base_encoding;

}



/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/