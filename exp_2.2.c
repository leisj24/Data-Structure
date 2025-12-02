#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define OK 1
#define ERROR 0
#define MAXSIZE 100000
#define OVERFLOW -2
typedef int Status;

typedef struct {
    char *base;
    char *top;
    int stacksize;
} SqStack;

Status InitStack(SqStack *S){
    S->base=(char *)malloc(MAXSIZE*sizeof(char));
    if(!S->base) exit(OVERFLOW);
    S->top=S->base;
    S->stacksize=MAXSIZE;
    return OK;
}

Status Push(SqStack *S,char e){
    if(S->top-S->base>=S->stacksize) return ERROR;
    *(S->top++)=e;
    return OK;
}

Status Pop(SqStack *S,char *e){
    if(S->top==S->base) return ERROR;
    *e=*--(S->top);
    return OK;
}

Status GetTop(SqStack S,char*e){
    if(S.top==S.base) return ERROR;
    *e=*(S.top-1);
    return OK;
}

Status Checkbracketmatching(SqStack *S){
    char e,ch;
    while(1){
        ch=getchar();
        if(ch=='@') break;
        if(ch=='(' || ch=='[' || ch=='{'){
            if(Push(S,ch)==ERROR) return ERROR;
        } else if(ch==')' || ch==']' || ch=='}'){
            if(GetTop(*S,&e)==ERROR) return ERROR;
            if((e!='(' && ch==')') || (e!='[' && ch==']') || (e!='{' && ch=='}')) return ERROR;
            else Pop(S,&e);
        }
    }
    if(S->top==S->base) return OK;
    else return ERROR;

}
                                                                                                                                                                      
int main(){
    
    SqStack S;
    InitStack(&S);
    char e;
    int i;
    while(i>0){
        printf("请输入字符串，以@结束:\n");
    
        if(Checkbracketmatching(&S)==OK)
            printf("YES!\n");
        else{
            printf("NO!\n");
            while(Pop(&S, &e) != ERROR); // 清空栈

        }
        getchar();  
        
        printf("是否继续检查括号匹配？(1-是, 0-否): ");
        scanf("%d", &i);
        getchar(); 

    }

    return 0;
}