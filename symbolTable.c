/*
 * =========================================================================================
 * name        : Assembler/symbolTable.c
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#include "symbolTable.h"

/* A node in a linked list to include all data about a single symbol */
typedef struct symbolTable * symbol_ptr;
typedef struct symbolTable {
    char symbolName[MAX_LABEL_LENGTH];
    int address;
    int isExternal;
    int closeToActionLine;
    symbol_ptr next;
} symbolTableLine;

symbol_ptr symbolHead;
symbol_ptr symbolCurrent;


/*---------------------------------------------------------------*/
/*Initialize the Symbole Table, by creating a first node which will represent the first line of the table, and set pointers to it.
 *The first line won't hold a real symbole until the addSymbol function will be used for the first time*/

void InitializeSymbolTable()
{
    symbolHead = (symbol_ptr)malloc(sizeof(symbolTableLine));
    symbolCurrent = (symbol_ptr)malloc(sizeof(symbolTableLine));

    /*In case of memory allocation faliure, print a failure message exit the program*/

    if (symbolHead == NULL || symbolCurrent == NULL )
    {
        printf("Faild to allocate memory for creating the Symbole Table\n");
        exit(0);
    }

    /*set values for the first line - 
    address is set to non existing address in order to distinguish between a non used table line to a used table */
    symbolCurrent->address = -1;
    symbolCurrent->isExternal = 1;
    symbolCurrent->closeToActionLine = 1;

    symbolHead = symbolCurrent;

}

/*---------------------------------------------------------------*/

/*
* A method used to insert a new line to the symbol table
* Input:
    1. Symbol/label name.
    2. A counter to represent the address (IC/DC).
    3. An int to represent whether this is an external value.
    4. An int to represent whether this is next to an Action sentence.
 * Output:
   - If succeded - 1.
   - If failed - 0 + an error message. 
*/
int AddToSymbolTable (char * symbolName, int counter, int isExternal, int closeToActionLine)
{
    symbol_ptr tempHead = symbolHead;
    symbol_ptr currLine = symbolCurrent;

    int result = 1; /*result flag. 1 = no problems found, 0 = problems found*/
    char * newTempLabel;
    
    /*Check if the given label is null*/
    if (!symbolName)
        {
            printf("Cannot receive NULL as a symbol name \n");
            result = 0;
        }

    else
    {
        symbol_ptr tempLine;

        /*If a given label ends with /n, remove it*/
        if ((newTempLabel = strchr(symbolName,'\n')) != NULL) 
             *newTempLabel = '\0';
           
        /*If the table has only one line we need to check if it has any symbols*/
        if (tempHead == currLine)
        {
            
            /*If no label was insert to the table yet*/
            if (currLine->address == -1)
            {
                strcpy(currLine->symbolName, symbolName);
                currLine->address = counter;
                currLine->isExternal = isExternal;
                currLine->closeToActionLine = closeToActionLine;
                currLine->next = NULL;

                symbolHead = currLine;
                symbolCurrent = currLine;

                return result;
            } 

        }
        
        /*Checking if the given label already exists in the table*/
        /*loops until the lines in the table are over*/
        while(tempHead)
        {
            /*check if the same label was found*/
            if(tempHead->symbolName == symbolName)
            {
                printf("Label already exists in the table.\n");
                result = 0;
                break;
            }
            tempHead = tempHead->next;
        }

        
        /*If no error was found, insert the new symbol to the table*/
        if (result)
        {   
            tempLine = (symbol_ptr)malloc(sizeof(symbolTableLine));

            if (!tempLine)
            {
                printf("Could not allocate memory for a new line in the symbole table.\n");
                exit(0);
            }
            strcpy(tempLine->symbolName, symbolName);
            tempLine->address = counter;
            tempLine->isExternal = isExternal;
            tempLine->closeToActionLine = closeToActionLine;
            tempLine->next = NULL;

            symbolCurrent->next = tempLine;
            symbolCurrent = tempLine;

        }       
    } 

    return result;
} 

/*---------------------------------------------------------------*/
/*This function recieve the last and most updated IC (as offset), and update all symbols close to directive references*/
void UpdateSymbolTable (int offset)
{
    symbol_ptr tempHead = symbolHead;
    symbol_ptr currLine = symbolCurrent;
    
    if(tempHead == currLine) /*No data in the symbol table*/
    {
        if (currLine->address == -1)
            return;
    }

    while (tempHead)
    {
        if (!tempHead->closeToActionLine && !tempHead->isExternal) 
        {
            tempHead->address += offset;
        }

        tempHead = tempHead->next;
    }
}

/*---------------------------------------------------------------*/

/*
    This function looks for a specific label on the Symbol table.
    If the label can be found, the function creates a symbol type struct with all given details, and returns it.
    IF the label cannot be found, the function returns NULL.
*/
symbol* FindSymbol(char *label)
{
    symbol_ptr tempHead = symbolHead;
    symbol_ptr currLine = symbolCurrent;
    symbol *foundElement = NULL;
    
    /*Check if the table is empty*/
    if(tempHead == currLine) 
    {
        if (currLine->address == -1)
        {
            printf ("Error: Symbol Table is empty\n");
            return(foundElement);
        }
    }

    /*Do as long as we have symbols in the table or the requested symbol was not found*/
    while (tempHead && !foundElement)
    {
        if (strcmp(label, tempHead->symbolName) == 0)
        {
            foundElement = (symbol*)malloc(sizeof(symbol));
            if (!foundElement)
            {
                printf("Error: Cannot allocate memory for requested symbol to be found in the symbol table. Exiting program\n");
                exit(1);
            }

            else
            {
              strcpy(foundElement->label,tempHead->symbolName);
              foundElement->symAddress = tempHead->address;

              if(tempHead->isExternal) /*Link type is external*/
                foundElement->symLinkType = LINK_TYPE_E;

              else /*all other labels are relocatable*/
              {
                foundElement->symLinkType = LINK_TYPE_R;  
              }

            }
        }

        tempHead = tempHead->next;
    }

    if (!foundElement)
    {
        printf("Error: Cannot find label %s in the symbol table.\n",label);
    }

    return foundElement;
}

/*---------------------------------------------------------------*/

/*This method will free the allocated memory for the symbol table*/
void FreeSymbolTable()
{
    symbol_ptr temp;

    /*No table to free*/
    if (symbolHead == NULL)
        return;

    while (symbolHead)
    {
        temp = symbolHead->next;
        free (symbolHead);
        symbolHead = temp;
    }  

}
