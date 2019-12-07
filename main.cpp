#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <io.h>
#include <malloc.h>
#include <ctime>
#include <typeinfo>
#include <direct.h>
#include <fstream>
#include <tchar.h>

using namespace std;

// 链表
typedef struct Node {
    _finddata_t fileInfo;
    char filePath[1000];
    Node *next;
} Node, *LinkFile;

// 文件类型的结构体
struct File {
    // 文件名称
    char fileName[30];
    // 文件长度
    _fsize_t size;
    // 文件路径
    char filePath[1000];
    // 文件偏移
    int fileOffset;
    // 创建日期
    __time64_t time_create;
};


/**
 * 与单个文件操作相关的类
 */
class FileManager {
private:
    char *fileBuf;
    long size;
public:
    // 将文件中的内容读取到缓冲区
    void readFile(char *fn);

    // 将文件中的内容直接写入包中
    void writeSingleFile(Node *r, FILE *packPointer);

    char *getFileBuf() const;
};

void FileManager::readFile(char *fn) {
    cout << "当前正在写的文件：" << fn << endl;
    filebuf *pbuf;
    ifstream filestr;
    // 要读入整个文件，必须采用二进制打开
    filestr.open(fn, ios::binary);
    // 获取filestr对应buffer对象的指针
    pbuf = filestr.rdbuf();

    // 调用buffer对象方法获取文件大小
    // size = pbuf->pubseekoff(0, ios::end, ios::in);
    // pbuf->pubseekpos(0, ios::in);

    // 分配内存空间
    fileBuf = new char[size];

    // 获取文件内容
    pbuf->sgetn(fileBuf, size);
    fileBuf[size] = '\0';
    filestr.close();

    // 显示文件内容
    cout.write(fileBuf, size);

    delete[] fileBuf;
}

char *FileManager::getFileBuf() const {
    return fileBuf;
}

void FileManager::writeSingleFile(Node *r, FILE *packPointer) {
    FILE *filePointer = fopen(r->filePath, "rb");
    // fileBuf = (char *) malloc(r->fileInfo.size);
    fileBuf = new char[r->fileInfo.size];
    fread(fileBuf, r->fileInfo.size, 1, filePointer);
    fwrite(fileBuf, r->fileInfo.size, 1, packPointer);
}

/**
 * 与文件链表操作相关的类
 */
class FileList {
private:
    LinkFile linkFile{};
    int fileNum = 0;
    char nameList[1000][1000] = {};
    char filePath[1000]{};
public:
    // 【强制】构造方法里面禁止加入任何业务逻辑，如果有初始化逻辑，请放在 init 方法中。
    explicit FileList(char *filePath);

    void initFileList();

    // 将节点插入链表
    void insertNode(Node **r, struct _finddata_t fileInfo, char *filePath);

    // 返回文件链表
    const Node *getLinkFile() const;

    // 返回文件个数
    int getFileNum() const;

    // 返回第no个文件名
    char *operator[](int no) {
        return this->nameList[no];
    }
};

FileList::FileList(char *filePath) {
    strcpy(this->filePath, filePath);
}

void FileList::insertNode(Node **r, struct _finddata_t fileInfo, char *filePath) {
    cout << "目前正在插入结点的文件：" << fileInfo.name << endl;
    Node *s = new Node;
    s->fileInfo = fileInfo;
    // 保存路径
    strcpy(s->filePath, filePath);
    // 保存文件名（带路径），先去除*.*
    // *(strrchr(s->filePath, '\\') + 1) = '\0';
    *(strrchr(s->filePath, '/') + 1) = '\0';
    strcat(s->filePath, s->fileInfo.name);
    (*r)->next = s;
    s->next = nullptr;
    *r = s;
}

int FileList::getFileNum() const {
    return fileNum;
}

void FileList::initFileList() {
    // 用于查找的句柄
    long handle;
    // struct _finddata_t是用来存储文件各种信息的结构体
    struct _finddata_t fileInfo{};
    linkFile = new Node;
    auto r = linkFile;
    linkFile->next = nullptr;

    if (!linkFile) {
        printf("创建失败");
    }
    // 第一次查找
    handle = _findfirst(filePath, &fileInfo);
    if (-1L == handle) {
        printf("文件路径出错");
    } else {
        // 下面一个if千万不能删除，不然如果搜索特定后缀名的文件无法把搜索到的第一个文件加入到结点，这是第一次搜索
        if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0) {
            insertNode(&r, fileInfo, filePath);
            strcpy(nameList[fileNum], fileInfo.name);
            ++fileNum;
        }
        while (!_findnext(handle, &fileInfo)) {
            if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0) {
                insertNode(&r, fileInfo, filePath);
                strcpy(nameList[fileNum], fileInfo.name);
                ++fileNum;
            }
        }
    }
    // 关闭句柄
    _findclose(handle);
}

const Node *FileList::getLinkFile() const {
    return linkFile;
}

