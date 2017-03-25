#include "q2.c"

char * cur_token;
FILE * inputfile;
char * first_declarations[2] = {
    "int",
    "char"
};
int error_flag = 0;

void picture();

void error(char * expected) {
    printf("Error in parsing at line %d. %s expected\n", line, expected);
    error_flag = 1;
}

/*
    Check First(command) set compliance
*/
int check_first_command() {

    if (
        // <primitive> [<attribute>]
        is_found_string(cur_token, primitive_set, 10) || is_text(cur_token)
        // LABEL : [;] <command>
        // LABEL : [;] <command> [<position>]
        is_label(cur_token) ||
        // { <command> ... }
        strcmp(cur_token, "{") == 0 ||
        // VARIABLE [:] = <any-expr>
        is_variable(cur_token) ||
        // up | down | left | right
        is_found_string(cur_token, direction_set, 4) ||
        // COMMAND-LINE
        is_command_line(cur_token) ||
        // command <print-arg> ...
        strcmp(cur_token, "command") == 0 ||
        // print <print-arg> ...
        strcmp(cur_token, "print") == 0 ||
        // sh BALANCED-TEXT
        strcmp(cur_token, "sh") == 0 ||
        /*
        copy FILENAME
        copy [FILENAME] thru MACRONAME [until TEXT]
        copy [FILENAME] thru BALANCED-BODY [until TEXT]
        */
        strcmp(cur_token, "copy") == 0 ||
        // for VARIABLE = <expr> to <expr> [by [*] <expr>] do BALANCED-BODY
        strcmp(cur_token, "for") == 0 ||
        // if <any-expr> then BALANCED-BODY [else BALANCED-BODY]
        strcmp(cur_token, "if") == 0 ||
        // reset [VARIABLE [[,] VARIABLE ...]]
        strcmp(cur_token, "reset") == 0
    ) {
        return 1;
    }

    return 0;
}

/*
    Check First(primitive) compliance

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

void check_first_primitive() {

    char * primitive_set[10] = {
        "box",
        "circle",
        "ellipse",
        "arc",
        "line",
        "arrow",
        "spline",
        "move",
        "plot",
        "["
    };

    if (is_found_string(cur_token, primitive_set, 10) || is_text(cur_token)) {
        return 1;
    }

    return 0;
}

/**
<picture> ::=
  .PS [NUMBER [NUMBER]]\n
  <statement> ...
  .PE \n
  */

void picture() {

    find_token(cur_token, inputfile);

    if (strcmp(cur_token, ".PS") == 0) {
        printf("Matched to %s\n", cur_token);

        find_token(cur_token, inputfile);

        if (is_number(cur_token)) {
            printf("Matched to %s\n", cur_token);
            find_token(cur_token, inputfile);

            if (is_number(cur_token)) {
                printf("Matched to %s\n", cur_token);
                find_token(cur_token, inputfile);
            }
        }

        // first(statement) = first(command)
        while (check_first_command() == 1) {
            fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);
            statement();
            find_token(cur_token, inputfile);
        }

        if (strcmp(cur_token, ".PE") == 0) {
            printf("Matched to %s\n", cur_token);
        } else {
            error(".PE in Picture");
        }
    } else {
        error(".PS in Picture");
    }

}

/**
<statement> ::=
  <command> ;
  <command> \n
**/

void statement() {
    find_token(cur_token, inputfile);

    if (check_first_command() == 1) {
        fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);

        command();

        find_token(cur_token, inputfile);
        if (strcmp(cur_token, ";") == 0) {
            printf("Matched to %s\n", cur_token);
        } else {
            fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);
        }
    } else {
        error("command in statement");
    }
}

