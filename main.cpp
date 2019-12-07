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

using namespace std;

// ����
typedef struct Node {
    _finddata_t fileInfo;
    char filePath[1000];
    Node *next;
} Node, *LinkFile;

// �ļ����͵Ľṹ��
struct File {
    // �ļ�����
    char fileName[30];
    // �ļ�����
    _fsize_t size;
    // �ļ�·��
    char filePath[1000];
    // �ļ�ƫ��
    int fileOffset;
    // ��������
    __time64_t time_create;
};


/**
 * �뵥���ļ�������ص���
 */
class FileManager {
private:
    char *fileBuf;
    long size;
public:
//    ���ļ��е����ݶ�ȡ��������
    void readFile(char *fn);

//    ���ļ��е�����ֱ��д�����
    void writeSingleFile(Node *r, FILE *packPointer);

    char *getFileBuf() const;
};

void FileManager::readFile(char *fn) {
    cout << "��ǰ����д���ļ���" << fn << endl;
    filebuf *pbuf;
    ifstream filestr;
//    Ҫ���������ļ���������ö����ƴ�
    filestr.open(fn, ios::binary);
//    ��ȡfilestr��Ӧbuffer�����ָ��
    pbuf = filestr.rdbuf();

//    ����buffer���󷽷���ȡ�ļ���С
//    size = pbuf->pubseekoff(0, ios::end, ios::in);
//    pbuf->pubseekpos(0, ios::in);

//    �����ڴ�ռ�
    fileBuf = new char[size];

//    ��ȡ�ļ�����
    pbuf->sgetn(fileBuf, size);
    fileBuf[size] = '\0';
    filestr.close();

//    ��ʾ�ļ�����
    cout.write(fileBuf, size);

    delete[] fileBuf;
}

char *FileManager::getFileBuf() const {
    return fileBuf;
}

void FileManager::writeSingleFile(Node *r, FILE *packPointer) {
    FILE *filePointer = fopen(r->filePath, "rb");
//    fileBuf = (char *) malloc(r->fileInfo.size);
    fileBuf = new char[r->fileInfo.size];
    fread(fileBuf, r->fileInfo.size, 1, filePointer);
    fwrite(fileBuf, r->fileInfo.size, 1, packPointer);
}

/**
 * ���ļ�����������ص���
 */
class FileList {
private:
    LinkFile linkFile{};
    int fileNum = 0;
    char nameList[1000][1000] = {};
    char filePath[1000]{};
public:
//    ��ǿ�ơ����췽�������ֹ�����κ�ҵ���߼�������г�ʼ���߼�������� init �����С�
    explicit FileList(char *filePath);

    void initFileList();

//    ���ڵ��������
    void insertNode(Node **r, struct _finddata_t fileInfo, char *filePath);

//    �����ļ�����
    const Node *getLinkFile() const;

//    �����ļ�����
    int getFileNum() const;

//    ���ص�no���ļ���
    char *operator[](int no) {
        return this->nameList[no];
    }
};

FileList::FileList(char *filePath) {
    strcpy(this->filePath, filePath);
}

void FileList::insertNode(Node **r, struct _finddata_t fileInfo, char *filePath) {
    cout << "Ŀǰ���ڲ�������ļ���" << fileInfo.name << endl;
    Node *s = new Node;
    s->fileInfo = fileInfo;
//	����·��
    strcpy(s->filePath, filePath);
//	�����ļ�������·��������ȥ��*.*
//    *(strrchr(s->filePath, '\\') + 1) = '\0';
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
//    ���ڲ��ҵľ��
    long handle;
//	struct _finddata_t�������洢�ļ�������Ϣ�Ľṹ��
    struct _finddata_t fileInfo{};
    linkFile = new Node;
    auto r = linkFile;
    linkFile->next = nullptr;

    if (!linkFile) {
        printf("����ʧ��");
    }
//	��һ�β���
    handle = _findfirst(filePath, &fileInfo);
    if (-1L == handle) {
        printf("�ļ�·������");
    } else {
//        ����һ��ifǧ����ɾ������Ȼ��������ض���׺�����ļ��޷����������ĵ�һ���ļ����뵽��㣬���ǵ�һ������
        if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0) {
            insertNode(&r, fileInfo, filePath);
            strcpy(nameList[fileNum], fileInfo.name);
//                printf("%s\n", nameList[fileNum]);
            ++fileNum;
        }
        while (!_findnext(handle, &fileInfo)) {
            if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0) {
                insertNode(&r, fileInfo, filePath);
                strcpy(nameList[fileNum], fileInfo.name);
//                printf("%s\n", nameList[fileNum]);
                ++fileNum;
            }
        }
    }
//	�رվ��
    _findclose(handle);
}

const Node *FileList::getLinkFile() const {
    return linkFile;
}


/**
 * ��һ�������飬�ÿ��������ɵ������
 */
