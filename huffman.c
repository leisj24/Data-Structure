#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<sys/stat.h>

#define OK 1
#define ERROR 0
#define MAXSIZE 100000
#define HUFFMAN_MAGIC 0x48554646  // "HUFF"的十六进制表示

typedef struct {
    unsigned int weight;
    unsigned int parent, lchild, rchild;
} HTNode, *HuffmanTree;
typedef char** HuffmanCode;

void CreateHuffmanTree(HuffmanTree *HT, int n) {
    if (n <= 1) return;
    int m = 2 * n - 1;
    
    *HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));
    if (!*HT) return;    
    // 初始化所有节点
    for (int i = 1; i <= m; i++) {
        (*HT)[i].weight = 0;
        (*HT)[i].parent = 0;
        (*HT)[i].lchild = 0;
        (*HT)[i].rchild = 0;
    }    
    for (int i = n + 1; i <= m; i++) {
        unsigned int min1 = UINT_MAX, min2 = UINT_MAX;
        int x1 = 0, x2 = 0;
        
        for (int j = 1; j < i; j++) {
            if ((*HT)[j].parent == 0) {
                if ((*HT)[j].weight < min1) {
                    min2 = min1;
                    x2 = x1;
                    min1 = (*HT)[j].weight;
                    x1 = j;
                } else if ((*HT)[j].weight < min2) {
                    min2 = (*HT)[j].weight;
                    x2 = j;
                }
            }
        }
        
        if (x1 == 0 || x2 == 0) break;        
        (*HT)[x1].parent = i;
        (*HT)[x2].parent = i;
        (*HT)[i].lchild = x1;
        (*HT)[i].rchild = x2;
        (*HT)[i].weight = (*HT)[x1].weight + (*HT)[x2].weight;
    }
}
void HuffmanCoding(HuffmanTree HT, HuffmanCode *HC, int n) {
    if (n <= 0) return;
    
    *HC = (HuffmanCode)malloc((n + 1) * sizeof(char*));
    if (!*HC) return;
    
    char *cd = (char*)malloc(n * sizeof(char));
    if (!cd) return;
    
    cd[n - 1] = '\0';
    
    for (int i = 1; i <= n; ++i) {
        int start = n - 1;
        int c = i;
        int f = HT[i].parent;
        
        while (f != 0) {
            --start;
            if (HT[f].lchild == c) 
                cd[start] = '0';
            else 
                cd[start] = '1';
            c = f;
            f = HT[f].parent;
        }
        
        (*HC)[i] = (char*)malloc((n - start) * sizeof(char));
        if ((*HC)[i]) {
            strcpy((*HC)[i], &cd[start]);
        }
    }
    free(cd);
}

