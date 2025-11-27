#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#define OK 1
#define ERROR 0
#define MAXSIZE 10000
typedef int Status;

typedef struct{
    unsigned int weight;
    unsigned int parent, lchild, rchild;
}HTNode, *HuffmanTree;
typedef char** HuffmanCode;

void CreateHuffmanTree(HuffmanTree *HT,int n){
    if(n <= 1) return;
    int m = 2 * n - 1;
    
    for(int i = 1; i <= m; i++){        
        (*HT)[i].parent = 0;
        (*HT)[i].lchild = 0;
        (*HT)[i].rchild = 0;
    }
    for(int i = n + 1; i <= m; i++){
        int min1, min2;
        min1 = min2 = INT_MAX;
        int x1 = 0, x2 = 0;
        for(int j = 1; j < i; j++){
            if((*HT)[j].parent == 0){
                if((*HT)[j].weight < min1){
                    min2 = min1;
                    x2 = x1;
                    min1 = (*HT)[j].weight;
                    x1 = j;
                } else if((*HT)[j].weight < min2){
                    min2 = (*HT)[j].weight;
                    x2 = j;
                }
            }
        }
        (*HT)[x1].parent = i;
        (*HT)[x2].parent = i;
        (*HT)[i].lchild = x1;
        (*HT)[i].rchild = x2;
        (*HT)[i].weight = (*HT)[x1].weight + (*HT)[x2].weight;
    }

}

Status GetFrequency(char str[], char list[], int freq[], int len){
    int i, j, k;
    int found;
    int list_len = 0;
    
    // 从字符串中移除换行符
    for(i = 0; i < len; i++){
        if(str[i] == '\n' || str[i] == '\r'){  // 新增对 '\r' 的判断
            str[i] = '\0';
            len = i;
            break;
        }
    }   
    for(i = 0; i < len; i++){          
        found = 0;
        for(j = 0; j < list_len; j++){
            if(str[i] == list[j]){
                freq[j]++;
                found = 1;
                break;
            }
        }
        if(!found){
            list[list_len] = str[i];
            freq[list_len] = 1;
            list_len++;
        }
    }
    printf("\n字符频率统计:\n");
    printf("字符\t频率\n");
    for(k = 0; k < list_len; k++){
        printf("%c\t%d\n", list[k], freq[k]);
    }
    return list_len;
}
void HuffmanCoding(HuffmanTree HT, HuffmanCode *HC, int n){
    //从叶子到根逆向求每个字符的赫夫曼编码，存储在编码表HC中
    *HC=(HuffmanCode) malloc((n+1)*sizeof(char *));
    //分配n个字符编码的头指针向量，0号单元不用
    char*cd=(char*) malloc(n*sizeof(char));             //分配临时存放编码的动态数组空间
    cd[n-1]='\0';
    int i, start, c, f;      //编码结束符
    for(i=1; i<=n; ++i){       //逐个字符求赫夫曼编码
        start=n-1; c=i; f=HT[i].parent;
        while(f!=0){   //从叶子结点开始向上回溯，直到根结点
            --start;                               //回溯一次start向前指一个位置
            if (HT[f].lchild==c) cd[start]='0';   //结点c是f的左孩子，则生成代码0
            else cd[start]='1';                   //结点c是f的右孩子，则生成代码1
            c=f; f=HT[f].parent;                  //继续向上回溯
        }                                          
        (*HC)[i]=(char*) malloc((n-start)*sizeof(char));          
        strcpy((*HC)[i], &cd[start]);  //将求得的编码从临时空间cd复制到HC的当前行中
    }
    free(cd);                  //释放临时空间
} 

void Translation(HuffmanCode HC, char list[], char *str, char *codedStr, int n){
    // 将输入字符串翻译为Huffman编码的比特流
    int i, j, k;
    int index;
    int coded_len = 0;
    
    printf("\n字符编码表:\n");
    printf("字符\t编码\n");
    for(i = 1; i <= n; i++){
        if(HC[i] != NULL){
            printf("%c\t%s\n", list[i-1], HC[i]);
        }
    }
    
    printf("\n编码过程:\n");
    
    printf("编码: ");
    
    // 遍历输入字符串，逐个字符进行编码
    for(i = 0; str[i] !='\0'; i++){
        // 在字符列表中查找当前字符的索引
        index = -1;
        for(j = 0; j < n; j++){
            if(list[j] == str[i]){
                index = j + 1;  
                break;
            }
        }    
        // 如果找到了该字符，添加其编码到结果
        if(index != -1 && HC[index] != NULL){
            strcpy(&codedStr[coded_len], HC[index]);
            coded_len += strlen(HC[index]);
        }
    }
    codedStr[coded_len] ='\0';
    printf("%s\n", codedStr);
    printf("编码长度: %d 比特\n", coded_len);
}

void Decoding(HuffmanTree HT, char list[], char *encodedStr, int n){  // 将比特流解码为原文
    int i;
    int root = 2 * n - 1;  // 根节点索引
    int current = root;    // 当前节点
    char decodedStr[MAXSIZE];
    int decoded_len = 0;
    
    printf("\n解码过程:\n");
    
    printf("解码: ");
    
    
    for(i = 0; encodedStr[i] != '\0'; i++){
        if(encodedStr[i] == '0'){
            current = HT[current].lchild;  // 0走左子树
        } else if(encodedStr[i] == '1'){
            current = HT[current].rchild;  // 1走右子树
        }        
        // 到达叶子节点（无左右孩子）
        if(HT[current].lchild == 0 && HT[current].rchild == 0){
            
            decodedStr[decoded_len++] = list[current - 1];
            current = root;  // 回到根节点，准备解码下一个字符
        }
    }
    decodedStr[decoded_len] = '\0';
    printf("%s\n", decodedStr);
    printf("解码完成！原文长度: %d 字符\n", decoded_len);
}
int main(){
    char str[MAXSIZE];
    char list[500];
    int freq[500];
    memset(freq, 0, sizeof(freq));  // 初始化freq数组
    memset(list, 0, sizeof(list));  // 初始化list数组
    
    printf("输入电文字符串：\n");
    fgets(str, MAXSIZE, stdin);
    int len = strlen(str);
    int n = GetFrequency(str, list, freq, len);
    int m = 2 * n - 1;
  
    HuffmanTree HT;
    HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));
    // 将频率值赋给叶子结点
    for(int i = 1; i <= n; i++){
        HT[i].weight = freq[i - 1];
    }
    CreateHuffmanTree(&HT, n);
    printf("\nHuffman树已构建，共有%d个字符\n", n);
    HuffmanCode HC;
    HuffmanCoding(HT, &HC, n);
    
    // 进行编码翻译
    char codedStr[MAXSIZE * 10];  
    memset(codedStr, 0, sizeof(codedStr));
    Translation(HC, list, str, codedStr, n);
    
    printf("\n是否解码？（1-是，0-否）\n");
    int j;
    scanf("%d", &j);
    if(j) Decoding(HT, list, codedStr, n);  
    
    
    for(int i = 1; i <= n; i++){
        if(HC[i] != NULL) free(HC[i]);
    }
    if(HC) free(HC);
    if(HT) free(HT);
    
    return 0;
}