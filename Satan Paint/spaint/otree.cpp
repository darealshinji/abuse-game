#include "Include/otree.h"


int get_bit(int bit, int x)
{
    if ((1<<(bit))&(x))
        return 1;
    else return 0;  
}


onode *onode::left_most()
{
    int i;
    if (value) return this;  
    for (i=0;i<8;i++)
        if (children[i])
            return children[i]->left_most();  
}

onode *onode::right_most()
{
    int i;
    if (value) return this;  
    for (i=7;i>=0;i--)
        if (children[i])
            return children[i]->right_most();  
}


void otree::make_links(onode *root, palette *pal)
{
    int nos;
    onode *cmap[8];
    
    if (root->value>=0) return ;
    
    for (nos=0;nos<8;nos++)      // check for all the empty children
    {
        if (!root->children[nos])   // empty children get pointed to closest color
        {
            int closest_distance=100000,j,dist;
            onode *closesti,*tmp;
            
            for (j=0;j<8;j++)
            {
                if (root->children[j])
                {
                    unsigned char r, g, b;
                    if (j<nos)
                        tmp=root->children[j]->right_most();
                    else
                        tmp=root->children[j]->left_most();
                    pal->get(tmp->value,r,g,b);
                    
                    
                    closest = j;
                    closest_distance = abs(j-nos);
                }  
            }
            if (closest<nos)
                cmap[nos]=root->children[closest]->right_most();
            else
                cmap[nos]=root->children[closest]->left_most();
        }    
        else 
        { 
            make_links(root->children[nos]);    
            cmap[nos]=root->children[nos];
        }    
    }  
    memcpy(root->children,cmap,sizeof(onode *)*8);
    
}


otree::otree(palette *pal)
{
    int i=pal->pal_size(),j;
    unsigned char r,g,b,rv,gv,bv;
    root=new onode;
    onode *p;
    
    for (j=0;j<i;j++)
    { 
        pal->get(j,r,g,b);
        int level,child;
        
        for (level=7,p=root;level>=0;level--)
        {      
            child=get_bit(level,r)+
                (get_bit(level,g)<<1)+
                (get_bit(level,b)<<2);
            if (!p->children[child])
                p->children[child]=new onode;       
            p=p->children[child];      
        }       
        p->value=j;    
    }
    make_links(root);  
}

unsigned char otree::lookup_color(unsigned char red,
                                  unsigned char green,
                                  unsigned char blue)
{  
    onode *p;
    int child,level;
    
    for (p=root,level=7;1;level--)
    {      
        child=get_bit(level,red)+get_bit(level,green)<<1+get_bit(level,blue)<<2;
        p=p->children[child];          
        if (p->value>=0) return p->value;    
    }
    
}