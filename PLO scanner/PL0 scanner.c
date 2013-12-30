// COP 3402 Homework #2 (PL0 scanner)
// Andrew Rorer
// Frank Yi

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_IDENT_LENGTH 11
#define MAX_DIGIT_LENGTH 5
#define MAX_TOKENS 1500
#define LETTER 0
#define DIGIT 1
#define SYMBOL 2
#define WHITESPACE 3

enum tokenTypes	{ nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,  slashsym = 7, oddsym = 8,  eqlsym = 9, neqsym = 10,
	lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
	beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, intsym = 29, procsym = 30, writesym = 31,
	readsym = 32, elsesym = 33
};


typedef struct
{
	char lexeme[MAX_IDENT_LENGTH + 1];
	int type;
}token;

//global variables
int currClass;
int nextClass;
int currChar = 0;
int nextChar = 0;
int nextToken;
int lexLen;
FILE *in_fp = NULL;
int numTokens = 0;
token tokenList[MAX_TOKENS];
char temp1[2];
char temp2[MAX_IDENT_LENGTH + 1];
char lexeme[MAX_IDENT_LENGTH + 1];


//function prototypes
int lookup(char ch1, char ch2);
int issymbol(int c);
int lex();
int addToken(int type, char* lexeme);
int reservedWords(char* lexeme);
int getChar();
char* addChar(char* lexeme, int newChar);
char* typeToLexeme(int type);
char* charToString(char c);

int main()
{
	in_fp = fopen("inputHw2.txt", "r");

	FILE* out_fp = fopen("output.txt", "w");

	getChar();

	int currToken = 0;
	while(1)
	{
		currToken = lex();

		if(currToken == EOF)
		{
			break;
		}

		if(currToken != 0)
		{
			addToken(currToken, lexeme);
			strcpy(lexeme, "");
		}
	}

	fclose(in_fp);
	in_fp = fopen("inputHw2.txt", "r");

	fprintf(out_fp, "Source Program:\n\n");

	char ch;
	while(1)
	{
		ch = getc(in_fp);

		if(ch == EOF)
		{
			break;
		}

		fprintf(out_fp, "%c", ch);
	}

	fprintf(out_fp, "\n\nLexeme Table:\nLexeme\tToken Type\n");

	int i;
	for (i = 0; i < numTokens; i++)
	{
		fprintf(out_fp, "%s\t\t%d\n", tokenList[i].lexeme, tokenList[i].type);
	}

	fprintf(out_fp, "\n\nLexeme List:\n");

	for (i = 0; i < numTokens; i++)
	{
		if(tokenList[i].type == identsym || tokenList[i].type == numbersym)
		{
			fprintf(out_fp, "%d %s ", tokenList[i].type, tokenList[i].lexeme);
		}

		else
		{
			fprintf(out_fp, "%d ", tokenList[i].type);
		}
	}

	fclose(in_fp);
	fclose(out_fp);

	return 0;
}


int lookup(char ch1, char ch2){
    switch(ch1){

    case '(':
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = lparentsym;
			getChar();
            break;

        case ')':
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = rparentsym;
			getChar();
            break;

        case '+':
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = plussym;
			getChar();
            break;

        case'-':
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = minussym;
			getChar();
            break;

        case'*':
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = multsym;
			getChar();
            break;

        case'/':
			if(nextChar == '*')
			{
				while(1)
				{
					getChar();

					if(currChar == '*' && nextChar == '/')
					{
						break;
					}
				}

				getChar();
				getChar();

				nextToken = 0;

				break;
			}

            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = slashsym;
			getChar();
            break;

        case'=':
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = eqlsym;
			getChar();
            break;

        case',':
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = commasym;
			getChar();
            break;

        case'.':
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = periodsym;
			getChar();
            break;

        case'<':
            strcpy(lexeme, addChar(lexeme, ch1));
            if((char)ch2 == '=')
            {
                nextToken = leqsym;
                strcpy(lexeme, addChar(lexeme, ch2));
				getChar();
            }

            else if((char)ch2 == '>')
            {
            	nextToken = neqsym;
            	strcpy(lexeme, addChar(lexeme, ch2));
				getChar();
            }
            else
                nextToken = lessym;
			getChar();
            break;

        case'>':
            strcpy(lexeme, addChar(lexeme, ch1));
			nextToken = gtrsym;
            if((char)ch2 == '=')
			{
				strcpy(lexeme, addChar(lexeme, ch2));
                nextToken = geqsym;
				getChar();
			}
			getChar();
            break;

        case';':
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = semicolonsym;
			getChar();
            break;

        case':':
            strcpy(lexeme, addChar(lexeme, ch1));
            if((char)ch2 == '=')
            {
                nextToken = becomessym;
                strcpy(lexeme, addChar(lexeme, ch2));
				getChar();
				getChar();
            }
            break;

		//TODO: fix the default case
        default:
            strcpy(lexeme, addChar(lexeme, ch1));
            nextToken = EOF;
            break;
    }
    return nextToken;
}

