#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define EPS 1e-7

typedef struct polinomial {
    float a;      // 用于存储系数，在头结点中存储项数
    int b;        // 用于存储指数
    struct polinomial* next;
} polinomial, *Po;
typedef int Status;
#define OK 1
#define ERROR 0
#define MY_OVERFLOW -1
#define NUM 100
Status LocateElem(polinomial* P,int e,Po* Q){
    Po cur = P->next;
    Po prev = P; 
    while(cur && cur->b < e){ prev = cur; cur = cur->next; }
    *Q = prev; 
    if(cur && cur->b == e) return OK;
    return ERROR;
}
int Inputpolinomial(polinomial* P,int k){
    printf("输入多项式%d(输入若干项，每项一行: 系数 指数, 以 0 0 结束)\n",k);
    P->next = NULL;
    P->a = 0;  // 初始化头结点中的项数为0
    float coef;
    int exp;
    while(1){
        if(scanf("%f %d", &coef, &exp) != 2) break;
        if(fabs(coef) < EPS && exp == 0) break; 
        Po prev;
        LocateElem(P, exp, &prev);
        Po cur = prev->next;
        if(cur && cur->b == exp){
            
            cur->a += coef;
            if(fabs(cur->a) < EPS){ 
                prev->next = cur->next;
                free(cur);
                P->a -= 1;  // 减少项数
            }
        }else{
            Po q = (Po)malloc(sizeof(polinomial));
            if(!q){ printf("内存分配失败\n"); return 0; }
            q->a = coef; q->b = exp; q->next = cur; prev->next = q;
            P->a += 1;  // 增加项数
        }
    }
    return OK;
}
Status Printpolist(polinomial** P,int i){
    for (int j = 0;j<=i;j++){
        Po p;
        p=P[j]->next;
        if(p==NULL){
            printf("多项式%d（项数：0）：0 0\n", j+1);
        }else{
            printf("多项式%d（项数：%.0f）：",j+1, P[j]->a);
            while(p){
                printf("%.2f %d ",p->a,p->b);
                p=p->next;
            }
            printf("\n");
        }
    }
    return OK;
}
Status Printpolimial(polinomial* P){
    Po p;
    p=P->next;
    if(p==NULL){
        printf("0 0\n");
        return OK;
    }
    printf("结果（项数：%.0f）：", P->a);
    while(p){
        printf("%.2f %d ",p->a,p->b);
        p=p->next;
    }
    printf("\n");
    return OK;
}
int calculate(){
    int choice;
    while(1){
        printf("请选择运算：1-加法，2-减法，3-微分，4-不定积分，5-除法，0-退出\n");
        int ret= scanf("%d",&choice);
        if (ret != 1) {
            printf("输入错误！请输入整数。\n");
            while (getchar() != '\n')
                continue;
        }
        if (choice < 0 || choice > 5) {
            printf("输入错误！请输入0-5之间的数。\n");
            continue;
        }
        break;
    }
    return choice;
}
int select(int i){
    int choice;
    while(1){
        printf("请选择需要多项式编号：\n");
        int ret= scanf("%d",&choice);
        if (ret != 1) {
            printf("输入错误！请输入整数。\n");
            while (getchar() != '\n')
                continue;
        }
        if (choice < 1 || choice > i+1) {
            printf("输入错误！请输入0-%d之间的数。\n",i);
            continue;
        }
        break;
    }
    return choice;
}
Status addpolinomial(polinomial* p1,polinomial* p2,polinomial* p3){
    Po p,q,r;
    p=p1->next;
    q=p2->next;
    r=p3;
    p3->a = 0;  // 初始化结果多项式的项数
    p3->next = NULL;
    
    while(p && q){
        Po new_node = (Po)malloc(sizeof(polinomial));
        if(!new_node) return ERROR;
        
        if(p->b < q->b){
            new_node->a = p->a;
            new_node->b = p->b;
            p = p->next;
        }
        else if(p->b > q->b){
            new_node->a = q->a;
            new_node->b = q->b;
            q = q->next;
        }
        else{  // 指数相等
            float sum = p->a + q->a;
            if(fabs(sum) > EPS){
                new_node->a = sum;
                new_node->b = p->b;
            }else{
                free(new_node);
                p = p->next;
                q = q->next;
                continue;
            }
            p = p->next;
            q = q->next;
        }
        new_node->next = NULL;
        r->next = new_node;
        r = new_node;
        p3->a += 1;  // 增加项数
    }
    
    // 处理剩余节点
    while(p){
        Po new_node = (Po)malloc(sizeof(polinomial));
        if(!new_node) return ERROR;
        new_node->a = p->a;
        new_node->b = p->b;
        new_node->next = NULL;
        r->next = new_node;
        r = new_node;
        p3->a += 1;
        p = p->next;
    }
    
    while(q){
        Po new_node = (Po)malloc(sizeof(polinomial));
        if(!new_node) return ERROR;
        new_node->a = q->a;
        new_node->b = q->b;
        new_node->next = NULL;
        r->next = new_node;
        r = new_node;
        p3->a += 1;
        q = q->next;
    }
    
    Printpolimial(p3);
    return OK;
}
Status substractpolinomial(polinomial* p1,polinomial* p2,polinomial* p3){
    Po p,q,r;
    p=p1->next;
    q=p2->next;
    r=p3;
    p3->a = 0;  // 初始化结果多项式的项数
    p3->next = NULL;
    
    while(p && q){
        Po new_node = (Po)malloc(sizeof(polinomial));
        if(!new_node) return ERROR;
        
        if(p->b < q->b){
            new_node->a = p->a;
            new_node->b = p->b;
            p = p->next;
        }
        else if(p->b > q->b){
            new_node->a = -q->a;  
            new_node->b = q->b;
            q = q->next;
        }
        else{  
            float diff = p->a - q->a;
            if(fabs(diff) > EPS){
                new_node->a = diff;
                new_node->b = p->b;
            }else{
                free(new_node);
                p = p->next;
                q = q->next;
                continue;
            }
            p = p->next;
            q = q->next;
        }
        new_node->next = NULL;
        r->next = new_node;
        r = new_node;
        p3->a += 1;  
    }
    
    // 处理剩余节点
    while(p){
        Po new_node = (Po)malloc(sizeof(polinomial));
        if(!new_node) return ERROR;
        new_node->a = p->a;
        new_node->b = p->b;
        new_node->next = NULL;
        r->next = new_node;
        r = new_node;
        p3->a += 1;
        p = p->next;
    }
    
    while(q){
        Po new_node = (Po)malloc(sizeof(polinomial));
        if(!new_node) return ERROR;
        new_node->a = -q->a;  
        new_node->b = q->b;
        new_node->next = NULL;
        r->next = new_node;
        r = new_node;
        p3->a += 1;
        q = q->next;
    }
    
    Printpolimial(p3);
    return OK;
}