// 统计文件中所有字节的频率
int GetByteFrequency(FILE *fp, unsigned int freq[256]) {
    unsigned char byte;
    int unique_bytes = 0;
    
    memset(freq, 0, sizeof(unsigned int) * 256);
    
    while (fread(&byte, 1, 1, fp) == 1) {
        if (freq[byte] == 0) {
            unique_bytes++;
        }
        freq[byte]++;
    }    
    rewind(fp);
    return unique_bytes;
}
// 计算文件总字节数
unsigned long GetFileSize(unsigned int freq[256]) {
    unsigned long total = 0;
    for (int i = 0; i < 256; i++) {
        total += freq[i];
    }
    return total;
}
// 写入文件头
void WriteFileHeader(FILE *outfp, int n, unsigned long fileSize, 
                    unsigned char byteList[], unsigned int freqList[]) {
    // 写入魔数
    unsigned int magic = HUFFMAN_MAGIC;
    fwrite(&magic, sizeof(unsigned int), 1, outfp);
    
    // 写入不同字节数
    unsigned short uniqueByteCount = (unsigned short)n;
    fwrite(&uniqueByteCount, sizeof(unsigned short), 1, outfp);
    
    // 写入文件总大小
    fwrite(&fileSize, sizeof(unsigned long), 1, outfp);
    
    // 写入字节列表和频率列表
    fwrite(byteList, 1, n, outfp);
    fwrite(freqList, sizeof(unsigned int), n, outfp);
}
// 读取文件头
int ReadFileHeader(FILE *infp, int *n, unsigned long *fileSize, 
                  unsigned char byteList[], unsigned int freqList[]) {
    // 读取魔数
    unsigned int magic;
    if (fread(&magic, sizeof(unsigned int), 1, infp) != 1) {
        return 0;
    }    
    if (magic != HUFFMAN_MAGIC) {
        printf("错误：无效的Huffman文件格式\n");
        return 0;
    }
    
    // 读取不同字节数
    unsigned short uniqueByteCount;
    if (fread(&uniqueByteCount, sizeof(unsigned short), 1, infp) != 1) {
        return 0;
    }
    *n = uniqueByteCount;    
    // 读取文件总大小
    if (fread(fileSize, sizeof(unsigned long), 1, infp) != 1) {
        return 0;
    }    
    // 读取字节列表和频率列表
    if (fread(byteList, 1, *n, infp) != *n) {
        return 0;
    }
    
    if (fread(freqList, sizeof(unsigned int), *n, infp) != *n) {
        return 0;
    }
    
    return 1;
}
// 将二进制字符串写入文件
void WriteBitStream(FILE *outfp, const char *bitStream, int *bitBuffer, int *bitCount) {
    int len = strlen(bitStream);
    
    for (int i = 0; i < len; i++) {
        *bitBuffer = (*bitBuffer << 1) | (bitStream[i] == '1' ? 1 : 0);
        (*bitCount)++;
        
        if (*bitCount == 8) {
            unsigned char byte = (unsigned char)*bitBuffer;
            fwrite(&byte, 1, 1, outfp);
            *bitBuffer = 0;
            *bitCount = 0;
        }
    }
}
// 写入剩余的比特
void FlushBitStream(FILE *outfp, int bitBuffer, int bitCount) {
    if (bitCount > 0) {
        bitBuffer <<= (8 - bitCount);
        unsigned char byte = (unsigned char)bitBuffer;
        fwrite(&byte, 1, 1, outfp);
        
        // 写入填充信息（最后一个字节的有效位数）
        fwrite(&bitCount, sizeof(unsigned char), 1, outfp);
    } else {
        // 如果没有剩余位，写入8表示最后一个字节是完整的
        unsigned char bitCount = 8;
        fwrite(&bitCount, sizeof(unsigned char), 1, outfp);
    }
}
void CompressFile(const char *inputFile, const char *outputFile) {
    FILE *infp = fopen(inputFile, "rb");
    if (!infp) {
        printf("错误：无法打开输入文件 %s\n", inputFile);
        return;
    }
    
    FILE *outfp = fopen(outputFile, "wb");
    if (!outfp) {
        printf("错误：无法创建输出文件 %s\n", outputFile);
        fclose(infp);
        return;
    }
    
    // 统计字节频率
    unsigned int freq[256];
    int n = GetByteFrequency(infp, freq);
    
    if (n == 0) {
        printf("文件为空\n");
        fclose(infp);
        fclose(outfp);
        return;
    }
    
    printf("不同字节数: %d\n", n);
    
    // 获取文件总大小
    unsigned long fileSize = GetFileSize(freq);
    printf("文件总大小: %lu 字节\n", fileSize);
    
    // 创建Huffman树
    HuffmanTree HT = NULL;
    int index = 1;
    
    int m = 2 * n - 1;
    HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));
    if (!HT) {
        printf("内存分配失败\n");
        fclose(infp);
        fclose(outfp);
        return;
    }    
    // 初始化所有节点
    for (int i = 1; i <= m; i++) {
        HT[i].weight = 0;
        HT[i].parent = 0;
        HT[i].lchild = 0;
        HT[i].rchild = 0;
    }
    
    // 设置叶子节点的权重
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            HT[index].weight = freq[i];
            index++;
        }
    }
    
    CreateHuffmanTree(&HT, n);    
    // 生成Huffman编码
    HuffmanCode HC = NULL;
    HuffmanCoding(HT, &HC, n);
    
    // 准备字节列表和频率列表
    unsigned char byteList[256];
    unsigned int freqList[256];
    int byteIndex = 0;
    
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            byteList[byteIndex] = (unsigned char)i;
            freqList[byteIndex] = freq[i];
            byteIndex++;
        }
    }
    
    // 写入文件头
    WriteFileHeader(outfp, n, fileSize, byteList, freqList);
    
    // 读取文件并进行编码
    unsigned char byte;
    int bitBuffer = 0;
    int bitCount = 0;
    
    // 构建字节到编码索引的映射
    int byteToIndex[256];
    byteIndex = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            byteToIndex[i] = byteIndex + 1; // Huffman编码索引从1开始
            byteIndex++;
        } else {
            byteToIndex[i] = -1;
        }
    }
    
    while (fread(&byte, 1, 1, infp) == 1) {
        int codeIndex = byteToIndex[byte];
        if (codeIndex != -1 && HC[codeIndex] != NULL) {
            WriteBitStream(outfp, HC[codeIndex], &bitBuffer, &bitCount);
        }
    }
    
    // 写入剩余的比特和填充信息
    FlushBitStream(outfp, bitBuffer, bitCount);
    
    printf("压缩完成\n");
    long compressedSize = ftell(outfp);
    double compressionRatio = (1.0 - (double)compressedSize / fileSize) * 100;
    //printf("压缩率: %.2f%%\n", compressionRatio);
    
    // 释放资源
    if (HC) {
        for (int i = 1; i <= n; i++) {
            if (HC[i]) free(HC[i]);
        }
        free(HC);
    }
    if (HT) free(HT);
    
    fclose(infp);
    fclose(outfp);
}