/**
 * 有一个索引块，该块中有若干的索引项。
 */
class IndexManager {
private:
    FileList *fileList;
    // 包  
    FILE *fp;
    char *packPath;
    File file{};
public:
    static int count;
public:
    // 初始化写入文件指针
    void initFp(char *packPath);

    // 初始化FileList类
    FileList *initFileListClass(char *filePath);

    // 初始化offset
    int initOffset();

    // 新增一个索引块并将索引块写入文件
    void writeIndexBlock(Node *r, FILE *packPointer);

    // 获取文件指针
    FILE *getFp() const;
};

int IndexManager::count = 0;

void IndexManager::initFp(char *packPath) {
    this->packPath = packPath;
    this->fp = fopen(this->packPath, "wb");
}

FileList *IndexManager::initFileListClass(char *filePath) {
    fileList = new FileList(filePath);
    return fileList;
}

int IndexManager::initOffset() {
    int initialOffset = sizeof(fileList->getFileNum()) + fileList->getFileNum() * sizeof(File);
    cout << "initialOffset为：" << initialOffset << endl;
    return initialOffset;
}

void IndexManager::writeIndexBlock(Node *r, FILE *packPointer) {
    if (count == 0) {
        file.fileOffset = initOffset();
    }
    cout << "当前count为：" << count << endl;
    ++count;
    strcpy(file.filePath, r->filePath);
    cout << file.filePath << endl;
    file.size = r->fileInfo.size;
    strcpy(file.fileName, r->fileInfo.name);
    file.time_create = r->fileInfo.time_create;
    fwrite(&file, sizeof(file), 1, packPointer);
    file.fileOffset += r->fileInfo.size;
    printf("已写入%s信息\n", file.fileName);
}

FILE *IndexManager::getFp() const {
    return fp;
}

class PackManager {
private:
    // 【推荐】谨慎使用继承的方式来进行扩展，优先使用聚合/组合的方式来实现。
    FileManager *fileManager;
    FileList *fileList;
    IndexManager *indexManager;
    LinkFile linkFile;
    int fileNum;
    char *fileBuf;
    FILE *fp;
public:
    PackManager();

    // 打包文件
    void packFiles(char *filePath, char *packPath);

    // 显示包内所有文件
    void showFile(char *packPath);

    // 重载方法：显示特定文件
    void showFile(char *packPath, int index);

    // 解包包内的所有文件
    void unpackFiles(char *packPath, char *unpackPath);

    // 解包特定的文件
    void unpackFiles(char *packPath, char *unpackPath, int index);

    // 提取解包文件中的重复代码
    void unpackSpecificFile(File file, FILE *packPointer, char *unpackPath, int i);

    // 创建文件夹
    void createFolder(char *folderName);
};

PackManager::PackManager() {
    indexManager = new IndexManager;
}

void PackManager::packFiles(char *filePath, char *packPath) {
    // 0. 初始化
    fileList = indexManager->initFileListClass(filePath);
    indexManager->initFp(packPath);
    fp = indexManager->getFp();
    File file;
    fileList->initFileList();
    fileNum = fileList->getFileNum();

    // 1. 获取写入文件个数
    fwrite(&fileNum, sizeof(int), 1, fp);

    // 2. 写入结构体信息
    linkFile = fileList->getLinkFile()->next;
    Node *r = linkFile;
    while (r != nullptr) {
        indexManager->writeIndexBlock(r, this->fp);
        r = r->next;
    }

    // 3. 写入文件信息
    // 重新指向头
    r = fileList->getLinkFile()->next;
    // 不能是r->next != nullptr，不然最后一个文件读不到！
    while (r != nullptr) {
        fileManager->writeSingleFile(r, this->fp);
        r = r->next;
    }
    fclose(fp);
}


void PackManager::showFile(char *packPath) {
    File file;
    fp = fopen(packPath, "rb");
    fread(&fileNum, sizeof(fileNum), 1, fp);
    cout << "可以读到文件个数：" << this->fileNum << endl;
    fseek(fp, sizeof(fileNum), SEEK_SET);
    printf("===============================================================================\n");
    for (int i = 0; i < fileNum; ++i) {
        fread(&file, sizeof(file), 1, fp);
        printf("%s\t\t%lu\t\t%s", file.fileName, file.size, ctime(&file.time_create));
    }
    printf("===============================================================================\n");
    printf("\t\t\t\t\t\t文件个数：%d\n", fileNum);
}

void PackManager::showFile(char *packPath, int index) {
    File file;
    int count = 0;
    fp = fopen(packPath, "rb");
    fread(&fileNum, sizeof(fileNum), 1, fp);
    cout << "可以读到文件个数：" << this->fileNum << endl;
    fseek(fp, sizeof(fileNum), SEEK_SET);
    printf("===============================================================================\n");
    for (int i = 0; i < fileNum; ++i) {
        if (count == index) {
            getchar();
            count = 0;
        }
        ++count;
        fread(&file, sizeof(file), 1, fp);
        printf("%s\t\t%lu\t\t%s", file.fileName, file.size, ctime(&file.time_create));
    }
    printf("===============================================================================\n");
    printf("\t\t\t\t\t\t文件个数：%d\n", fileNum);
}

