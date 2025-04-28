#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define MAX_SIZE 30000

enum{
/* IDENTIFIERS AND CONSTANTS */
    ID, CT_INT, CT_REAL, CT_CHAR, CT_STRING,
/* KEYWORDS*/
    BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE,
/* DELIMITATORS */
    COMMA, SEMICOLON, LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC, END,
/* OPERATORS*/
    ADD, SUB, MUL, DIV, DOT, AND, OR, NOT, ASSIGN, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ
};

typedef struct _Token{
    int code; // code(name)
    union{
        char *text; // used for ID, CT_STRING
        int i; // used for CT_INT, CT_CHAR
        double r; // used for CT_REAL
    };
    int line; // entry file line
    struct _Token *next;
}Token;

int line=1;
Token *lastToken, *tokens, *iTk, *consumedTk;
char buffin[30001];
char *pCrtCh;
char *a[]={"ID", "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING", "BREAK",
            "CHAR", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN",
            "STRUCT", "VOID", "WHILE", "COMMA", "SEMICOLON", "LPAR",
            "RPAR","LBRACKET","RBRACKET", "LACC", "RACC", "END", "ADD", "SUB", "MUL", "DIV", "DOT", "AND",
            "OR", "NOT", "ASSIGN", "EQUAL","NOTEQ", "LESS",
            "LESSEQ", "GREATER", "GREATEREQ"};

int getNextToken();

char *createString(const char *pStartCh, char *pCrtCh){

    int size = pCrtCh-pStartCh;
    char *value;
    value=(char *)malloc((size+1)*sizeof(char));
    if(value == NULL){
        printf("alocare esuata");
        exit(EXIT_FAILURE);
    }
    strncpy(value, pStartCh, size);
    value[size]='\0';

    return value;
}

void err(const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error: ");
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}

void tkerr(const Token *tk,const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d: ",tk->line);
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}

#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");
Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk,Token)
    tk->code=code;
    tk->line=line;
    tk->next=NULL;
    if(lastToken){
        lastToken->next=tk;
    }
        else{
            tokens=tk;
        }
    lastToken=tk;
    return tk;
}