/*
* A function that gets the current and next characters in the file for lexical analysis.  Takes the input file pointer as its argument, and returns 1
* if there is more to read, 2 if the end of the file has been reached, and 0 if there has been an error.
*/
int getChar(){
	if(currChar == 0)
	{
		currChar = getc(in_fp);
		nextChar = getc(in_fp);
	}
	else
	{
		currChar = nextChar;
		nextChar = getc(in_fp);
	}

    if (currChar != EOF) {
    	//if currChar is whitespace run getChar again and return that value
    	if(isspace(currChar))
    	{
    		currClass = WHITESPACE;

			if(isalpha(nextChar))
            {
            	nextClass = LETTER;
            }

            else if(isdigit(nextChar))
            {
            	nextClass = DIGIT;
            }

			 else if(isspace(nextChar))
            {
            	nextClass = WHITESPACE;
            }

            else if(issymbol(nextChar))
            {
            	nextClass = SYMBOL;
            }

			return 2;
    	}

    	if (isalpha(currChar))
        {
            currClass = LETTER;

            if(isalpha(nextChar))
            {
            	nextClass = LETTER;
            }

            else if(isdigit(nextChar))
            {
            	nextClass = DIGIT;
            }

            else if(isspace(nextChar))
            {
            	nextClass = WHITESPACE;
            }

            else if(issymbol(nextChar))
            {
            	nextClass = SYMBOL;
            }

            return 1;
        }

        else if (isdigit(currChar))
        {
        	if (isalpha(nextChar))
        	{
        		printf("Error: idents cannot start with a number\n");

        		return 0;
        	}

        	else if(isdigit(nextChar))
        	{
        		nextClass = DIGIT;
        	}

        	else if(isspace(nextChar))
        	{
        		nextClass = WHITESPACE;
        	}

        	else if(issymbol(nextChar))
        	{
        		nextClass = SYMBOL;
        	}

        	currClass = DIGIT;

        	return 1;
        }

        else if(issymbol(currChar))
        {
        	currClass = SYMBOL;

        	if(isalpha(nextChar))
        	{
        		nextClass = LETTER;
        	}

        	else if(isdigit(nextChar))
        	{
        		nextClass = DIGIT;
        	}

        	else if(isspace(nextChar))
        	{
        		nextClass = WHITESPACE;
        	}

        	else if(issymbol(nextChar))
        	{
        		nextClass = SYMBOL;
        	}

        	return 1;
        }
    }

    printf("End of file\n");
	currClass = EOF;
    return 2;
}

int lex(){
    lexLen = 0;
	//getChar();

    switch (currClass){

    case SYMBOL:
    	lookup((char)currChar, (char)nextChar);
    	return nextToken;


    case LETTER:
    	strcpy(lexeme, addChar(lexeme, currChar));
		getChar();

        while ((currClass == LETTER || currClass == DIGIT)/* && (nextClass != WHITESPACE && nextClass != SYMBOL)*/){
        	strcpy(lexeme, addChar(lexeme, currChar));
        	getChar();
        }

    	//Reserved word identifier goes here
        nextToken = reservedWords(lexeme);
        break;

    // parse ints lits
    case DIGIT:
    	strcpy(lexeme, addChar(lexeme, currChar));
    	getChar();
    	while (currClass == DIGIT){
    		strcpy(lexeme, addChar(lexeme, currChar));

			if(strlen(lexeme) > MAX_DIGIT_LENGTH)
			{
				printf("Error: max digit length exceeded");
				break;
			}
    		getChar();
    	}
    	nextToken = numbersym;
    	break;

	case WHITESPACE:
		getChar();
		nextToken = 0;
		break;

    //EOF
    case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        break;
    }
    return nextToken;
}