void PackManager::unpackFiles(char *packPath, char *unpackPath) {
    int i = 1, fileNumFromPack;
    // 包指针
    FILE *pIobuf, *fq;
    // 文件类型结构体
    File file;

    // 文件夹不存在则创建文件夹
    createFolder(unpackPath);

    // 打开包
    pIobuf = fopen(packPath, "rb");
    if (!pIobuf) {
        printf("包打开失败");
    }
    fread(&fileNumFromPack, sizeof(int), 1, pIobuf);
    cout << "可以读到文件个数：" << fileNumFromPack << endl;
    while (i <= fileNumFromPack) {
        this->unpackSpecificFile(file, pIobuf, unpackPath, i);
        ++i;
    }
}

void PackManager::unpackFiles(char *packPath, char *unpackPath, int index) {
    int i = 0, fileNumFromPack;
    FILE *pIobuf, *fq;
    File file;

    createFolder(unpackPath);
    pIobuf = fopen(packPath, "rb");
    if (!pIobuf) {
        printf("包打开失败");
    }
    fread(&fileNumFromPack, sizeof(int), 1, pIobuf);
    while (i <= fileNumFromPack) {
        // int compareResult = strcmp((*fileList)[i], file.fileName);
        if (i == index) {
            this->unpackSpecificFile(file, pIobuf, unpackPath, i);
            ++i;
        } else {
            // 移到下一个写进去的结构体处
            fseek(pIobuf, sizeof(fileNum) + i * sizeof(file), SEEK_SET);
            ++i;
        }
    }
}

void PackManager::createFolder(char *folderName) {
    if (_access(folderName, 0) == -1) {
        _mkdir(folderName);
    }
}

inline void PackManager::unpackSpecificFile(File file, FILE *packPointer, char *unpackPath, int i) {
    // 读出一段结构体
    fread(&file, sizeof(File), 1, packPointer);
    // 拼接出解包的路径
    strcpy(file.filePath, unpackPath);
    strcat(file.filePath, file.fileName);
    // 在新路径新建文件
    FILE *fq = fopen(file.filePath, "wb");
    char *buffer = (char *) malloc(file.size);
    // 移到偏移处
    fseek(packPointer, file.fileOffset, SEEK_SET);
    fread(buffer, file.size, 1, packPointer);
    fwrite(buffer, file.size, 1, fq);
    fclose(fq);
    // 移到下一个写进去的结构体处
    fseek(packPointer, sizeof(int) + i * sizeof(file), SEEK_SET);
}

int main(int argc, _TCHAR *argv[]) {
//    char *newBuffer;
//    FILE *fp = fopen("D:/dest.png", "wb");
//    auto *fm = new FileManager;
//    fm->writeSingleFile("D:/f/mugetwo.jpg", fp);

//    fm->readFile("D:/tf/22niang.jpg");
//    newBuffer = fm->getFileBuf();
//    cout << "Aaaaaaaa" << newBuffer << endl;

//    auto *fileList = new FileList("D:\\tf\\*.*");
//    int cnt = fileList->getFileNum();
//    for (int i = 0; i < cnt; ++i) {
//        cout << (*fileList)[i] << endl;
//    }


//    测试：将文件结构体的信息写入包中
//    File file;
//    auto *im = new IndexManager;
//    im->initFp("D:\\aa.aaa");
//    im->initFileListClass("D:\\f\\*.*");
//    auto *fl = im->getFileList();
//    fl->initFileList();
//    Node *r = fl->getLinkFile()->next;
//    while (r->next != nullptr) {
//        file = im->createIndexBlock(r);
//        im->writeIndex(file);
//        r = r->next;
//    }


//    for (int i = 0; i < fl->getFileNum(); ++i) {
//        cout << (*fl)[i] << endl;
//    }
//    im->initFp("D:\\aa.aaa");

//----------------------------------------
//    FILE *fp = fopen("D://aa.aaa", "rb");
//    File tempFile;
//    fread(&tempFile, sizeof(tempFile), 1, fp);
//    printf("name: %s\t\tsize:%lu\t\t%s", tempFile.fileName, tempFile.size, ctime(&tempFile.time_create));
//    return 0;

    auto *pm = new PackManager;
    pm->packFiles("D://aaa/*.txt", "D:/pack.dat");
//    pm->packFiles("D:/f/*.*", "D:/dest.png");
//    pm->showFile("D:/pack.dat");
//    pm->showFile("D:/pack.dat", 1);
//    pm->unpackFiles("D:/pack.dat", "E:/aaa/");
    pm->unpackFiles("D:/pack.dat", "E:/aaa/", 3);
//    pm->unpackFiles("D:/pack.data", "E:/aaa/");
    return 0;
}