int getNextToken()
{
    int state=0,nCh;
    char ch;
    const char *pStartCh;
    Token *tk;

    for(;;)
    { // infinite loop
        ch=*pCrtCh;
        switch(state)
        {
            case 0:
                if(isalpha(ch)||ch=='_')
                {
                    pStartCh=pCrtCh;
                    pCrtCh++;
                    state=27;
                }
                else if(ch=='+')
                {
                    pCrtCh++;
                    state=1;
                }
                else if(ch=='-')
                {
                    pCrtCh++;
                    state=2;
                }
                else if(ch=='*')
                {
                    pCrtCh++;
                    state=3;
                }
                else if(ch=='/')
                {
                    pCrtCh++;
                    state=4;
                }
                else if(ch=='.')
                {
                    pCrtCh++;
                    state=5;
                }
                else if(ch=='&')
                {
                    pCrtCh++;
                    state=6;
                }
                else if(ch=='|')
                {
                    pCrtCh++;
                    state=8;
                }
                else if(ch=='!')
                {
                    pCrtCh++;
                    state=10;
                }
                else if(ch=='=')
                {
                    pCrtCh++;
                    state=12;
                }
                else if(ch=='<')
                {
                    pCrtCh++;
                    state=14;
                }
                else if(ch=='>')
                {
                    pCrtCh++;
                    state=16;
                }
                else if(ch==',')
                {
                    pCrtCh++;
                    state=18;
                }
                else if(ch==';')
                {
                    pCrtCh++;
                    state=19;
                }
                else if(ch=='(')
                {
                    pCrtCh++;
                    state=20;
                }
                else if(ch==')')
                {
                    pCrtCh++;
                    state=21;
                }
                else if(ch=='[')
                {
                    pCrtCh++;
                    state=22;
                }
                else if(ch==']')
                {
                    pCrtCh++;
                    state=23;
                }
                else if(ch=='{')
                {
                    pCrtCh++;
                    state=24;
                }
                else if(ch=='}')
                {
                    pCrtCh++;
                    state=25;
                }
                else if(ch>='0'&&ch<='9')
                {
                    pStartCh=pCrtCh;
                    pCrtCh++;
                    state=29;
                }
                else if(ch=='\'')
                {
                    pCrtCh++;
                    state=37;
                }
                else if(ch=='"')
                {
                    pStartCh=pCrtCh;
                    pCrtCh++;
                    state=40;
                }
                else if(ch==' '||ch=='\t'||ch=='\r')
                {
                    pCrtCh++;
                }
                else if(ch=='\n')
                { // taken separately to analyze current line
                    line++;
                    pCrtCh++;
                }
                else if(ch=='\0')
                { // sfarsit de sir
                    addTk(END);
                    return END;
                }
                else tkerr(addTk(END),"invalid character");
                break;

            case 1:
                addTk(ADD);
                return ADD;
            case 2:
                addTk(SUB);
                return SUB;
            case 3:
                addTk(MUL);
                return MUL;
            case 4:
                if(ch=='/')
                {
                    pCrtCh++;
                    state=43;
                }
                else state=44;
                break;
            case 5:
                addTk(DOT);
                return DOT;
            case 6:
                if(ch=='&')
                {
                    pCrtCh++;
                    state=7;
                }
                else tkerr(addTk(END),"invalid character");
                break;
            case 7:
                addTk(AND);
                return AND;
            case 8:
                if(ch=='|')
                {
                    pCrtCh++;
                    state=9;
                }
                else tkerr(addTk(END),"invalid character");
                break;
            case 9:
                addTk(OR);
                return OR;
            case 10:
                if(ch=='=')
                {
                    pCrtCh++;
                    state=11;
                }
                else state=45; //to NOT
                break;
            case 11:
                addTk(NOTEQ);
                return NOTEQ;
            case 12:
                if(ch=='=')
                {
                    pCrtCh++;
                    state=13;
                }
                else state=46; //to ASSIGN
                break;
            case 13:
                addTk(EQUAL);
                return EQUAL;
            case 14:
                if(ch=='=')
                {
                    pCrtCh++;
                    state=15;
                }
                else state=47; //to LESS
                break;
            case 15:
                addTk(LESSEQ);
                return LESSEQ;
            case 16:
                if(ch=='=')
                {
                    pCrtCh++;
                    state=17;
                }
                else state=48; //to GREAT
                break;
            case 17:
                addTk(GREATEREQ);
                return GREATEREQ;
            case 18:
                addTk(COMMA);
                return COMMA;
            case 19:
                addTk(SEMICOLON);
                return SEMICOLON;
            case 20:
                addTk(LPAR);
                return LPAR;
            case 21:
                addTk(RPAR);
                return RPAR;
            case 22:
                addTk(LBRACKET);
                return LBRACKET;
            case 23:
                addTk(RBRACKET);
                return RBRACKET;
            case 24:
                addTk(LACC);
                return LACC;
            case 25:
                addTk(RACC);
                return RACC;
            case 27:
                if(isalnum(ch)||ch=='_')pCrtCh++;
                else state=28;
                break;
            case 28:
                nCh=pCrtCh-pStartCh; // word length
                // keywords
                if(nCh==5&&!memcmp(pStartCh,"break",5))tk=addTk(BREAK);
                else if(nCh==4&&!memcmp(pStartCh,"char",4))tk=addTk(CHAR);
                else if (nCh==6&&!memcmp(pStartCh,"double",6))tk=addTk(DOUBLE);
                else if (nCh==4&&!memcmp(pStartCh,"else",4))tk=addTk(ELSE);
                else if (nCh==3&&!memcmp(pStartCh,"for",3))tk=addTk(FOR);
                else if (nCh==2&&!memcmp(pStartCh,"if",2))tk=addTk(IF);
                else if (nCh==3&&!memcmp(pStartCh,"int",3))tk=addTk(INT);
                else if (nCh==6&&!memcmp(pStartCh,"return",6))tk=addTk(RETURN);
                else if (nCh==6&&!memcmp(pStartCh,"struct",6))tk=addTk(STRUCT);
                else if (nCh==4&&!memcmp(pStartCh,"void",4))tk=addTk(VOID);
                else if (nCh==5&&!memcmp(pStartCh,"while",5))tk=addTk(WHILE);

                else{ // if not keyword, then it's an ID
                    tk=addTk(ID);
                    tk->text=createString(pStartCh,pCrtCh);
                }
                return tk->code;
            case 29:
               if(ch>='0'&&ch<='9') pCrtCh++;
               else if (ch=='.'){ pCrtCh++; state=31; }
               else state=30;
               break;
            case 30:
                tk=addTk(CT_INT);
                tk->i=strtol(pStartCh, NULL, 10);
                return tk->code;
            case 31:
                if(ch>='0'&&ch<='9')
                {
                    pCrtCh++;
                    state=32;
                }

                else tkerr(addTk(END),"invalid character");
                break;
            case 32:
                if(ch>='0'&&ch<='9') pCrtCh++;
                else if (ch=='e'|| ch=='E') state=33;
                else state=36;
                break;
            case 33:
                if(ch=='+'||ch=='-')
                {
                    pCrtCh++;
                    state=34;
                }
                else
                {
                    pCrtCh++;
                    state=34;
                }
                break;
            case 34:
                if(ch>='0'&&ch<='9')
                {
                    pCrtCh++;
                    state=35;
                }

                else tkerr(addTk(END),"invalid character");
                break;
            case 35:
                if(ch>='0'&&ch<='9') pCrtCh++;
                else state=36;
                break;
            case 36:
                tk=addTk(CT_REAL);
                tk->r=atof(pStartCh);
                return tk->code;
            case 37:
                if(ch!='\'')
                {
                    pCrtCh++;
                    state=38;
                }
                else tkerr(addTk(END),"invalid character");
                break;
            case 38:
                if(ch=='\'')
                {
                    pCrtCh++;
                    state=39;
                }
                else tkerr(addTk(END),"invalid character");
                break;
            case 39:
                tk=addTk(CT_CHAR);
                tk->i=*(pCrtCh-2);
                return tk->code;
            case 40:
                if(ch!='"')
                {
                    pCrtCh++;
                    state=40;
                }
                else
                {
                    pCrtCh++;
                    state=41;
                }
                break;
            case 41:
                tk=addTk(CT_STRING);
                tk->text=createString(pStartCh+1,pCrtCh-1);
                return tk->code;
            case 43:
                if(!(ch == '\n' || ch == '\r' || ch == '\0')) {pCrtCh++;}
                else
                {
                    pCrtCh++;
                    state=0;
                }
                break;
            case 44:
                addTk(DIV);
                return DIV;
            case 45:
                addTk(NOT);
                return NOT;
            case 46:
                addTk(ASSIGN);
                return ASSIGN;
            case 47:
                addTk(LESS);
                return LESS;
            case 48:
                addTk(GREATER);
                return GREATER;
            default:
                printf("unknown state\n");
                break;
        }
    }
}

