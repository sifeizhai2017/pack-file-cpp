#include <cstdlib>
#include <iostream>

using namespace std;

//�ļ���ص���Ϣȫ���ڸ��������֡�
//�����ṩ�ļ����������ڴ��С�
//�йظ��ļ��ĳ��ȡ��ļ�����·��������
//��������࣬��ص��ļ���Ϣ��ȫ���ˡ�����Ҫ�ٴδ��ļ���ȡ��
struct TXX {
    //....  �ļ���Ϣ
}

class FileMan {
    char *filebuf;
public:
    void readfile(char *fn); //���ļ����ݶ����ڴ���
    TXX fileinfo();

    char *filecontxt();

    //void  writefile(TXX *,char *fcontxt);

};

//�о�Ŀ¼�µ������ļ�
class FileList {
public:
    FileList(char *path);

    //[]���أ�����ֱ�ӴӸ����ʵ�����صڼ����ļ���
    char *operator[](int no) {
    }

    int filecount(); //Ŀ¼���ļ��ĸ�����
};

//�����ࡣ��һ�������飬�ÿ��������ɵ������
struct IdxStr {
};

class IndexMan {
public:
    void createidxblock();

    int getavalilableidx();

    void writeidx(IdxStr *is);

    void listidx();


    char *unfile(int no); //no��listidx�е���ţ����ص��Ǹ��ļ������ݣ�
    //�й��ļ��Ľṹ��IdxStr�С�
    //���ļ���Ϣ��idxStrת��TXX�С�


    void addfile(FileMan *fm);

};

int main(int argc, char *argv[]) {
    FileMan fm;
    fm.readfile("C:\\test.txt");
    fm.fileinfo().length();

    FileList fl("c:\\windows");
    int cnt = fl.filecount();
    for (int i0; i < cnt; i++) {
        printf("%s\n", fl[i]);
    }


    system("PAUSE");
    return EXIT_SUCCESS;
}
