#include "Include/id.h"

int cur_id=LAST_ID;

int new_id()
{
    return cur_id++;
}

class pm_command {
public:
    int id;
    char *command;
    pm_command *next;
    pm_command(int ID, char *Command) {
        id = ID;
        command = strcpy((char*) jmalloc(strlen(Command) + 1, "command name"), Command);
    }
};

pm_command *first_id_command=NULL;

int painter::addc(char *command)
{
    int ni=new_id();
    pm_command *p=new pm_command(ni,command);
    p->next=first_id_command;
    first_id_command=p;
    return id;
}


void painter::do_idcommand(int id)
{
    for (pm_command *p=first_id_command;p;p=p->next)
    {
        if (p->id==id)
        {
            do_command(p->command);
            return ;
        }
    }
}


pmenu *painter::create_menu()
{
    pmenu_item *tm=new pmenu_item("Texture",new psub_menu(
        new pmenu_item(ID_LOAD_TEXTURE, "Load texture         ",-1,
        new pmenu_item(0,               "Select active        ",-1,
        new pmenu_item(ID_APPLY_TEXTURE,"Apply to segments (a)",'a',
        NULL))),NULL),NULL);
    
    pmenu_item *fm=new pmenu_item("File",new psub_menu(
        new pmenu_item(addc(",       "Load      (l)",'l',
        new pmenu_item(ID_SAVEAS,     "Save as   (s)",'s',
        new pmenu_item(ID_SAVE,       "Save      (S)",'S',
        new pmenu_item(0,       NULL,        -1,
        new pmenu_item(ID_FRAMES,     "Set frames  ",-1,
        new pmenu_item(0,       NULL,        -1,
        new pmenu_item(ID_QUIT,       "Quit      (q)",'q',NULL)))))))),
        NULL),em);
    
    pmenu *pm=new pmenu
}
