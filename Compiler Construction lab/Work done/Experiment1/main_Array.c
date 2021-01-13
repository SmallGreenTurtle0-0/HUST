#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLe 20
#define MAXLi 20
#define MAXW 200

typedef struct PAIR {
    int line, order;
} _PAIR;
typedef struct NODE {
    char key[MAXLe];
    int nx;
    _PAIR location[MAXLi];
} _NODE;
int nodecmp(_NODE a, _NODE b) {
    return strcmp(a.key, b.key);
}
_NODE db[MAXW];
int n;
void insert(char x[], int line, int order) {
    for(int i = 0; i < n; i++)
        if(strcmp(x, db[i].key) == 0) {
            db[i].nx++;
            db[i].location[db[i].nx-1].line = line;
            db[i].location[db[i].nx-1].order = order;
            //printf("%s (%d, %d)\n", db[i].key, db[i].location[db[i].nx-1].line, db[i].location[db[i].nx-1].order);
            return;
        }
    n++;
    strcpy(db[n-1].key, x);
    db[n-1].nx = 1;
    db[n-1].location[db[n-1].nx-1].line = line;
    db[n-1].location[db[n-1].nx-1].order = order;
    //printf("%s (%d, %d)\n", db[n-1].key, db[n-1].location[db[n-1].nx-1].line, db[n-1].location[db[n-1].nx-1].order);
    return;
}
int notletter(char x) {
    if((x>='a' && x<='z') || (x>='A' && x<='Z'))
        return 0;
    return 1;
}

char line[MAXW];
char stopw[MAXW][MAXLe];
int ns;
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
int main()
{
    FILE* f2;
    ns = 0;
    f2 = fopen("stopw.txt", "r");
    while(fgets(line, MAXLe, f2) != NULL) {
        strcpy(stopw[ns], line);
        stopw[ns][strlen(stopw[ns])-1] = '\0';
        //printf("%s\n", stopw[ns]);
        ns++;
    }
    fclose(f2);
    FILE *f1;
    f1 = fopen("vanban.txt", "r");
    n = 0;
    int nl = 0;
    while(fgets(line, MAXW, f1) != NULL) {
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
                    insert(tmp, nl, nw);
                }
                start = -1;
            }
        }
    }
    for(int i = 0; i < n; i++)
        for(int j = i+1; j < n; j++)
            if(nodecmp(db[i], db[j]) == 1) {
                _NODE TMP = db[i];
                db[i] = db[j];
                db[j] = TMP;
            }

    for(int i = 0; i < n; i++)
        if(checkStopW(db[i].key) == 0) {
            printf("%s, %d", db[i].key, db[i].nx);
                for(int j = 0; j < db[i].nx; j++)
                printf(", (%d, %d)", db[i].location[j].line, db[i].location[j].order);
            puts("");
        }
    fclose(f1);

    return 0;
}