const char *tkCodeName(int code) {
    return a[code];
}

//Lexical Syntax
bool structDef();
bool fnDef();
bool varDef();
bool typeBase();
bool arrayDecl();
bool exp();
bool fnParam();
bool stmCompound();
bool stm();
bool expAssign();
bool expUnary();
bool expOr();
bool expOrPrim();
bool expAnd();
bool expAndPrim();
bool expEq();
bool expEqPrim();
bool expRel();
bool expRelPrim();
bool expAdd();
bool expAddPrim();
bool expMul();
bool expMulPrim();
bool expCast();
bool expPostfix();
bool expPostfixPrim();
bool expPrimary();

// Function to consume token
bool cons(int code){
    if(iTk->code==code)
    {
        consumedTk=iTk;
        iTk=iTk->next;
        printf("cons(%s",tkCodeName(code));
        if(code == ID || code == CT_STRING) {
            printf(", %s", consumedTk->text);
        }
        printf(")");
        printf(" => consumed\n");
        return true;
        }
       // printf(" => found %s\n",tkCodeName(iTk->code));
    return false;
}


// Function unit
bool unit(){
    for(;;){
        if(structDef()){}
        else if(fnDef()){}
        else if(varDef()){}
        else break;
    }
    if(cons(END)){
        return true;
    }
    return false;
}
// Function for structure definitions
bool structDef(){
    Token *start=iTk;
    if (cons(STRUCT)){
        if(cons(ID)){
            if(cons(LACC)){
                while(varDef()){}
                if(cons(RACC)){
                    if(cons(SEMICOLON)){
                        return true;
                    }
                }
            }
        }
    }
    iTk = start;
    return false;
}

// Function for variable definitions
bool varDef(){
    Token *start = iTk;
    if (typeBase()) {
        if (cons(ID)) {
            if(arrayDecl()){}
            while (cons(COMMA)) {
                if (!cons(ID)) {
                    iTk = start;
                    return false;
                }
                if(arrayDecl()){}
            }
            if (cons(SEMICOLON)) {
                return true;
            }
        }
    }

    iTk = start;
    return false;
}