/**
<command> ::=
  <primitive> [<attribute>]
  LABEL : [;] <command>
  LABEL : [;] <command> [<position>]
  { <command> ... }
  VARIABLE [:] = <any-expr>
  up | down | left | right
  COMMAND-LINE
  command <print-arg> ...
  print <print-arg> ...
  sh BALANCED-TEXT
  copy FILENAME
  copy [FILENAME] thru MACRONAME [until TEXT]
  copy [FILENAME] thru BALANCED-BODY [until TEXT]
  for VARIABLE = <expr> to <expr> [by [*] <expr>] do BALANCED-BODY
  if <any-expr> then BALANCED-BODY [else BALANCED-BODY]
  reset [VARIABLE [[,] VARIABLE ...]]
  **/

void command() {
    find_token(cur_token, inputfile);

    // <primitive> [<attribute>]
    if (is_found_string(cur_token, primitive, 10) || is_text(cur_token)) {
        printf("Matched to %s\n", cur_token);

        find_token(cur_token, inputfile);
        if (check_first_attribute() == 1) {
            attribute();
        } else {
            fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);
        }
    }

    /**
      LABEL : [;] <command>
      LABEL : [;] <command> [<position>]
    **/
    else if (strcmp(cur_token, "LABEL") == 0) {
        printf("Matched to %s\n", cur_token);

        find_token(cur_token, inputfile);
        if (strcmp(cur_token, ":") == 0) {
            printf("Matched to %s\n", cur_token);

            find_token(cur_token, inputfile);
            if (strcmp(cur_token, ";") == 0) {
                printf("Matched to %s\n", cur_token);
            } else {
                fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);
            }

            command();

            find_token(cur_token, inputfile);
            if (check_first_position() == 1) {
                position();
            } else {
                fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);
            }
        } else {
            error("; in command");
        }
    }

    // { <command> ... }
    else if (strcmp(cur_token, "{") == 0) {
        printf("Matched to %s\n", cur_token);

        find_token(cur_token, inputfile);

        while (check_first_command() == 1) {
            fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);
            command();
            find_token(cur_token, inputfile);
        }
        
        if (strcmp(cur_token, "}") == 0) {
            printf("Matched to %s\n", cur_token);
        } else {
            error("} in command");
        }
    }

    // VARIABLE [:] = <any-expr>
    else if (check_first_variable() == 1) {

        find_token(cur_token, inputfile);

        if (strcmp(cur_token, ":") == 0) {
            printf("Matched to %s\n", cur_token);
            find_token(cur_token, inputfile);
        }

        if (strcmp(cur_token, "=") == 0) {
            printf("Matched to %s\n", cur_token);

            any_expr();
        } else {
            error("= in command");
        }

    } 
    // up | down | left | right
    else if(is_found_string(cur_token, direction_set, 4)) {
        printf("Matched to %s\n", cur_token);
    }
    //COMMAND-LINE
    else if(is_command_line() == 1) {
        printf("Matched to %s\n", cur_token);
    }
    // command <print-arg> ...
    // print <print-arg> ...
    else if (strcmp(cur_token, "command") == 0 || strcmp(cur_token, "print") == 0) {
        find_token(cur_token, inputfile);

        while(check_first_print_arg() == 1) {
            fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);
            print_arg();
            find_token(cur_token, inputfile);
        }
        fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);
    }
    else if (strcmp(cur_token, "sh") == 0) {
        find_token(cur_token, inputfile);

        if (is_balanced_body(cur_token) == 0) {
            printf("Matched to %s\n", cur_token);
        } else {
            error("balanced text in command");
        }
    }
    /*
    Skipped:
    copy FILENAME
    copy [FILENAME] thru MACRONAME [until TEXT]
    copy [FILENAME] thru BALANCED-BODY [until TEXT]
    */

    // for VARIABLE = <expr> to <expr> [by [*] <expr>] do BALANCED-BODY
    else if(strcmp(cur_token, "for") == 0) {
        find_token(cur_token, inputfile);

        if(is_variable(cur_token) == 0) {
            printf("Matched to %s\n", cur_token);

            find_token(cur_token, inputfile);
            if(strcmp(cur_token, "=") == 0) {
                printf("Matched to %s\n", cur_token);
                find_token(cur_token, inputfile);

                if(is_expr(cur_token) == 1) {
                    printf("Matched to %s\n", cur_token);
                    find_token(cur_token, inputfile);
                    
                    if(strcmp(cur_token, "to") == 0) {
                        printf("Matched to %s\n", cur_token);
                        find_token(cur_token, inputfile);
                        
                        if(is_expr(cur_token) == 1) {
                            printf("Matched to %s\n", cur_token);
                            
                            find_token(cur_token, inputfile);

                            if(strcmp(cur_token, "by") == 0) {
                                printf("Matched to %s\n", cur_token);
                                find_token(cur_token, inputfile);

                                if(strcmp(cur_token, "*") == 0) {
                                    printf("Matched to %s\n", cur_token);
                                    find_token(cur_token, inputfile);
                                }

                                if(is_expr(cur_token) == 1) {
                                    printf("Matched to %s\n", cur_token);
                                    find_token(cur_token, inputfile);
                                }
                                else {
                                    error("Expr in command");
                                }
                            }

                            if(strcmp(cur_token, "do") == 0) {
                                printf("Matched to %s\n", cur_token);
                                find_token(cur_token, inputfile);

                                if(is_balanced_body(cur_token) == 1) {
                                    printf("Matched to %s\n", cur_token);
                                }
                                else {
                                    error("Balanced_text in command");
                                }
                            }
                            else {
                                error("do in command");
                            }

                        } else {
                            error("expr in command");
                        }
                        
                    } else {
                        error("expr in command");
                    }
                } else {
                    error("expr in command");
                }
            } else {
                error("= in command");
            }
        } else {
            error("VARIABLE in command");
        }
    }

    // if <any-expr> then BALANCED-BODY [else BALANCED-BODY]
    else if(strcmp(cur_token, "if") == 0) {
        find_token(cur_token, inputfile);
        if(is_any_expr(cur_token) == 1) {
            printf("Matched to %s\n", cur_token);
            find_token(cur_token, inputfile);
            
            if(strcmp(cur_token, "then") == 0) {
                printf("Matched to %s\n", cur_token);
                find_token(cur_token, inputfile);
            
                if(is_balanced_body(cur_token) == 1) {
                    printf("Matched to %s\n", cur_token);
                    find_token(cur_token, inputfile);

                    if(strcmp(cur_token, "else") == 0) {
                        printf("Matched to %s\n", cur_token);
                        find_token(cur_token, inputfile);

                        if(is_balanced_body(cur_token) == 1) {
                            printf("Matched to %s\n", cur_token);
                        } else {
                            error("balanced_body in command");
                        }
                    } else {
                        fseek(inputfile, -1 * strlen(cur_token), SEEK_CUR);
                    }                   
                } else {
                    error("any_expr in command");
                }
                
            } else {
                error("any_expr in command");
            }

        } else {
            error("any_expr in command");
        }
    }
    /*
        Skipped:
        reset [VARIABLE [[,] VARIABLE ...]]
    */
    else {
        error("Expected first(statement)");
    }
}

/**
<print-arg> ::=
  TEXT
 <expr>
 <position>
 
  **/

void print_arg() {
    find_token(cur_token, inputfile);

    if(is_text(cur_token) == 1) {
        printf("Matched to %s\n", cur_token);
    }
    else if(check_first_expr() == 1) {
        expr();
    }
    else if(check_first_position() == 1) {
        position();
    }
    else {
        error("TEXT | expr | position in print_arg");
    }
}

int main() {
    int count = 0;
    char filename[20];
    cur_token = malloc(20);
    array = malloc(8 * sizeof(symtable * ));
    printf("Enter filename: ");
    scanf("%s", filename);
    inputfile = fopen(filename, "r");
    char c;
    int i;

    picture();
    if (cur_token[0] == EOF) {
        if (!error_flag) {
            printf("Success\n");
        }
    } else {
        error("End of file");
    }
    print_sym_table();
}