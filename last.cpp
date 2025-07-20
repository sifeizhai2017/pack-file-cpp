/*包含头文件*/
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <malloc.h>
#include <ctime>
#include <direct.h>

using namespace std;
#define MAXSIZE 1000

/*文件个数的全局变量*/
int fileNum = 0;

/*使用到的物理数据类型*/
// 链表
typedef struct Node
{
    struct _finddata_t fileInfo;
    char filePath[MAXSIZE];
    struct Node *next;
} Node, *LinkFile;

// 文件类型的结构体
struct File
{
    //    文件名称
    char fileName[30];
    //    文件长度
    _fsize_t size;
    //	  文件路径
    char filePath[MAXSIZE];
    //	  文件偏移
    long fileOffset;
    //	  创建日期
    __time64_t time_create;
};

/*声明物理数据类型的操作（函数）和使用到的其他功能函数*/
void CreateFolder(char *folderName);

void UnpackFile(char *packPath, char *unpackPath);

void UnpackFileWithNumber(char *packPath, char *unpackPath, int index);

void InsList(Node **r, struct _finddata_t fileInfo, char *filePath);

LinkFile CreateLinkFile(char *filePath);

void PackFile(char *packPath, LinkFile lf);

int FileLength(char *fileName);

void ShowPack(char *packPath);

void ShowPackWithNumber(char *packPath, int showNumber);

/*定义物理数据类型的操作（函数）和使用到的其他功能函数*/
/**
 * 创建文件夹
 * @param folderName 文件夹名称
 */
void CreateFolder(char *folderName)
{
    // 文件夹不存在则创建文件夹
    if (_access(folderName, 0) == -1)
    {
        _mkdir(folderName);
    }
}

/**
 * 解包文件
 * @param packPath 包的路径
 * @param unpackPath 解包的路径
 */
void UnpackFile(char *packPath, char *unpackPath)
{
    //	FIXME: 要重新获取文件数目
    int i = 0, fileNumFromPack;
    //    包指针
    FILE *fp, *fq;
    //    文件类型结构体
    struct File tempFile;

    //     文件夹不存在则创建文件夹
    CreateFolder(unpackPath);
    //    打开包
    fp = fopen(packPath, "rb");
    if (!fp)
    {
        printf("包打开失败");
    }
    fread(&fileNumFromPack, sizeof(int), 1, fp);
    cout << fileNumFromPack << endl;
    while (i <= fileNumFromPack)
    {
        //        读出一段结构体
        fread(&tempFile, sizeof(File), 1, fp);
        //        拼接出解包的路径o
        strcpy(tempFile.filePath, unpackPath);
        strcat(tempFile.filePath, tempFile.fileName);
        //        在新路径新建文件
        fq = fopen(tempFile.filePath, "wb");
        //        缓冲区（本来想和打包一样一个一个字节读，但是发现好像做不到）
        char *buffer = (char *)malloc(tempFile.size);
        //        移到偏移处
        fseek(fp, tempFile.fileOffset, SEEK_SET);
        fread(buffer, tempFile.size, 1, fp);
        fwrite(buffer, tempFile.size, 1, fq);
        fclose(fq);
        //        移到下一个写进去的结构体处
        fseek(fp, sizeof(int) + i * sizeof(tempFile), SEEK_SET);
        ++i;
    }
}

/**
 * 解包特定文件
 * @param packPath 包的路径
 * @param unpackPath 解包的路径
 * @param index 索引
 */
void UnpackFileWithNumber(char *packPath, char *unpackPath, int index)
{
    int i = 0, fileNumFromPack;
    //    包指针
    FILE *fp, *fq;
    //    文件类型结构体
    struct File tempFile;

    //     文件夹不存在则创建文件夹
    CreateFolder(unpackPath);
    //    打开包
    fp = fopen(packPath, "rb");
    if (!fp)
    {
        printf("包打开失败");
    }
    fread(&fileNumFromPack, sizeof(int), 1, fp);
    while (i <= fileNumFromPack)
    {
        if (i == index)
        {
            //            读出一段结构体
            fread(&tempFile, sizeof(File), 1, fp);
            //            拼接出解包的路径
            strcpy(tempFile.filePath, unpackPath);
            strcat(tempFile.filePath, tempFile.fileName);
            //            在新路径新建文件
            fq = fopen(tempFile.filePath, "wb");
            //            缓冲区
            char *buffer = (char *)malloc(tempFile.size);
            //            移到偏移处
            fseek(fp, tempFile.fileOffset, SEEK_SET);
            fread(buffer, tempFile.size, 1, fp);
            fwrite(buffer, tempFile.size, 1, fq);
            fclose(fq);
            ++i;
        }
        else
        {
            //            移到下一个写进去的结构体处
            fseek(fp, sizeof(fileNum) + i * sizeof(tempFile), SEEK_SET);
            ++i;
        }
    }
}

