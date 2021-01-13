#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLe 20
#define MAXLi 20
#define MAXW 200
typedef struct pair_st{
    int line;
    int order;
} pair;
typedef struct node_st {
    char key[MAXLe];
    int nk;
    struct node_st* left;
    struct node_st* right;
    pair location[MAXW];
} node;
typedef struct tree_st {
    int size;
    node* root;
} tree;

node* createNode(char key[], int line, int order) {
    node* x;
    x = (node*)malloc(sizeof(node));
    x->nk = 1;
    strcpy(x->key, key);
    x->location[0].line = line;
    x->location[0].order = order;
    x->left = NULL;
    x->right = NULL;
    return x;
}
tree* createNullTree() {
    tree* t;
    t = (tree*)malloc(sizeof(t));
    t->size = 0;
    t->root = NULL;
    return t;
}
int nodecmp(node a, node b) {
    return strcmp(a.key, b.key);
}
void printNode(node* a) {
    printf("%s, %d", a->key, a->nk);
    for(int i = 0; i < a->nk; i++)
        printf(", (%d, %d)", a->location[i].line, a->location[i].order);
    puts("");
}
void insert(node* x, tree* t) {
    //puts("---inner insert----");
    if(t->root == NULL) {
        //puts("NULL case");
        (t->root) = x;
        t->size = 1;
        //printNode((t->root));
        return;
    }
    node *cur;
    cur = (node*)malloc(sizeof(node));
    cur = (t->root);

    while(cur != NULL) {
        int tmp = nodecmp(*x, *cur);
        //puts("compare: ");
        //printNode(x), printNode(cur);
        //printf("tmp = %d\n", tmp);
        if(tmp == 0) {
            //puts("INTREE case");
            cur->nk++;
            cur->location[cur->nk -1] = x->location[0];
            return;
        }
        if(tmp == -1) {
            if(cur->left == NULL) {
                cur->left = x;
                //puts("OUTTREE case");
                t->size++;
                return;
            }
            else
                cur = (cur->left);
        }
        else {
            if(cur->right == NULL) {
                cur->right = x;
                //puts("OUTTREE case");
                t->size++;
                return;
            }
            else
                cur = (cur->right);
        }
    }

}
int ns;
char stopw[MAXW][MAXLe];
void ReadStopW() {
    FILE* f;
    ns = 0;
    f = fopen("stopw.txt", "r");
    char line[MAXLe];
    while(fgets(line, MAXLe, f) != NULL) {
        strcpy(stopw[ns], line);
        stopw[ns][strlen(stopw[ns])-1] = '\0';
        //printf("%s\n", stopw[ns]);
        ns++;
    }
    fclose(f);
}
void inorderTravesal(node* cur) {
    if(cur == NULL)
        return;
    inorderTravesal(cur->left);
    printNode(cur);
    inorderTravesal(cur->right);
}
tree* t;
int notletter(char x) {
    if((x>='a' && x<='z') || (x>='A' && x<='Z'))
        return 0;
    return 1;
}
int checkStopW(char x[]) {
    //printf("check => %s", x);
    for(int i = 0; i < ns; i++)
        if(strcmp(x, stopw[i]) == 0) {
            //puts("1");
            return 1;
        }
    //puts("0");
    return 0;
}
int main() {
    ReadStopW();
    FILE *f;
    f = fopen("vanban.txt", "r");
    t = createNullTree();
    int nl = 0;
    char line[MAXLe*MAXW];
    while(fgets(line, MAXW*MAXLe, f) != NULL) {
        //printf("%s\n", line);
        nl++;
        int nw = 0, start = -1;
        for(int i = 0; i < strlen(line); i++) {
            if(!notletter(line[i]) && start == -1)
                start = i;
            if(notletter(line[i])) {
                if(start != -1) {
                    char tmp[MAXLe];
                    memset(tmp, '\0', sizeof tmp);
                    strncpy(tmp, line+start, i-start);
                    for(int j = 0; j < strlen(tmp); j++)
                        if(tmp[j] <=  'Z')
                            tmp[j] = ((tmp[j] - 'A') + 'a');
                    nw++;
                    if(!checkStopW(tmp)) {
                        node* _new;
                        _new = createNode(tmp, nl, nw);
                        insert(_new, t);
                    }
                }
                start = -1;
            }
        }
    }
    inorderTravesal(t->root);
    fclose(f);
    return 0;
}