// Function for base types
bool typeBase(){
    Token *start=iTk;
    if(cons(INT)){
        return true;
    }
    if(cons(DOUBLE)){
        return true;
    }
    if(cons(CHAR)){
        return true;
    }
    if(cons(STRUCT)){
        if(cons(ID)){
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for array declarations
bool arrayDecl(){
    Token *start=iTk;
    if(cons(LBRACKET)){
        if(exp()){}
        if(cons(RBRACKET)){
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for function definitions
bool fnDef(){
    Token *start=iTk;
    if(typeBase() || typeBase(cons(MUL))||cons(VOID)){
        if(cons(ID)){
            if(cons(LPAR)){
                if(fnParam()){
                    while(cons(COMMA)){
                        if(fnParam()){}
                    }
                }
                if(cons(RPAR)){
                    if(stmCompound()){
                        return true;
                    }
                }
            }
        }
    }
    iTk = start;
    return false;
}

// Function for function parameters
bool fnParam(){
    Token *start=iTk;
    if(typeBase()){
        if(cons(ID)){
            if(arrayDecl()){}
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for statements
bool stm(){
    Token *start=iTk;
    if(stmCompound()){
        return true;
    }
    iTk=start;
    if(cons(IF)){
        if(cons(LPAR)){
            if(exp()){
                if(cons(RPAR)){
                    if(stm()){
                        if(cons(ELSE)){
                            if(stm()){
                                return true;
                            }
                            //iTk = start;
                            //return false;
                        }
                        return true;
                    }
                }
            }
        }
    }
    iTk=start;
    if(cons(WHILE)){
        if(cons(LPAR)){
            if(exp()){
                if(cons(RPAR)){
                    if(stm()){
                        return true;
                    }
                }
            }
        }
    }
    iTk=start;
    if(cons(FOR)){
        if(cons(LPAR)){
            if(exp()){}
            if(cons(SEMICOLON)){
                if(exp()){}
                if(cons(SEMICOLON)){
                    if(exp()){}
                    if(cons(RPAR)){
                        if(stm()){
                            return true;
                        }
                    }
                }
            }
        }
    }
    iTk=start;
    if(cons(BREAK)){
        if(cons(SEMICOLON)){
            return true;
        }
    }
    iTk=start;
    if(cons(RETURN)){

        if(exp()){}

        if(cons(SEMICOLON)){
            return true;
        }
    }
    iTk=start;
    if(exp()){}
    if(cons(SEMICOLON)){
        return true;
    }
    iTk = start;
    return false;
}

// Function for compound statements
bool stmCompound(){
    Token *start=iTk;
    if(cons(LACC)){
        for(;;){
            if(varDef()){}
            else if(stm()){}
            else break;
        }
        if(cons(RACC)){
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for expessions
bool exp(){
    Token *start=iTk;
    if(expAssign()){
        return true;
    }
    iTk = start;
    return false;
}

// Function for function calls
bool expAssign(){
    Token *start=iTk;
    if(expUnary()){
        if(cons(ASSIGN)){
            if(expAssign()){
                return true;
            }
        }
    }
    iTk=start;
    if(expOr()){
        return true;
    }
    iTk = start;
    return false;
}


// Function for logical OR expessions
bool expOr(){
    Token *start=iTk;
    if(expAnd()){
        if(expOrPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for logical OR expessions continuation
bool expOrPrim(){
    Token *start=iTk;
    if(cons(OR)){
        if(expAnd()){
            if(expOrPrim()){
                return true;
            }
        }
    }
    iTk = start;
    return true;
}

// Function for logical AND expessions
bool expAnd(){
    Token *start=iTk;
    if(expEq()){
        if(expAndPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for logical AND expessions continuation
bool expAndPrim(){
    Token *start=iTk;
    if(cons(AND)){
        if(expEq()){
            if(expAndPrim()){
                return true;
            }
        }
    }
    iTk = start;
    return true;
}

// Function for equality expessions
bool expEq(){
    Token *start=iTk;
    if(expRel()){
        if(expEqPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for equality expessions continuation
bool expEqPrim(){
    Token *start=iTk;
    if(cons(EQUAL)||cons(NOTEQ)){
        if(expRel()){
            if(expEqPrim()){
                return true;
            }
        }
    }
    iTk = start;
    return true;
}

// Function for relational expessions
bool expRel(){
    Token *start=iTk;
    if(expAdd()){
        if(expRelPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for relational expessions continuation
bool expRelPrim(){
    Token *start=iTk;
    if(cons(LESS)||cons(LESSEQ)||cons(GREATER)||cons(GREATEREQ)){
        if(expAdd()){
            if(expRelPrim()){
                return true;
            }
        }
    }
    iTk = start;
    return true;
}

// Function for addition expessions
bool expAdd(){
    Token *start=iTk;
    if(expMul()){
        if(expAddPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for addition expessions continuation
bool expAddPrim(){
    Token *start=iTk;
    if(cons(ADD)||cons(SUB)){
        if(expMul()){
            if(expAddPrim()){
                return true;
            }
        }
    }
    iTk = start;
    return true;
}

// Function for multiplication expessions
bool expMul(){
   //printf("expMul\n");
    Token *start=iTk;
    if(expCast()){
        if(expMulPrim()){
            return true;
        }
    }
    iTk = start;
    return false;
}

// Function for multiplication expessions continuation
bool expMulPrim(){
    //    printf("expMulPrim\n");
    Token *start=iTk;
    if(cons(MUL)||cons(DIV)){
        if(expCast()){
            if(expMulPrim()){
                return true;
            }
        }
    }
    iTk = start;
    return true;
}

// Function for type casts
bool expCast(){
    Token *start=iTk;
    if(cons(LPAR)){
        if(typeBase()){
            if(arrayDecl()){}
            if(cons(RPAR)){
                if(expCast()){
                    return true;
                }
            }
        }
    }
    iTk=start;
    if(expUnary()){
        return true;
    }
    iTk = start;
    return false;
}

// Function for unary expessions
bool expUnary(){
    Token *start=iTk;
    if(cons(SUB)||cons(NOT)){
        if(expUnary()){
            return true;
        }
    }
    iTk=start;
    if(expPostfix()){
        return true;
    }
    iTk = start;
    return false;
}


// Function for postfix expessions
bool expPostfix(){
    //printf("expPostfix\n");
    Token *start=iTk;
    if(expPrimary()){
        if(expPostfixPrim()){
            return true;
        }
        if(expPostfix()){
            if(cons(DOT)){
                if(cons(ID)){
                    return true;
                }
            }
        }
    }
    iTk = start;
    return false;
}

// Function for postfix expessions continuation
bool expPostfixPrim(){
   // printf("expPostfixPrim\n");
    Token *start=iTk;
    if(cons(LBRACKET)){
        if(exp()){
            if(cons(RBRACKET)){
                if(expPostfixPrim()){
                    return true;
                }
            }
        }
    }

    iTk = start;
    return true;
}

// Function for primary expessions
bool expPrimary() {
    //printf("expPrimary\n");
    Token *start = iTk;

    if (cons(ID)) {
        if (cons(LPAR)) {
            while (expPrimary()) {
                if (!cons(COMMA)) {
                    break;
                }
            }
            if (cons(RPAR)) {
                if (expPostfixPrim()) {
                    return true;
                }
            }
            iTk = start;
            return false;
        }
        return true;
    }
    else if (cons(CT_INT) || cons(CT_REAL) || cons(CT_CHAR) || cons(CT_STRING)) {
        return true;
    }
    else if (cons(LPAR)) {
        if (exp()) {
            if (cons(RPAR)) {
                return true;
            }
        }
        iTk = start;
        return false;
    }
    iTk=start;
    return true;
}

void FreeMemory()
{
    Token *p;
    while(tokens!=NULL)
    {
        p=tokens;
        tokens=tokens->next;
        free(p);
    }
}


void showAtoms()
{
    Token *p;
    while(tokens!=NULL)
    {
        printf("%d\t%s",tokens->line, a[tokens->code]);
        if(tokens->code == ID || tokens->code == CT_STRING){ printf(":%s",tokens->text); }
        else if(tokens->code == CT_INT){ printf(":%d",tokens->i); }
        else if(tokens->code == CT_CHAR){ printf(":%c",tokens->i); }
        else if(tokens->code == CT_REAL){ printf(":%f",tokens->r); }
        printf("\n");
        p=tokens;
        tokens=tokens->next;
        free(p);
    }
}
int main() {
    FILE *fis;

    fis = fopen("5.txt", "rb");
    if (fis == NULL) {
        printf("Couldn't open file");
        return -1;
    }

    int n = fread(buffin, 1, MAX_SIZE - 1, fis);
    buffin[n] = '\n';
    fclose(fis);

    pCrtCh = buffin;

    while (getNextToken() != END) {

    }
    iTk = tokens;
    bool c3 = unit();
    printf("%d \n \n", c3);
    showAtoms();
    FreeMemory();

    return 0;
}