/**
 * 将文件结构体插入到链表里面去
 * @param r 结点
 * @param fileInfo 结构体
 * @param filePath 文件路径
 */
void InsList(Node **r, struct _finddata_t fileInfo, char *filePath)
{
    Node *s;
    s = (Node *)malloc(sizeof(Node));
    if (!s)
    {
        printf("链表节点创建失败");
    }
    s->fileInfo = fileInfo;
    //	printf("%s\n", filePath);
    //	保存路径
    strcpy(s->filePath, filePath);
    //	保存文件名（带路径），先去除*.*
    *(strrchr(s->filePath, '\\') + 1) = '\0';
    strcat(s->filePath, s - L > fileInfo.name);
    (*r)->next = s;
    s->next = NULL;
    *r = s;
}

/**
 * 创建文件链表
 * @param filePath
 * @return 文件链表
 */
LinkFile CreateLinkFile(char *filePath)
{
    //	用于查找的句柄
    long handle;
    //	struct _finddata_t是用来存储文件各种信息的结构体
    struct _finddata_t fileInfo;
    LinkFile lf = (LinkFile)malloc(sizeof(Node));
    Node *r = lf;
    lf->next = NULL;

    if (!lf)
    {
        printf("创建失败");
    }
    //	第一次查找
    handle = _findfirst(filePath, &fileInfo);
    if (-1L == handle)
    {
        printf("文件路径出错");
    }
    else
    {
        //			第一次搜索
        if (strcmp(fileInfo.name, ".") && strcmp(fileInfo.name, ".."))
        {
            ++fileNum;
            InsList(&r, fileInfo, filePath);
        }
        while (!_findnext(handle, &fileInfo))
        {
            if (strcmp(fileInfo.name, ".") && strcmp(fileInfo.name, ".."))
            {
                ++fileNum;
                InsList(&r, fileInfo, filePath);
            }
        }
    }
    //	关闭句柄
    _findclose(handle);

    return lf;
}

/**
 * 打包文件
 */
void PackFile(char *packPath, LinkFile lf)
{
    //    fq是要打包的文件指针，fp是打包的文件指针
    FILE *fp, *fq;
    Node *r;
    char ch;
    r = lf->next;
    //    文件类型结构体
    struct File tempFile;
    tempFile.size = 0;
    //    偏移量 = 记录文件数目的fileNum的大小 + fileNum个结构体的大小
    tempFile.fileOffset = sizeof(fileNum) + fileNum * sizeof(File);

    fp = fopen(packPath, "wb+");
    if (!fp)
    {
        printf("文件打开失败！");
    }
    fwrite(&fileNum, sizeof(int), 1, fp);
    //    写入文件信息
    while (r != NULL)
    {
        //        tempFile.filePath = r->fileInfo.name;
        //        数组名是指针常量，显然不可以被赋值
        strcpy(tempFile.filePath, r->filePath);
        cout << tempFile.filePath << endl;
        tempFile.size = r->fileInfo.size;
        strcpy(tempFile.fileName, r->fileInfo.name);
        tempFile.time_create = r->fileInfo.time_create;
        fwrite(&tempFile, sizeof(tempFile), 1, fp);
        tempFile.fileOffset += r->fileInfo.size;
        r = r->next;
        printf("已写入%s信息\n", tempFile.fileName);
    }
    printf("=============================================\n");
    //    写文件
    r = lf->next;
    while (r != NULL)
    {
        fq = fopen(r->filePath, "rb");
        while (true)
        {
            fread(&ch, 1, 1, fq);
            //            如果结束就关闭文件
            if (feof(fq))
            {
                printf("已写入%s内容\n", r->fileInfo.name);
                fclose(fq);
                break;
            }
            //            往包里写
            fwrite(&ch, 1, 1, fp);
        }
        r = r->next;
    }
    fclose(fp);
}

/**
 * 获取文件的长度
 * @param fileName 文件路径
 * @return 返回字节长度
 */
int FileLength(char *fileName)
{
    FILE *fp = fopen(fileName, "r");
    if (!fp)
    {
        return -1;
    }
    //    移动一个文件的存取位置到文件的末尾
    fseek(fp, 0L, SEEK_END);
    //    利用ftell获得目前的文件访问位置
    int size = ftell(fp);
    fclose(fp);

    return size;
}

/**
 * 显示包内的文件信息（不带有限制一次显示数量参数）
 * @param packPath 包的路径
 */