Status differentiate(polinomial* p1,polinomial* p2){
    printf("输入求导阶数：");
    int n;
    scanf("%d",&n);
    if(n < 0){
        printf("求导阶数不能为负数！\n");
        return ERROR;
    }
    
    Po p = p1->next;
    
    if(!p2) return ERROR;
    p2->next = NULL;
    p2->a = 0;  
    Po q = p2;
    
    while(p){
        if(p->b < n){  
            p = p->next;
            continue;
        }
        
        
        float coef = p->a;
        int exp = p->b;
        for(int i = 0; i < n; i++){
            coef *= exp--;
        }
        
        if(fabs(coef) > EPS){  
            Po r = (Po)malloc(sizeof(polinomial));
            if(!r) return ERROR;
            
            r->a = coef;
            r->b = p->b - n;
            r->next = NULL;
            
            q->next = r;
            q = r;
            p2->a += 1;  
        }
        
        p = p->next;
    }
    Printpolimial(p2);
    return OK;
}
Status integral(polinomial* p1,polinomial* p2){
    Po p,q;
    p=p1->next;
    if(p2) {
        p2->next = NULL;
        p2->a = 0;  
    }
    q=p2;
    while(p){
        Po r=(Po)malloc(sizeof(polinomial));
        if(!r) return ERROR;
        r->a=p->a/(p->b+1);
        r->b=p->b+1;
        r->next=NULL;
        q->next=r;
        q=r;
        if(p2) p2->a += 1;  
        p=p->next;
    }
    
    Printpolimial(p2);
    return OK;
}

Po copy_poly(Po src){
    Po head = (Po)malloc(sizeof(polinomial));
    head->next = NULL;
    head->a = src->a;  // 复制项数信息
    Po cur = src->next;
    Po tail = head;
    while(cur){
        Po node = (Po)malloc(sizeof(polinomial));
        node->a = cur->a;
        node->b = cur->b;
        node->next = NULL;
        tail->next = node;
        tail = node;
        cur = cur->next;
    }
    return head;
}

