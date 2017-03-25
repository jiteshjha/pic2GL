#include "q2.c"

char *cur_token;
FILE *inputfile;
char *first_declarations[2]={"int", "char"};
int error_flag=0;

void picture();

void error(char *expected)
{
    printf("Error in parsing at line %d. %s expected\n", line, expected);
    error_flag=1;
}

/**
<picture> ::=
  .PS [NUMBER [NUMBER]]\n
  <statement> ...
  .PE \n
  */

void picture() {
    if (strcmp(cur_token, ".PS") == 0) {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        
        if(is_number(cur_token)) {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);

            if(is_number(cur_token)) {
                find_token(cur_token, inputfile);
                printf("Pointing to %s\n",cur_token);

                if(is_found_string(curr_token, primitive, 10) || is_text(cur_token)) {
                    find_token(cur_token, inputfile);
                    printf("Pointing to %s\n",cur_token);

                    statement();
                }
                if(strcmp(cur_token, ".PE") == 0) {
                    find_token(cur_token, inputfile);
                    printf("Pointing to %s\n",cur_token);
                }
                else {
                    error(".PE in Picture");
                }
            }
            else {
                error("NUMBER in Picture");
            }           
        }
        else {
            error("NUMBER in Picture");
        }
    }
    else {
        error(".PS in Picture");
    }

}

/**
<statement> ::=
  <command> ;
  <command> \n
**/

void statement() {
    if(
    // <primitive> [<attribute>]
    is_found_string(curr_token, primitive, 10) ||
    // LABEL : [;] <command>
    // LABEL : [;] <command> [<position>]
    is_label(curr_token) ||
    // { <command> ... }
    strcmp(cur_token, "{") == 0 ||
    // VARIABLE [:] = <any-expr>
    is_variable(curr_token) ||
    // up | down | left | right
    is_direction(curr_token) ||
    // COMMAND-LINE
    is_command_line(curr_token) ||
    // command <print-arg> ...
    strcmp(curr_token, "command") == 0 ||
    // print <print-arg> ...
    strcmp(curr_token, "print") == 0 ||
    // sh BALANCED-TEXT
    strcmp(curr_token, "sh") == 0 ||
    /*
    copy FILENAME
    copy [FILENAME] thru MACRONAME [until TEXT]
    copy [FILENAME] thru BALANCED-BODY [until TEXT]
    */
    strcmp(curr_token, "copy") == 0 ||
    // for VARIABLE = <expr> to <expr> [by [*] <expr>] do BALANCED-BODY
    strcmp(curr_token, "for") == 0 ||
    // if <any-expr> then BALANCED-BODY [else BALANCED-BODY]
    strcmp(curr_token, "if") == 0 ||
    // reset [VARIABLE [[,] VARIABLE ...]]
    strcmp(curr_token, "reset") == 0
    ) {
        command();
        if(strcmp(curr_token, ";") == 0) {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);
        }       

    }
}


int main() {
    int count = 0;
    char filename[20];
    cur_token=malloc(20);
    array=malloc(8*sizeof(symtable*));
    printf("Enter filename: ");
    scanf("%s", filename);
    inputfile = fopen(filename, "r");
    char c;
    int i;
    find_token(cur_token, inputfile);
    printf("Pointing to %s\n",cur_token);
    program();
    if (cur_token[0]==EOF)
    {
        if (!error_flag)
            printf("Success\n");
    }
    else error("End of file");
    print_sym_table();
}