// 从文件读取一个比特
int ReadBit(FILE *infp, int *bitBuffer, int *bitCount, int *currentByte, int *bitsRead) {
    if (*bitCount == 0) {
        if (fread(currentByte, 1, 1, infp) != 1) {
            return -1;
        }
        *bitBuffer = *currentByte;
        *bitCount = 8;
    }
    
    int bit = (*bitBuffer >> 7) & 1;
    *bitBuffer <<= 1;
    (*bitCount)--;
    (*bitsRead)++;
    
    return bit;
}

// 解压缩文件函数
void DecompressFile(const char *inputFile, const char *outputFile) {
    FILE *infp = fopen(inputFile, "rb");
    if (!infp) {
        printf("错误：无法打开压缩文件 %s\n", inputFile);
        return;
    }
    
    FILE *outfp = fopen(outputFile, "wb");
    if (!outfp) {
        printf("错误：无法创建输出文件 %s\n", outputFile);
        fclose(infp);
        return;
    }
    
    // 读取压缩文件头部信息
    int n;
    unsigned long fileSize;
    unsigned char byteList[256];
    unsigned int freqList[256];
    
    if (!ReadFileHeader(infp, &n, &fileSize, byteList, freqList)) {
        printf("错误：无法读取文件头或文件格式不正确\n");
        fclose(infp);
        fclose(outfp);
        return;
    }
    
    if (n <= 0 || n > 256) {
        printf("错误：无效的字节数 %d\n", n);
        fclose(infp);
        fclose(outfp);
        return;
    }
    
    printf("不同字节数: %d\n", n);
    printf("期望解压大小: %lu 字节\n", fileSize);
    
    // 重建Huffman树
    HuffmanTree HT = NULL;
    int m = 2 * n - 1;
    HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));
    if (!HT) {
        printf("内存分配失败\n");
        fclose(infp);
        fclose(outfp);
        return;
    }
    
    // 初始化所有节点
    for (int i = 1; i <= m; i++) {
        HT[i].weight = 0;
        HT[i].parent = 0;
        HT[i].lchild = 0;
        HT[i].rchild = 0;
    }
    
    // 设置叶子节点的权重
    for (int i = 0; i < n; i++) {
        HT[i + 1].weight = freqList[i];
    }
    
    CreateHuffmanTree(&HT, n);
    
    // 读取比特流并解码
    int bitBuffer = 0;
    int bitCount = 0;
    int currentByte = 0;
    int root = 2 * n - 1;
    int currentNode = root;
    unsigned long decodedCount = 0;
    
    // 获取文件大小以确定数据结束位置
    fseek(infp, 0, SEEK_END);
    long fileEnd = ftell(infp);
    fseek(infp, -(long)sizeof(unsigned char), SEEK_END); // 定位到填充信息
    unsigned char lastByteBitCount;
    fread(&lastByteBitCount, sizeof(unsigned char), 1, infp);
    
    // 重新定位到数据开始位置
    long dataStart = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(unsigned long) + 
                     n * (1 + sizeof(unsigned int));
    fseek(infp, dataStart, SEEK_SET);
    
    int bitsRead = 0;
    int totalBits = (fileEnd - dataStart - 1) * 8; // 总比特数（减去填充信息字节）
    
    while (decodedCount < fileSize) {
        // 如果是最后一个字节，只读取有效位数
        if (ftell(infp) == fileEnd - 1 && bitsRead >= totalBits - (8 - lastByteBitCount)) {
            break;
        }
        
        int bit = ReadBit(infp, &bitBuffer, &bitCount, &currentByte, &bitsRead);
        if (bit == -1) {
            break;
        }
        
        if (bit == 0) {
            currentNode = HT[currentNode].lchild;
        } else {
            currentNode = HT[currentNode].rchild;
        }
        
        // 安全检查
        if (currentNode < 1 || currentNode > m) {
            printf("错误：解码过程中遇到无效节点 %d\n", currentNode);
            break;
        }        
        // 到达叶子节点
        if (HT[currentNode].lchild == 0 && HT[currentNode].rchild == 0) {
            if (currentNode >= 1 && currentNode <= n) {
                unsigned char byte = byteList[currentNode - 1];
                fwrite(&byte, 1, 1, outfp);
                decodedCount++;
                currentNode = root;
                
                if (decodedCount == fileSize) {
                    break;
                }
            } else {
                printf("错误：叶子节点索引超出范围 %d\n", currentNode);
                break;
            }
        }
    }
    
    printf("解压完成，解压字节数: %lu\n", decodedCount);
    
    
    // 释放资源
    if (HT) free(HT);    
    fclose(infp);
    fclose(outfp);
}

