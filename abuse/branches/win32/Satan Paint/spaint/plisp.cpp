void clisp_init()                      // external initalizer call by lisp_init()
{ 
    add_c_bool_fun("zoom",3,3,              0);   // zoom amount mousex mousey
    add_c_bool_fun("clear_unsed",0,0,       1);
    add_c_bool_fun("blend",0,0,             2);
    
    add_c_function("distx",0,0,                   1);
    add_c_function("disty",0,0,                   2);  
}

long c_caller(long number, void *arg)  // exten c function switches on number
{
    switch (number) {
        case 0:
            break;
    }
}

void *l_caller(long number, void *arg, void *env);  // exten lisp function switches on number