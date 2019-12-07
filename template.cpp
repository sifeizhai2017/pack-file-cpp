#include <cstdlib>
#include <iostream>

using namespace std;

//文件相关的信息全部在该类中体现。
//该类提供文件的内容在内存中。
//有关该文件的长度、文件名、路径。。。
//有了这个类，相关的文件信息就全有了。不需要再次打开文件获取。
struct TXX {
    //....  文件信息
}

class FileMan {
    char *filebuf;
public:
    void readfile(char *fn); //把文件内容读到内存中
    TXX fileinfo();

    char *filecontxt();

    //void  writefile(TXX *,char *fcontxt);

};

//列举目录下的所有文件
class FileList {
public:
    FileList(char *path);

    //[]重载，可以直接从该类的实例返回第几个文件。
    char *operator[](int no) {
    }

    int filecount(); //目录中文件的个数。
};

//索引类。有一个索引块，该块中有若干的索引项。
struct IdxStr {
};

class IndexMan {
public:
    void createidxblock();

    int getavalilableidx();

    void writeidx(IdxStr *is);

    void listidx();


    char *unfile(int no); //no是listidx中的序号，返回的是该文件的内容，
    //有关文件的结构在IdxStr中。
    //把文件信息从idxStr转到TXX中。


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