void add_term(Po P, float coef, int exp){
    Po pre = P;
    Po cur = P->next;
    while(cur && cur->b < exp){ pre = cur; cur = cur->next; }
    if(cur && cur->b == exp){
        cur->a += coef;
        if(fabs(cur->a) < 1e-7){ 
            pre->next = cur->next;
            free(cur);
        }
    }else{
        Po node = (Po)malloc(sizeof(polinomial));
        node->a = coef;
        node->b = exp;
        node->next = cur;
        pre->next = node;
    }
}

Status getlead(Po P, float* a, int* b){
    Po cur = P->next;
    if(!cur) return ERROR;
    while(cur->next) cur = cur->next;
    *a = cur->a; *b = cur->b;
    return OK;
}

void subtractdivisor(Po rem, Po divisor, float coef, int shift){
    Po t = divisor->next;
    while(t){
        int target = t->b + shift;
        float delta = coef * t->a; 
        Po pre = rem;
        Po cur = rem->next;
        while(cur && cur->b < target){ 
            pre = cur; cur = cur->next; 
        }
        if(cur && cur->b == target){
            cur->a -= delta;
            if(fabs(cur->a) < 1e-7){ 
                pre->next = cur->next; free(cur); 
            }
        }else{
            Po node = (Po)malloc(sizeof(polinomial));
            node->a = -delta;
            node->b = target;
            node->next = cur;
            pre->next = node;
        }
        t = t->next;
    }
}
    void free_poly(Po P){
        if(!P) return;
        Po cur = P->next;
        while(cur){
            Po nxt = cur->next;
            free(cur);
            cur = nxt;
        }
        P->next = NULL;
    }