int main() {
    int choice;
    char inputFile[256];
    char outputFile[256];    
    while (1) {
        printf("\n========== Huffman文件压缩程序 ==========\n");
        printf("1. 压缩文件\n");
        printf("2. 解压缩文件\n");
        printf("3. 退出\n");
        printf("请选择操作 (1-3): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("输入错误，请重新输入\n");
            while (getchar() != '\n');
            continue;
        }
        getchar();
        
        switch (choice) {
            case 1:
                printf("输入要压缩的文件名: ");
                if (fgets(inputFile, sizeof(inputFile), stdin) == NULL) {
                    printf("读取文件名失败\n");
                    break;
                }
                inputFile[strcspn(inputFile, "\n")] = 0;
                
                FILE *test = fopen(inputFile, "rb");
                if (!test) {
                    printf("错误：文件 %s 不存在或无法打开\n", inputFile);
                    break;
                }
                fclose(test);
                
                snprintf(outputFile, sizeof(outputFile), "%s.huff", inputFile);
                
                printf("输入文件: %s\n", inputFile);
                printf("输出文件: %s\n", outputFile);
                printf("开始压缩...\n");
                CompressFile(inputFile, outputFile);
                break;
                
            case 2:
                printf("输入要解压的.huff文件名: ");
                if (fgets(inputFile, sizeof(inputFile), stdin) == NULL) {
                    printf("读取文件名失败\n");
                    break;
                }
                inputFile[strcspn(inputFile, "\n")] = 0;
                
                test = fopen(inputFile, "rb");
                if (!test) {
                    printf("错误：文件 %s 不存在或无法打开\n", inputFile);
                    break;
                }
                fclose(test);
                
                strcpy(outputFile, inputFile);
                char *dot = strrchr(outputFile, '.');
                if (dot && strcmp(dot, ".huff") == 0) {
                    *dot = '\0';
                } else {
                    strcat(outputFile, ".extracted");
                }
                
                printf("输入文件: %s\n", inputFile);
                printf("输出文件: %s\n", outputFile);
                printf("开始解压...\n");
                DecompressFile(inputFile, outputFile);
                break;
                
            case 3:
                printf("程序退出\n");
                return 0;
                
            default:
                printf("错误：请输入有效的选项 (1-3)\n");
        }
    }
    
    return 0;
}