class IndexManager {
private:
    FileList *fileList;
//    ��
    FILE *fp;
    char *packPath;
    int initialOffset;
    File file{};
public:
    static int count;
public:
//    ��ʼ��д���ļ�ָ��
    void initFp(char *packPath);

//    ��ʼ��FileList��
    FileList *initFileListClass(char *filePath);

//    ��ʼ��offset
    int initOffset();

//    ����һ�������鲢��������д���ļ�
    void writeIndexBlock(Node *r, FILE* packPointer);

//    ��ȡ�ļ�ָ��
    FILE *getFp() const;

    virtual ~IndexManager();
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
//    cout << "file�Ĵ�СΪ��" << sizeof(File) << endl;
//    TODO: initialOffset��ȷ����Ҫ���²鿴createIdxBlock�е�ƫ���ۼ�
//    TODO: ƫ����ȷ����Ҳ�벻ͨ��
    int initialOffset = sizeof(fileList->getFileNum()) + fileList->getFileNum() * sizeof(File);
    cout << "initialOffsetΪ��" << initialOffset << endl;
    return initialOffset;
//    file.fileOffset = 0;
}

//File IndexManager::createIndexBlock(Node *r) {
////    TODO: ����ǵ�һ��ʹ�ø÷�����ʱ��һ��Ҫ����ƫ��
////    FIXME: ƫ�ƻ��Ǵ���
//    cout << count << endl;
//    if (count == 0) {
//        file.fileOffset = initOffset();
//    }
//    cout << "��ǰcountΪ��" << count << endl;
//    ++count;
//    strcpy(file.filePath, r->filePath);
//    cout << file.filePath << endl;
//    file.size = r->fileInfo.size;
//    strcpy(file.fileName, r->fileInfo.name);
//    file.time_create = r->fileInfo.time_create;
//    fwrite(&file, sizeof(File), 1, fp);
//    file.fileOffset += r->fileInfo.size;
//    printf("��д��%s��Ϣ\n", file.fileName);
//    return file;
//}

FILE *IndexManager::getFp() const {
    return fp;
}

IndexManager::~IndexManager() {
    count = 0;
    cout << "�ټ�" << endl;
}

void IndexManager::writeIndexBlock(Node *r, FILE* packPointer) {
    if (count == 0) {
        file.fileOffset = initOffset();
    }
    cout << "��ǰcountΪ��" << count << endl;
    ++count;
    strcpy(file.filePath, r->filePath);
    cout << file.filePath << endl;
    file.size = r->fileInfo.size;
    strcpy(file.fileName, r->fileInfo.name);
    file.time_create = r->fileInfo.time_create;
    fwrite(&file, sizeof(file), 1, packPointer);
    file.fileOffset += r->fileInfo.size;
//    r = r->next;
    printf("��д��%s��Ϣ\n", file.fileName);
}

class PackManager {
private:
//    ���Ƽ�������ʹ�ü̳еķ�ʽ��������չ������ʹ�þۺ�/��ϵķ�ʽ��ʵ�֡�
    FileManager *fileManager;
    FileList *fileList;
    IndexManager *indexManager;
    LinkFile linkFile;
    int fileNum;
    char *fileBuf;
    FILE *fp;
public:
    PackManager();

//    ����ļ�
    void packFiles(char *filePath, char *packPath);

//    ��ʾ���������ļ�
    void showFile(char *packPath);

//    ���ط�������ʾ�ض��ļ�
    void showFile(char *packPath, int index);

//    ������ڵ������ļ�
    void unpackFiles(char *packPath, char *unpackPath);

//    ����ض����ļ�
    void unpackFiles(char *packPath, char *unpackPath, int index);

//    ��ȡ����ļ��е��ظ�����
    void unpackSpecificFile();

//    �����ļ���
    void createFolder(char *folderName);
};

PackManager::PackManager() {
    indexManager = new IndexManager;
}

void PackManager::packFiles(char *filePath, char *packPath) {
//    ��ʼ����
//    cout << packPath << " " << filePath << endl;
    fileList = indexManager->initFileListClass(filePath);
    indexManager->initFp(packPath);
    fp = indexManager->getFp();
    File file;
    fileList->initFileList();
    fileNum = fileList->getFileNum();
//    ----------------------------------------------
//    1. ��ȡд���ļ�����
    fwrite(&fileNum, sizeof(int), 1, fp);

//    2. д��ṹ����Ϣ
    linkFile = fileList->getLinkFile()->next;
    Node *r = linkFile;
    file.size = 0;
//    ƫ���� = ��¼�ļ���Ŀ��fileNum�Ĵ�С + fileNum���ṹ��Ĵ�С
    file.fileOffset = sizeof(fileNum) + fileNum * sizeof(File);

    while (r != nullptr) {
        indexManager->writeIndexBlock(r, this->fp);
        r = r->next;
    }

//    3. д���ļ���Ϣ
//    ����ָ��ͷ
    r = fileList->getLinkFile()->next;
//    ������r->next != nullptr����Ȼ���һ���ļ���������
    while (r != nullptr) {
        fileManager->writeSingleFile(r, this->fp);
        r = r->next;
    }
    fclose(fp);
}