/*
* A function that adds the token indicated by argument type to the token list.  Takes an integer for the token type and a string to contain the lexeme,
* and returns 1 if successful and 0 if there is a failure.
*/
int addToken(int type, char* lexeme)
{
	//if the numTokens counter exceeds the memory of the array return 0 to show a failure
	if(numTokens > MAX_TOKENS)
	{
		return 0;
	}

	//store the token in tokenList
	tokenList[numTokens].type = type;
	strcpy(tokenList[numTokens].lexeme, lexeme);

	numTokens++;

	return 1;
}

char* typeToLexeme(int type)
{
	switch(type)
	{
	case nulsym:
		return "\0";

	case identsym:
		return lexeme;

	case numbersym:
		return lexeme;

	case plussym:
		return "+";

	case minussym:
		return "-";

	case multsym:
		return "*";

	case slashsym:
		return "/";

	case oddsym:
		//TODO: figure out what this actually is
		break;

	case eqlsym:
		return "=";

	case neqsym:
		return "<>";

	case lessym:
		return "<";

	case leqsym:
		return "<=";

	case gtrsym:
		return ">";

	case geqsym:
		return ">=";

	case lparentsym:
		return "(";

	case rparentsym:
		return ")";

	case commasym:
		return ",";

	case semicolonsym:
		return ";";

	case periodsym:
		return ".";

	case becomessym:
		return ":=";

	case beginsym:
		return "begin";

	case endsym:
		return "end";

	case ifsym:
		return "if";

	case thensym:
		return "then";

	case whilesym:
		return "while";

	case dosym:
		return "do";

	case callsym:
		return "call";

	case constsym:
		return "const";

	case intsym:
		return "int";

	case procsym:
		return "procedure";

	case writesym:
		return "write";

	case readsym:
		return "read";

	case elsesym:
		return "else";
	}

	return "Error: token type not found";
}

/*
* A function that adds a character to the existing lexeme.  Takes the current lexeme as its argument and returns the lexeme with curr added to the end.
*/
char* addChar(char* lexeme, int newChar)
{
	if(strlen(lexeme) < MAX_IDENT_LENGTH)
	{
		strcpy(temp2, lexeme);
	}

	else
	{
		printf("Error: identifiers can only contain 11 characters");
	}

	strcat(temp2, charToString(newChar));

	return temp2;
}

int reservedWords(char* lexeme)
{
	if(strcmp(lexeme, "const") == 0)
		return constsym;
	else if(strcmp(lexeme, "int") == 0)
		return intsym;
	else if(strcmp(lexeme, "procedure") == 0)
		return procsym;
	else if(strcmp(lexeme, "call") == 0)
        return callsym;
    else if(strcmp(lexeme, "begin") == 0)
        return beginsym;
    else if(strcmp(lexeme, "end") == 0)
    	return endsym;
    else if(strcmp(lexeme, "if") == 0)
        return ifsym;
    else if(strcmp(lexeme, "then") == 0)
        return thensym;
    else if(strcmp(lexeme, "while") == 0)
        return whilesym;
    else if(strcmp(lexeme, "do") == 0)
        return dosym;
    else if(strcmp(lexeme, "read") == 0)
        return readsym;
    else if(strcmp(lexeme, "write") == 0)
        return writesym;
    else
    	return identsym;
}

char* charToString(char c)
{
	temp1[0] = c;
	temp1[1] = '\0';

	return temp1;
}

int issymbol(int c)
{
	if((c >= 40 && c <= 47) || (c >= 58 && c <= 62))
	{
		return 1;
	}

	return 0;
}
