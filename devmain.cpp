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

// ����
typedef struct Node {
    _finddata_t fileInfo;
    char filePath[MAXSIZE];
    Node *next;
} Node, *LinkFile;

// �ļ����͵Ľṹ��
struct File {
    // �ļ�����
    char fileName[30];
    // �ļ�����
    _fsize_t size;
    // �ļ�·��
    char filePath[MAXSIZE];
    // �ļ�ƫ��
    long fileOffset;
    // ��������
    __time64_t time_create;
};

/**
 * �뵥���ļ�������ص���
 */
class FileMan {
private:
    char *fileBuf;
public:
//    ���ļ��е����ݶ�ȡ��������
    void readFile(const char *fn);

    char *getFileBuf() const;

//    File fileInfo(char *fn);
};

void FileMan::readFile(const char *fn) {
    FILE *fp = fopen(fn, "rb");
    if (fp == NULL) {
        cout << "�ļ���ʧ��" << endl;
    }

    // ���ļ�ָ���ƶ����ļ�ĩβ
    fseek(fp, 0, SEEK_END);
    // �����ǰ�ļ�ָ������ļ���ʼ���ֽ���
    long size = ftell(fp);
    // ���ļ�ָ�뷵�ص��ļ���ʼ��
    fseek(fp, 0, SEEK_SET);
    this->fileBuf = (char *) malloc(sizeof(char) * size);
    fileBuf[size] = '\0';
    fclose(fp);
}

char *FileMan::getFileBuf() const {
    return this->fileBuf;
}


/**
 * ���ļ����������ص���
 */
class FileList {
private:
    LinkFile lf;
    int fileNum;
public:
    explicit FileList(char *filePath);
//    ���ڵ��������
    void insertNode(Node **r, struct _finddata_t fileInfo, char *filePath);
//    ��������أ���ȡ��no���ļ�
    char *operator[](int no);
//    �����ļ�����
    const Node *getLf() const;
//    �����ļ�����
    int getFileNum() const;
};

FileList::FileList(char *filePath) {
    this->fileNum = 0;
//    ���ڲ��ҵľ��
    long handle;
//	struct _finddata_t�������洢�ļ�������Ϣ�Ľṹ��
    struct _finddata_t fileInfo{};
    lf = new Node;
    Node *r = lf;
    lf->next = NULL;

    if (!lf) {
        printf("����ʧ��");
    }
//	��һ�β���
    handle = _findfirst(filePath, &fileInfo);
    if (-1L == handle) {
        printf("�ļ�·������");
    } else {
//        ��һ������
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
//	�رվ��
    _findclose(handle);
}

void FileList::insertNode(Node **r, struct _finddata_t fileInfo, char *filePath) {
    Node *s = new Node;
//    cout << &s << endl;
    s->fileInfo = fileInfo;
//	printf("%s\n", filePath);
//	����·��
    strcpy(s->filePath, filePath);
//	�����ļ�������·��������ȥ��*.*
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
 * ���ļ������ص���
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