void PackManager::showFile(char *packPath) {
    File file;
//    TODO: ���ظ������ݷ�װ
    fp = fopen(packPath, "rb");
    fread(&fileNum, sizeof(fileNum), 1, fp);
    cout << "���Զ����ļ�������" << this->fileNum << endl;
    fseek(fp, sizeof(fileNum), SEEK_SET);
    printf("===============================================================================\n");
    for (int i = 0; i < fileNum; ++i) {
        fread(&file, sizeof(file), 1, fp);
        printf("%s\t\t%lu\t\t%s", file.fileName, file.size, ctime(&file.time_create));
    }
    printf("===============================================================================\n");
    printf("\t\t\t\t\t\t�ļ�������%d\n", fileNum);
}

void PackManager::showFile(char *packPath, int index) {
    File file;
    int count = 0;
    fp = fopen(packPath, "rb");
    fread(&fileNum, sizeof(fileNum), 1, fp);
    cout << "���Զ����ļ�������" << this->fileNum << endl;
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
    printf("\t\t\t\t\t\t�ļ�������%d\n", fileNum);
}

void PackManager::unpackFiles(char *packPath, char *unpackPath) {
    int i = 1, fileNumFromPack;
//    ��ָ��
    FILE *pIobuf, *fq;
//    �ļ����ͽṹ��
    File file;

//     �ļ��в������򴴽��ļ���
    createFolder(unpackPath);
//    �򿪰�
    pIobuf = fopen(packPath, "rb");
    if (!pIobuf) {
        printf("����ʧ��");
    }
    fread(&fileNumFromPack, sizeof(int), 1, pIobuf);
    cout << "���Զ����ļ�������" << fileNumFromPack << endl;
    while (i <= fileNumFromPack) {
        unpackSpecificFile(file, pIobuf, unpackPath);
//        ����һ�νṹ��
        fread(&file, sizeof(File), 1, pIobuf);
//        ƴ�ӳ������·��
        strcpy(file.filePath, unpackPath);
        strcat(file.filePath, file.fileName);
//        ����·���½��ļ�
        cout << file.filePath << "-----" << endl;
        fq = fopen(file.filePath, "wb");
        char *buffer = (char *) malloc(file.size);
//        �Ƶ�ƫ�ƴ�
        fseek(pIobuf, file.fileOffset, SEEK_SET);
        fread(buffer, file.size, 1, pIobuf);
        fwrite(buffer, file.size, 1, fq);
        fclose(fq);
//        �Ƶ���һ��д��ȥ�Ľṹ�崦
        fseek(pIobuf, sizeof(int) + i * sizeof(file), SEEK_SET);
        ++i;
    }
}

void PackManager::unpackFiles(char *packPath, char *unpackPath, int index) {
    int i = 0, fileNumFromPack;
//    ��ָ��
    FILE *fp, *fq;
//    �ļ����ͽṹ��
    struct File tempFile;

//     �ļ��в������򴴽��ļ���
    createFolder(unpackPath);
//    �򿪰�
    fp = fopen(packPath, "rb");
    if (!fp) {
        printf("����ʧ��");
    }
    fread(&fileNumFromPack, sizeof(int), 1, fp);
    while (i <= fileNumFromPack) {
        if (i == index) {
//            ����һ�νṹ��
            fread(&tempFile, sizeof(File), 1, fp);
//            ƴ�ӳ������·��
            strcpy(tempFile.filePath, unpackPath);
            strcat(tempFile.filePath, tempFile.fileName);
//            ����·���½��ļ�
            fq = fopen(tempFile.filePath, "wb");
//            ������
            char *buffer = (char *) malloc(tempFile.size);
//            �Ƶ�ƫ�ƴ�
            fseek(fp, tempFile.fileOffset, SEEK_SET);
            fread(buffer, tempFile.size, 1, fp);
            fwrite(buffer, tempFile.size, 1, fq);
            fclose(fq);
            ++i;
        } else {
//            �Ƶ���һ��д��ȥ�Ľṹ�崦
            fseek(fp, sizeof(fileNum) + i * sizeof(tempFile), SEEK_SET);
            ++i;
        }
    }
}

void PackManager::createFolder(char *folderName) {
    if (_access(folderName, 0) == -1) {
        _mkdir(folderName);
    }
}

int main() {
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


//    ���ԣ����ļ��ṹ�����Ϣд�����
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
    pm->unpackFiles("D:/pack.dat", "E:/aaa/");
//    pm->unpackFiles("D:/pack.data", "E:/aaa/");
    return 0;
}