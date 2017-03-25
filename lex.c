#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
<primitive> ::=
  box                   # closed object — rectangle
  circle                # closed object — circle
  ellipse               # closed object — ellipse
  arc                   # open object — quarter-circle
  line                  # open object — line
  arrow                 # open object — line with arrowhead
  spline                # open object — spline curve
  move
  TEXT TEXT ...         # text within invisible box
  plot <expr> TEXT      # formatted text
  ’[’ <command> ... ’]’
**/
char *primitive[10] = {"box", "circle", "ellipse", "arc", "line", "arrow", "spline", "move", "plot", "["};
/**
    up | down | left | right
**/
char *direction[4] = {"up", "down", "left", "right"};

char spec_char[8] = {'(',')','{','}',';',',', '[', ']'};
char operator_sym[10] = {'=','<','>','!','+','-','/','*','&','|'};
char identifier_sym[37] = {'a','b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_'};
char *relops[6]={"==", "!=", "<=", ">=", ">", "<"};
char *mulops[3]={"*", "/", "%"};
char *addops[2]={"+", "-"};
const int alphanum=0, specialchar=1, operator=2;
int id_count=0;
char *datatype[6] = {"int", "char"};
int cur_flag = -1;
int line=1;

typedef struct symboltable
{
    int index;
    char *name;
    char *type;
    int size;
    char scope;
    int arg_no;
    int args[10];
    char *returntype;
    struct symboltable *next;
}symtable;

symtable **array;

void insert (symtable entry)
{
    int len=strlen(entry.name);
    if (array[len%8]==NULL)
    {
        array[len%8]=malloc(sizeof(symtable));
        *(array[len%8]) = entry;
        array[len%8]->next=NULL;
    }
    else
    {
        symtable *temp=array[len%8];
        while (temp->next != NULL)
            temp=temp->next;
        temp->next = malloc(sizeof(symtable));
        *(temp->next) = entry;
        temp->next->next=NULL;
    }


}

void print_sym_table()
{
    int i;
    symtable *temp;
    printf("\nIndex\tName\tType\tSize\tScope\tNo of Arguments\tArguments\tReturn Type\n");
    for (i=0;i<8;i++)
    {
        temp = array[i];
        while (temp != NULL)
        {
            printf("%d\t%s\t%s",temp->index, temp->name, temp->type);
            if (strcmp(temp->type,"function")==0)
            {
                printf("\t\t%c\t%d\t",temp->scope,temp->arg_no);
                if (temp->arg_no==0)printf("NULL");
                else
                {
                    int p;
                    for (p=0;p<temp->arg_no;p++)
                        printf("<id, %d> ", temp->args[p]);
                }
                printf("\t%s\n", temp->returntype);
            }

            else printf("\t%d\t%c\n", temp->size, temp->scope);

            temp = temp->next;
        }
    }
}

int search_sym_table(char *buffer)
{
    int i;
    symtable *temp;
    temp = array[strlen(buffer)%8];
    while (temp!=NULL)
    {
        if (strcmp(temp->name, buffer)==0)
            return temp->index;
        temp=temp->next;
    }
    return -1;
}
//Character Search in an array of characters
int is_found(char c, char array[], int len)
{
    int i;
    for (i=0;i<len;i++)
        if (c == array[i])return 1;
    return 0;
}
//Search String in a String array 
int is_found_string(char *string, char *arr[], int n)
{
    int i;
    for (i=0;i<n;i++)
    {
        if (strcmp(string,arr[i])==0)
        {
            return i;
        }
    }
    return -1;
}

int get_char_class(char c)
{
    if (is_found(c, spec_char, 8))return specialchar;
    else if (is_found(c, operator_sym, 10))return operator;
    else if (is_found(c, identifier_sym, 37))return alphanum;
    else return -1;
}

void handle_exceptions(char c, FILE *src)
{
    if (c == '\"')
    {
        c = fgetc(src);
        while (c!='\"')
        {
            if (c=='\n')line++;
            c = fgetc(src);
        }
    }
    else if (c == '\'')
    {
        c = fgetc(src);
        while (c!='\'')
        {
            if (c=='\n')line++;
            c = fgetc(src);
        }
    }
    else if (c == '#')
    {
        while(c!='\n')
            c = fgetc(src);
        line++;
    }
    else if (c=='\n')
    {
        line++;
    }
}

int is_identifier(char *buffer)
{
    int len = strlen(buffer);
    int i;
    for (i=0;i<len;i++)
    {
        if (!((buffer[i]>=65 && buffer[i]<91) || (buffer[i]>=97 && buffer[i]<123) || (buffer[i]=='_')))
            return 0;
    }
    return 1;
}

int find_token(char *buffer, FILE *src)
{
    char c = fgetc(src);
    int this_class = get_char_class(c);
    int k=0;
    if (c == EOF)
    {
        buffer[0] = EOF;
        return -1;
    }
    if (this_class<0)
    {
        handle_exceptions(c, src);
        return find_token(buffer,src);
    }
    if (this_class == 1)
    {
        buffer[0] = c;
        buffer[1] = '\0';
        if (strcmp(buffer, ";")==0 || strcmp(buffer, "{")==0)cur_flag=-1;
        return this_class;
    }
    while (get_char_class(c) == this_class && c != EOF)
    {
        buffer[k++] = c;
        c = fgetc(src);
    }
    buffer[k]='\0';
    if (is_identifier(buffer))
    {
        if (is_found_string(buffer, keywords, 32)>=0)
        {
            int n;
            if ((n=is_found_string(buffer, datatype, 2))>=0)
            {
                cur_flag = n;
            }
        }
        else if (cur_flag>=0)
        {
            if (cur_flag != 5)
            {

                symtable entry;
                entry.index = ++id_count;
                entry.name = malloc(strlen(buffer));
                strcpy(entry.name, buffer);
                entry.type = malloc(10);
                strcpy(entry.type, datatype[cur_flag]);
                entry.size = cur_flag == 0?4:1;
                entry.scope = 'l';
                insert(entry);
            }
        }
    }

    fseek(src, -1, SEEK_CUR);
    return this_class;
}



int is_number(char *buffer)
{
    int len=strlen(buffer);
    int i;
    for (i=0;i<len;i++)
    {
        if (!((buffer[i]>='0')&&(buffer[i]<='9')))
            return 0;

    }
    return 1;
}
