#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_QUEUE_SIZE 100
#define MAX_CUSTOMERS 1000
#define PROCESS_TIME 10

// 客户结构
typedef struct {
    int customerIndex;
    int amount;
    int arriveTime;
    int processTime;
} Customer;

// 队列结构
typedef struct {
    Customer* data;
    int front;
    int rear;
    int size;
} Queue;

// 事件结构
typedef struct {
    int time;
    int customerIndex;
    int amount;
    int processTime;
} Event;

// 事件队列结构
typedef struct {
    Event* events;
    int count;
} EventQueue;

// 队列操作函数
Queue* initQueue(void) {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        printf("内存分配失败\n");
        exit(1);
    }
    q->data = (Customer*)malloc(sizeof(Customer) * MAX_QUEUE_SIZE);
    if (q->data == NULL) {
        printf("内存分配失败\n");
        free(q);
        exit(1);
    }
    q->front = q->rear = 0;
    q->size = 0;
    return q;
}

// 入队
int enQueue(Queue* q, Customer c) {
    if ((q->rear + 1) % MAX_QUEUE_SIZE == q->front) {
        return 0;  // 队列已满
    }
    q->data[q->rear] = c;
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->size++;
    return 1;
}

// 出队
int deQueue(Queue* q, Customer* c) {
    if (q->front == q->rear) {
        return 0;  // 队列为空
    }
    *c = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;
    return 1;
}

// 判断队列是否为空
int isEmpty(Queue* q) {
    return q->front == q->rear;
}

// 事件队列操作
void addEvent(EventQueue* eq, int time, int customerIndex, int amount, int processTime) {
    Event e = {time, customerIndex, amount, processTime};
    int i = eq->count - 1;
    
    // 按时间排序插入
    while (i >= 0 && eq->events[i].time > time) {
        eq->events[i + 1] = eq->events[i];
        i--;
    }
    eq->events[i + 1] = e;
    eq->count++;
}

// 银行模拟函数
void bankSimulation(int total, int closeTime, int N, int* transactions, int* arriveTimes) {
    Queue* depQueue = initQueue();  // 存款队列
    Queue* witQueue = initQueue();  // 取款队列
    
    // 初始化事件队列
    EventQueue eq = {0};
    eq.events = (Event*)malloc(sizeof(Event) * N);
    if (eq.events == NULL) {
        printf("内存分配失败\n");
        free(depQueue->data);
        free(depQueue);
        free(witQueue->data);
        free(witQueue);
        return;
    }
    
    int time = 0;           // 当前时间
    int balance = total;    // 银行余额
    int totalWait = 0;      // 总等待时间
    int processed = 0;      // 已处理客户数
    int* waits = (int*)calloc(N, sizeof(int));  // 每个客户的等待时间
    
    if (waits == NULL) {
        printf("内存分配失败\n");
        free(eq.events);
        free(depQueue->data);
        free(depQueue);
        free(witQueue->data);
        free(witQueue);
        return;
    }
    
    // 初始化事件队列
    for (int i = 0; i < N; i++) {
        addEvent(&eq, arriveTimes[i], i, transactions[i], PROCESS_TIME);
    }
    
    printf("\n开始模拟:\n");
    printf("------------------------\n");
    printf("初始余额: %d\n", balance);
    printf("------------------------\n");
        int eventProcessed = 0;
        
        // 处理当前时间的到达事件
        for(int j=0; j<eq.count; j++) {
            Event e = eq.events[j];
            Customer c = {e.customerIndex, e.amount, e.time, e.processTime};
            
            printf("时间 %d: 客户 %d 到达,\n", e.time, e.customerIndex + 1);
            if(time < e.time){
                time = e.time;
            }
            if(time+e.processTime>closeTime){
                printf("银行关闭，无法处理该客户业务。\n");
                break;
            }
            // 根据业务类型加入相应队列
            if (e.amount > 0) {
                
                enQueue(depQueue, c);
                balance += e.amount;
                deQueue(depQueue, &c);

                int waitTime = time - c.arriveTime;
                if (waitTime > 0) {
                    waits[c.customerIndex] = waitTime;
                    totalWait += waitTime;
                }
                
                processed++;
                
                printf("时间 %d: 处理客户 %d, 存款 %d, 等待 %d, 余额 %d\n", 
                       time, c.customerIndex + 1, c.amount, waitTime, balance);
                time += c.processTime;
                eventProcessed += 1;

               if (!isEmpty(witQueue)) {
                for(int k=0; k<witQueue->size; k++) {
                    deQueue(witQueue, &c);
                    if (balance >= -c.amount) {
                int waitTime = time - c.arriveTime;
                if (waitTime > 0) {
                    waits[c.customerIndex] = waitTime;
                    totalWait += waitTime;
                }                
                balance += c.amount;  // 取款时amount为负数
                
                processed++;
                printf("时间 %d: 处理客户 %d, 取款 %d, 等待 %d, 余额 %d\n", 
                       time, c.customerIndex + 1, -c.amount, waitTime, balance);
                time += c.processTime;
                eventProcessed += 1;
                }else enQueue(witQueue, c);
            }
        }
            }else if(e.amount+balance>=0){
                
                int waitTime = time - c.arriveTime;
                if (waitTime > 0) {
                    waits[c.customerIndex] = waitTime;
                    totalWait += waitTime;
                }                
                balance += c.amount;  
                time += c.processTime;
                processed++;
                printf("时间 %d: 处理客户 %d, 取款 %d, 等待 %d, 余额 %d\n", 
                       time, c.customerIndex + 1, -c.amount, waitTime, balance);
                eventProcessed += 1;
                              
            }else{
                enQueue(witQueue, c);
            }
        }
        time=closeTime;
        
        Customer c;

        if(deQueue(witQueue, &c)){
            
            
            waits[c.customerIndex]=time - c.arriveTime;
            totalWait+=waits[c.customerIndex];


        }
    // 打印统计信息
    printf("\n模拟结束:\n");
    printf("------------------------\n");
    printf("总处理客户数: %d\n", processed);
    printf("最终银行余额: %d\n", balance);
    
    printf("\n各客户等待时间:\n");
    for (int i = 0; i < N; i++) {
        printf("客户 %d: %d 分钟\n", i + 1, waits[i]);
    }
    
    if (processed > 0) {
        float avgWait = (float)totalWait / N;
        printf("\n平均等待时间: %.2f 分钟\n", avgWait);
    }
    
    
    free(eq.events);
    free(depQueue->data);
    free(depQueue);
    free(witQueue->data);
    free(witQueue);
    free(waits);
}


int main() {
    int N, total, closeTime;
    printf("请输入客户数量、初始总金额和银行关闭时间（以空格分隔）：\n");
    scanf("%d %d %d", &N, &total, &closeTime);
    
    int* transactions = (int*)malloc(N * sizeof(int));
    int* arriveTimes = (int*)malloc(N * sizeof(int));
    
    if (!transactions || !arriveTimes) {
        printf("内存分配失败\n");
        if (transactions) free(transactions);
        if (arriveTimes) free(arriveTimes);
        return 1;
    }
    
    // 读取每个客户的交易金额和到达时间
    for (int i = 0; i < N; i++) {
        printf("请输入第 %d 个客户的交易金额和到达时间（以空格分隔）：\n", i + 1);
        scanf("%d %d", &transactions[i], &arriveTimes[i]);
    }
    
    // 调用银行模拟函数
    bankSimulation(total, closeTime, N, transactions, arriveTimes);
    
    // 释放内存
    free(transactions);
    free(arriveTimes);
    return 0;
}