Status divide(polinomial* p1,polinomial* p2,polinomial* p3,polinomial* p4){
    if(p2==NULL || p2->next==NULL){
        printf("除数不能为0！\n");
        return ERROR;
    }

    if(p3) {
        p3->next = NULL;
        p3->a = 0;  // 初始化商的项数为0 
    }
    if(p4) {
        p4->next = NULL;
        p4->a = 0;  // 初始化余式的项数为0 
    }
    Po rem = copy_poly(p1);
    if(!rem) return ERROR;
    float a1; int b1;
    float a2; int b2;

    while(getlead(rem, &a1, &b1) == OK && getlead(p2, &a2, &b2) == OK && b1 >= b2){
        float coef = a1 / a2;
        int exp = b1 - b2;
        add_term(p3, coef, exp);
        if(p3) p3->a += 1;  
        subtractdivisor(rem, p2, coef, exp);
    }

    // 计算余式的项数 
    Po cur = rem->next;
    while(cur){
        if(p4) p4->a += 1;
        cur = cur->next;
    }
    
    p4->next = rem->next;
    free(rem);
    Printpolimial(p3);
    printf("余数");
    Printpolimial(p4);
    return OK;
}
Status revisepoly(polinomial** Polist){
    int i, j;
    Po q;
    j = select(i);
    q=Polist[j-1];
    printf("请输入修改后节点的系数和指数：\n");
    float a;
    int b;
    scanf("%f %d", &a, &b);

    Po prev;
    if (!(LocateElem(Polist[j - 1], b, &prev)))
    {
        Po p = (Po)malloc(sizeof(polinomial));
        p->a = a;
        p->b = b;
        p->next = prev->next;
        prev->next = p;
        q->a += 1;
    }else{
            Po node = prev->next; 
            if(fabs(a)<1e-7){
                
                prev->next = node->next;
                free(node);
                q->a -= 1;
            }else{
                node->a = a;
            }
        }
        
        printf("修改后的多项式为：\n");
        Printpolimial(Polist[j-1]);
    return OK;
}
int main(){
    Po Polist[NUM] ;
    int choice1,choice2,n,i,j,k,m;
    for(int ti=0; ti<NUM; ti++) Polist[ti]=NULL;
    for(i=0;;i++){
        Polist[i]=(Po)malloc(sizeof(polinomial));
        if(!Polist[i]){ printf("内存分配失败\n"); return 0; }

        Inputpolinomial(Polist[i],i+1);
        while(1){        
            printf("是否继续输入多项式？1-是，0-否\n");
            scanf("%d",&n);
            if((n!=0&&n!=1)){
                printf("输入错误！请输入0或1。\n");
                continue;
            }
            break;
        }
        if(n==0){
            printf("多项式输入结束！");
            break;
        }else continue;
    }

    printf("输入的多项式如下：\n");
    Printpolist(Polist,i);
    while(1){
    while(1){
        printf("请选择操作：1-进行运算，2-修改多项式，3-清除多项式，4-销毁多项式，0-结束\n");
        int ret= scanf("%d",&choice1);
        if (ret != 1) {
            printf("输入错误！请输入整数。\n");
            while (getchar() != '\n')
                continue;
        }
        if (choice1 < 0 || choice1 > 4) {
            printf("输入错误！请输入0-4之间的数。\n");
            continue;
        }
        break;
    }

    switch(choice1){
        case 1:
        for(int l=i+1;;l++){
        int ret1,ret2;
        Po p1,p2;
        
        if(l < NUM){
            if(Polist[l]==NULL){ Polist[l]=(Po)malloc(sizeof(polinomial)); if(!Polist[l]){ printf("内存分配失败\n"); break; } Polist[l]->next = NULL; }
            if(Polist[l+1]==NULL){ Polist[l+1]=(Po)malloc(sizeof(polinomial)); if(!Polist[l+1]){ printf("内存分配失败\n"); break; } Polist[l+1]->next = NULL; }
        } else {
            printf("多项式存储已达上限\n"); break;
        }
        choice2=calculate();
        switch(choice2){
            case 1:
            printf("请输入多项式编号：\n");
            ret1= scanf("%d",&j);
            ret2= scanf("%d",&k);
            if (ret1 != 1 || ret2 != 1) {
                printf("输入错误！请输入整数。\n");
                while (getchar() != '\n')
                    continue;
            }
            if (j < 1 || j > i+1 || k < 1 || k > i+1) {
                printf("输入错误！请输入0-%d之间的数。\n",i);
                continue;
            }
            addpolinomial(Polist[j-1],Polist[k-1],Polist[l]);
            break;

            case 2:
            printf("请输入多项式编号：\n");
            ret1= scanf("%d",&j);
            ret2= scanf("%d",&k);
            if (ret1 != 1 || ret2 != 1) {
                printf("输入错误！请输入整数。\n");
                while (getchar() != '\n')
                    continue;
            }
            if (j < 1 || j > i+1 || k < 1 || k > i+1) {
                printf("输入错误！请输入0-%d之间的数。\n",i);
                continue;
            }
            substractpolinomial(Polist[j-1],Polist[k-1],Polist[l]);
            break;

            case 3:
            j=select(i);
            differentiate(Polist[j-1],Polist[l]);break;

            case 4:
            j=select(i);
            integral(Polist[j-1],Polist[l]);           
            break;

            case 5:
            printf("请输入多项式编号：\n");
            ret1= scanf("%d",&j);
            ret2= scanf("%d",&k);
            if (ret1 != 1 || ret2 != 1) {
                printf("输入错误！请输入整数。\n");
                while (getchar() != '\n')
                    continue;
            }
            if (j < 1 || j > i+1 || k < 1 || k > i+1) {
                printf("输入错误！请输入0-%d之间的数。\n",i);
                continue;
            }            
            divide(Polist[j-1],Polist[k-1],Polist[l],Polist[l+1]);
            break;

            case 0:printf("退出\n");break;       
            default:printf("输入错误！请输入0-5之间的数。\n");
        }    
        break;
        }
        break;

        case 2:
        revisepoly(Polist);
        break;

        case 3:
        j=select(i);
        free_poly(Polist[j-1]);
        printf("清除完毕。\n");    
        break;

        case 4:
        j=select(i);     
        free_poly(Polist[j-1]);
        free(Polist[j-1]);
        for(m=j-1;m<i;m++)
            Polist[m]=Polist[m+1];
        i--;
        printf("销毁完成。\n");        
        break;

        case 0:
            // 销毁所有多项式
            printf("正在清理所有多项式...\n");
            for(int k = 0; k <= i; k++) {
                if(Polist[k] != NULL) {
                    free_poly(Polist[k]);    
                    free(Polist[k]);         
                    Polist[k] = NULL;
                }
            }
            printf("所有多项式已销毁\n");
            printf("结束\n");
            return 0;

        default:printf("输入错误！请输入0-4之间的数。\n");
    
        break;
    }
}

    return 0;
}
