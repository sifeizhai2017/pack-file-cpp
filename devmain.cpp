#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <io.h>
#include <malloc.h>
#include <ctime>
#include <direct.h>

using namespace std;
const int MAXSIZE = 1000;

// 链表
typedef struct Node {
    _finddata_t fileInfo;
    char filePath[MAXSIZE];
    Node *next;
} Node, *LinkFile;

// 文件类型的结构体
struct File {
    // 文件名称
    char fileName[30];
    // 文件长度
    _fsize_t size;
    // 文件路径
    char filePath[MAXSIZE];
    // 文件偏移
    long fileOffset;
    // 创建日期
    __time64_t time_create;
};

/**
 * 与单个文件操作相关的类
 */
class FileMan {
private:
    char *fileBuf;
public:
//    将文件中的内容读取到缓冲区
    void readFile(const char *fn);

    char *getFileBuf() const;

//    File fileInfo(char *fn);
};

void FileMan::readFile(const char *fn) {
    FILE *fp = fopen(fn, "rb");
    if (fp == NULL) {
        cout << "文件打开失败" << endl;
    }

    // 将文件指针移动到文件末尾
    fseek(fp, 0, SEEK_END);
    // 求出当前文件指针距离文件开始的字节数
    long size = ftell(fp);
    // 将文件指针返回到文件开始处
    fseek(fp, 0, SEEK_SET);
    this->fileBuf = (char *) malloc(sizeof(char) * size);
    fileBuf[size] = '\0';
    fclose(fp);
}

char *FileMan::getFileBuf() const {
    return this->fileBuf;
}


/**
 * 与文件链表操作相关的类
 */
class FileList {
private:
    LinkFile lf;
    int fileNum;
public:
    explicit FileList(char *filePath);
//    将节点插入链表
    void insertNode(Node **r, struct _finddata_t fileInfo, char *filePath);
//    运算符重载，获取第no个文件
    char *operator[](int no);
//    返回文件链表
    const Node *getLf() const;
//    返回文件个数
    int getFileNum() const;
};

FileList::FileList(char *filePath) {
    this->fileNum = 0;
//    用于查找的句柄
    long handle;
//	struct _finddata_t是用来存储文件各种信息的结构体
    struct _finddata_t fileInfo{};
    lf = new Node;
    Node *r = lf;
    lf->next = NULL;

    if (!lf) {
        printf("创建失败");
    }
//	第一次查找
    handle = _findfirst(filePath, &fileInfo);
    if (-1L == handle) {
        printf("文件路径出错");
    } else {
//        第一次搜索
        if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0) {
            cout << fileInfo.name << endl;
            ++fileNum;
            insertNode(&r, fileInfo, filePath);
        }
        while (!_findnext(handle, &fileInfo)) {
            if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0) {
                cout << fileInfo.name << endl;
                ++fileNum;
                insertNode(&r, fileInfo, filePath);
            }
        }
    }
//	关闭句柄
    _findclose(handle);
}

void FileList::insertNode(Node **r, struct _finddata_t fileInfo, char *filePath) {
    Node *s = new Node;
//    cout << &s << endl;
    s->fileInfo = fileInfo;
//	printf("%s\n", filePath);
//	保存路径
    strcpy(s->filePath, filePath);
//	保存文件名（带路径），先去除*.*
    *(strrchr(s->filePath, '\\') + 1) = '\0';
    strcat(s->filePath, s->fileInfo.name);
    (*r)->next = s;
    s->next = NULL;
    *r = s;
}

const Node *FileList::getLf() const {
    return lf;
}

int FileList::getFileNum() const {
    return fileNum;
}

/**
 * 与文件打包相关的类
 */
class IndexMan {

};

int main() {
    FileMan *fm = new FileMan;
    fm->readFile("D:/t.txt");
    cout << fm->getFileBuf() << endl;

    FileList *fl = new FileList("D:\\tf\\*.*");
    cout << fl->getFileNum() << endl;
}
