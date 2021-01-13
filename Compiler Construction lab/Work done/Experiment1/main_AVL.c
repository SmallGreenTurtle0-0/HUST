#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLe 20
#define MAXLi 20
#define MAXW 200
typedef struct pair_st {
    int line;
    int order;
} pair;
typedef struct node_st {
    char key[MAXLe];
    int nk;
    struct node_st *left;
    struct node_st *right;
    pair location[MAXW];
} node;
node* createNode(char key[], int line, int order) {
    node* ans;
    ans = (node*)malloc(sizeof(node));
    ans->nk = 1;
    ans->location[0].line = line;
    ans->location[0].order = order;
    return ans;
}
typedef struct tree_st {
    int size;
    node* root;
} tree;
tree* createNullTree() {
    tree* t;
    t = (tree*)malloc(sizeof(tree));
    t->size = 0;
    t->root = NULL;
}
void printnode(node a) {
    printf("%s, %d", a.key, a.nk);
    for(int i = 0; i < a.nk; i++)
        printf(", (%d, %d)", a.location[i].line, a.location[i].order);
    puts("");
}
int nodecmp(node a, node b) {
    return strcmp(a.key, b.key);
}

node* insert(node* x, tree* t) {
    if(t->root == NULL) {
        t->root = x;
        t->size = 1;
        return t->root;
    }
    node* cur;
    cur = (node*)malloc(sizeof(node));
    cur = t->root;
    while(cur != NULL) {
        int tmp = nodecmp(*x, *cur);
        if(tmp == 0) {
            cur->nk++;
            cur->location[cur->nk - 1] = x->location[0];
            return cur;
        }
        if(tmp == -1) {
            if(cur->left == NULL) {
                cur->left = x;
                t->size++;
                return cur->left;
            }
            cur = cur->left;
        }
        else {
            if(cur->right == NULL) {
                cur->right = x;
                t->size++;
                return cur->right;
            }
            cur = cur->right;
        }
    }
}
void inorderTravel(node* x) {
    if(x == NULL)
        return;
    inorderTravel(x->left);
    printnode(*x);
    inorderTravel(x->right);
}
int ns;
char stopw[MAXW][MAXLe];
void ReadStopwList() {
    char word[MAXLe];
    ns = 0;
    FILE *f;
    f = fopen("stopw.txt", "r");
    while(fscanf(f, "%s", word) != EOF) {
        strcpy(stopw[ns], word);
        //printf("%s\n", stopw[ns]);
        ns++;
    }
    //puts("ok readstropwlist");
    fclose(f);
}
int isletter(char x) {
    if((x>='a' && x <= 'z') && (x>='A' && x<='Z'))
        return 1;
    return 0;
}
int notStopw(char w[]) {
    for(int i = 0; i < ns; i++)
        if(!strcpy(w, stopw[i]))
            return 0;
    return 1;
}
tree* t;
void BuildBST() {
    FILE *f;
    f = fopen("vanban.txt", "r");
    char line[MAXLe*MAXW];
    char word[MAXLe];
    char word_lower[MAXLe];
    int ns=0;
    while(fgets(line, MAXLe*MAXW, f) != NULL) {
        ns++;
        int nw = 0;
        int start = -1;
        memset(word, '\0', sizeof(word));
        memset(word_lower, '\0', sizeof(word_lower));
        for(int i = 0; i < strlen(line); i++) {
            char x = line[i];
            if(isletter(x) && start == -1)
                start = i;
            if(!isletter(x) && start != -1) {
                strncpy(word, line+start, i-start);
                start = -1; nw++;
                strcpy(word_lower, tolower(word));
                if(notStopw(word_lower))
                    insert(createNode(word_lower, ns, nw), t);
            }
        }
    }
    fclose(f);
}
int main() {
    ReadStopwList();
    BuildBST();
    inorderTravel(t->root);
    return 0;
}
