#include "q2.c"

char *cur_token;
FILE *inputfile;
char *first_declarations[2]={"int", "char"};
int error_flag=0;

void id_list();
void tprime();
void statement_list();

void error(char *expected)
{
    printf("Error in parsing at line %d. %s expected\n", line, expected);
    error_flag=1;
}

void data_type()
{
    if (strcmp("int",cur_token)==0 || strcmp("char",cur_token)==0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
    }
    else error("int or char");
}

void id_list_dash2()
{
    if (strcmp(cur_token, ",")==0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        id_list();
    }

}

void id_list_dash()
{
    if (strcmp(cur_token, ",")==0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        id_list();
    }
    else if (strcmp(cur_token, "[")==0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n", cur_token);
        if (is_number(cur_token))
        {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n", cur_token);
            if (strcmp(cur_token, "]")==0)
            {
                find_token(cur_token, inputfile);
                printf("Pointing to %s\n", cur_token);

            }
            else error("]");
            id_list_dash2();
        }
        else error("[");
    }
}

void id_list()
{
    if (search_sym_table(cur_token)>=0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        id_list_dash();
    }
    else error("identifier");
}

void declarations()
{
    if (is_found_string(cur_token, first_declarations, 2)>=0)
    {
        data_type();
        id_list();
        if (strcmp(cur_token,";")==0)
        {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);
            declarations();
        }
    }
}

void relop()
{
    if (is_found_string(cur_token, relops, 6)>=0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);

    }
    else error("relational operator");

}

void addop()
{
    if (is_found_string(cur_token, addops, 2)>=0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);

    }
    else error("+ or -");

}

void mulop()
{
    if (is_found_string(cur_token, mulops, 3)>=0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);

    }
    else error("* or / or %");

}

void factor()
{
    if (search_sym_table(cur_token)>=0 || is_number(cur_token))
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
    }
    else error("identifier or number");
}

void tprime()
{
    if (is_found_string(cur_token, mulops, 3)>=0)
    {
        mulop();
        factor();
        tprime();
    }
}

void term()
{
    factor();
    tprime();
}

void seprime()
{
    if (is_found_string(cur_token, addops, 2)>=0)
    {
        addop();
        term();
        seprime();
    }
}

void simple_expn()
{
    term();
    seprime();
}

void eprime()
{
    if (is_found_string(cur_token, relops, 6)>=0)
    {
        relop();
        simple_expn();
    }
}

void expn()
{
    simple_expn();
    eprime();
}

void assign_stat()
{
    if (search_sym_table(cur_token)>=0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        if (strcmp(cur_token,"=")==0)
        {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);
            expn();
        }
        else error("=");
    }
    else error("identifier");
}

void dprime()
{
    if (strcmp(cur_token, "else")==0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        if (strcmp(cur_token, "{")==0)
        {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);
            statement_list();
            if (strcmp(cur_token, "}")==0)
            {
                find_token(cur_token, inputfile);
                printf("Pointing to %s\n",cur_token);
            }
            else error("}");
        }
        else error("{");
    }
}

void decision_stat()
{
    if (strcmp(cur_token, "if")==0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        if (strcmp(cur_token, "(")==0)
        {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);
            expn();
            if (strcmp(cur_token, ")")==0)
            {
                find_token(cur_token, inputfile);
                printf("Pointing to %s\n",cur_token);
                if (strcmp(cur_token, "{")==0)
                {
                    find_token(cur_token, inputfile);
                    printf("Pointing to %s\n",cur_token);

                    statement_list();
                    if (strcmp(cur_token, "}")==0)
                    {
                        find_token(cur_token, inputfile);
                        printf("Pointing to %s\n",cur_token);
                        dprime();
                    }
                    else error("}");
                }
                else error("{");
            }
            else error(")");
        }
        else error("(");
    }
    else error("if");
}

void looping_stat()
{
    if (strcmp(cur_token, "while")==0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        if (strcmp(cur_token, "(")==0)
        {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);
            expn();
            if (strcmp(cur_token, ")")==0)
            {
                find_token(cur_token, inputfile);
                printf("Pointing to %s\n",cur_token);
                if (strcmp(cur_token,"{")==0)
                {
                    find_token(cur_token, inputfile);
                    printf("Pointing to %s\n",cur_token);
                    statement_list();
                    if (strcmp(cur_token, "}")==0)
                    {
                        find_token(cur_token, inputfile);
                        printf("Pointing to %s\n",cur_token);
                    }
                    else error("}");
                }
                else error("{");
            }
            else error(")");
        }
        else error("(");
    }
    else if (strcmp(cur_token, "for")==0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        if (strcmp(cur_token, "(")==0)
        {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);
            assign_stat();
            if (strcmp(cur_token, ";")==0)
            {
                find_token(cur_token, inputfile);
                printf("Pointing to %s\n",cur_token);
                expn();
                if (strcmp(cur_token, ";")==0)
                {
                    find_token(cur_token, inputfile);
                    printf("Pointing to %s\n",cur_token);
                    assign_stat();
                    if (strcmp(cur_token, ")")==0)
                    {
                        find_token(cur_token, inputfile);
                        printf("Pointing to %s\n",cur_token);
                        if (strcmp(cur_token,"{")==0)
                        {
                            find_token(cur_token, inputfile);
                            printf("Pointing to %s\n",cur_token);
                            statement_list();
                            if (strcmp(cur_token, "}")==0)
                            {
                                find_token(cur_token, inputfile);
                                printf("Pointing to %s\n",cur_token);
                            }
                            else error("}");
                        }
                        else error("{");
                    }
                    else error(")");
                }
                else error(";");
            }
            else error(";");
        }
        else error("(");
    }
}

void statement()
{
    if (strcmp(cur_token, "if")==0)
    {
        decision_stat();
    }
    else if (strcmp(cur_token, "while")==0 || strcmp(cur_token, "for")==0)
    {
        looping_stat();
    }
    else if (search_sym_table(cur_token)>=0)
    {
        assign_stat();
        if (strcmp(cur_token, ";")==0)
        {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);
        }
        else error(";");
    }
    else error("loop or decision or identifier");
}

void statement_list()
{
    if (search_sym_table(cur_token)>=0 || strcmp(cur_token, "if")==0 || strcmp(cur_token, "for")==0 || strcmp(cur_token, "while")==0)
    {
        statement();
        statement_list();
    }
}

void program()
{
    if (strcmp(cur_token,"main")==0)
    {
        find_token(cur_token, inputfile);
        printf("Pointing to %s\n",cur_token);
        if (strcmp(cur_token, "(")==0)
        {
            find_token(cur_token, inputfile);
            printf("Pointing to %s\n",cur_token);
            if (strcmp(cur_token, ")") == 0)
            {
                find_token(cur_token, inputfile);
                printf("Pointing to %s\n",cur_token);
                if (strcmp(cur_token, "{")==0)
                {
                    find_token(cur_token, inputfile);
                    printf("Pointing to %s\n",cur_token);
                    declarations();
                    statement_list(); 
                    if (strcmp(cur_token,"}")==0)
                    {
                        find_token(cur_token, inputfile);
                        return;
                    }
                    else 
                    {
                        error("}");
                    }
                }
                else error("{");
            }
            else error(")");
        } 
        else error("(");
    }
    else error("main");
}

int main()
{
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