void ShowPack(char *packPath)
{
    int fileNumFromPack;
    FILE *fp;
    struct File tempFile;
    fp = fopen(packPath, "rb");
    /*
        因为这里是通过命令行实现的，
            所以全局变量前面定义的全局变量在这里似乎不起作用
            所以需要重新在包中读取一次文件数目
    */
    fread(&fileNumFromPack, sizeof(int), 1, fp);
    //    跳到正式写文件结构体的部分
    fseek(fp, sizeof(fileNum), SEEK_SET);
    printf("文件名称\t\t文件大小\t创建时间\t\t\n");
    printf("===============================================================================\n");
    for (int i = 0; i < fileNumFromPack; ++i)
    {
        fread(&tempFile, sizeof(tempFile), 1, fp);
        printf("%s\t\t%lu\t\t%s", tempFile.fileName, tempFile.size, ctime(&tempFile.time_create));
    }
    printf("===============================================================================\n");
    printf("\t\t\t\t\t\t文件个数：%d\n", fileNumFromPack);
}

/**
 * 显示包内文件（限制一次显示数量）
 * @param packPath 包的路径
 * @param showNumber 一次显示数量
 */
void ShowPackWithNumber(char *packPath, int showNumber)
{
    int fileNumFromPack;
    FILE *fp;
    int index = 0;
    //    文件类型结构体
    struct File tempFile;
    fp = fopen(packPath, "rb");
    fread(&fileNumFromPack, sizeof(int), 1, fp);
    //    跳到正式写文件结构体的部分
    fseek(fp, sizeof(fileNum), SEEK_SET);
    printf("文件名称\t\t    文件大小\t\t创建时间\t\t\n");
    printf("===============================================================================\n");
    for (int i = 0; i < fileNumFromPack; ++i)
    {
        fread(&tempFile, sizeof(tempFile), 1, fp);
        printf("%s\t\t%lu\t\t%s", tempFile.fileName, tempFile.size, ctime(&tempFile.time_create));
        ++index;
        //        规定一次显示的文件个数
        if (index == showNumber)
        {
            getchar();
            index = 0;
        }
    }
    printf("===============================================================================\n");
    printf("\t\t\t\t\t\t文件个数：%d\n", fileNumFromPack);
}

/*定义主函数，通过主函数调用各物理数据类型的操作和功能函数*/
/**
 * 主函数
 * @param argc 参数个数
 * @param argv
 *      - 空     打包
 *          eg: pack D:\\tf\\*.* D:\\pack.dat
 *      - -l    显示包内文件
 *          eg: pack -l D:\\pack.dat
 *              pack -l3 D:\\pack.dat
 *      - -u    解包
 *          eg: pack -u D:\\pack.dat D:\\unpack\\
 *              pack -u2 D:\\pack.dat D:\\unpack\\
 * @return
 */
int main(int argc, char *argv[])
{
    char digits[MAXSIZE];
    int i, j, realDigits;

    //    对参数进行判断，进入不同的函数
    if (argv[1][0] != '-')
    {
        //        先读取文件信息
        printf("源文件目录为：%s", argv[1]);
        printf("包的目录为：%s\n", argv[2]);
        LinkFile lf = CreateLinkFile(argv[1]);
        PackFile(argv[2], lf);
    }
    else
    {
        //		如果参数是l，则显示包内的文件
        if (argv[1][1] == 'l')
        {
            if (argv[1][2] == '\0')
            {
                ShowPack(argv[2]);
            }
            else
            {
                //				TODO: 将字符串转化为数字
                //				没有Integer.parseInt()方法真的好不舒服，还要自己处理
                i = 0;
                j = 0;
                while (argv[1][i] != '\0')
                {
                    if (argv[1][i] >= '0' && argv[1][i] <= '9')
                    {
                        digits[j] = argv[1][i];
                        ++j;
                    }
                    ++i;
                }
                realDigits = atoi(digits);
                ShowPackWithNumber(argv[2], realDigits);
            }
        }
        else if (argv[1][1] == 'u')
        {
            if (argv[1][2] == '\0')
            {
                UnpackFile(argv[2], argv[3]);
            }
            else
            {
                i = 0;
                j = 0;
                while (argv[1][i] != '\0')
                {
                    if (argv[1][i] >= '0' && argv[1][i] <= '9')
                    {
                        digits[j] = argv[1][i];
                        ++j;
                    }
                    ++i;
                }
                realDigits = atoi(digits);
                UnpackFileWithNumber(argv[2], argv[3], realDigits);
            }
        }
        else
        {
            printf("参数输入错误，程序即将退出！\n");
            system("exit");
        }
    }

    return 0;
}
