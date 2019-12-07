#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h> 
//----------------------------------------全局变量 
unsigned filenum = 0;//文件个数 
//---------------------------------------- 

//----------------------------------------结构体 
typedef struct Node{//文件信息链表单元 第一次搜索用  结构体 
	struct	_finddata_t Info;
	char	address[260];//文件绝对地址 
	struct	Node *next; 
}Node,*FileInfoLL; //file_information_linklist

typedef struct{//文件信息目录 结构体 
	char	name[260];//最长文件名长度
	char 	address[260];//[260]最长文件名长度
	__time64_t time_create;//创建日期 
	_fsize_t size;//最大4g 
	unsigned long file_offset;//偏移量 
}FileinfoL; //file_information_list
//---------------------------------------- 

//----------------------------------------函数声明 
FileInfoLL get_fileinfo (char *search_address);//检索文件夹文件
void InfoInsert(Node **r,struct _finddata_t FileInfo,char *SearchAddress);//向文件信息链表插入文件信息
void LLPrint(FileInfoLL L);//检查文件信息链表
void LLdestory(FileInfoLL L);//文件信息链表销毁  
void file_write(char* OutAddress,FileInfoLL L);//写文件
void file_readlist(char *outfile_address);//查询全部文件信息 
void file_readlist_p(int a,char *outfile_address);//查询前n个文件信息
void file_unpack(char *outfile_address,char *upack_address);//全部解包函数
void file_unpack_s(int a,char *outfile_address,char *upack_address);//解指定包  
//----------------------------------------

//----------------------------------------函数体 
FileInfoLL get_fileinfo (char *search_address){//检索文件夹文件	 
	long Handle;//句柄
	struct _finddata_t FileInfo;//存储文件信息的结构体 
	
	FileInfoLL L;//创建空单链表
	L = (FileInfoLL)malloc(sizeof(Node));
	if(L == NULL){
		printf("文件信息链表头创建失败"); 
	} 
	L->next = NULL;
	Node *r = L;//指向最后一个元素 
	
	Handle = _findfirst(search_address,&FileInfo);//findfirst函数找第一个文件
	if((Handle = _findfirst(search_address,&FileInfo)) == -1L){ 
		printf("文件路径错误或没有文件\n");
	}
	else{
		if(strcmp(FileInfo.name,".") != 0 && strcmp(FileInfo.name,"..") != 0){//会有两个多出来的文件
			InfoInsert(&r,FileInfo,search_address);
			filenum++;
		}		 
		while(_findnext(Handle,&FileInfo) == 0){//findnext函数找其他文件
			if(strcmp(FileInfo.name,".") != 0&&strcmp(FileInfo.name,"..") != 0){
				InfoInsert(&r,FileInfo,search_address);
				filenum++;
			}		 
		}
	}
	_findclose(Handle);
	return  L; 
}

void InfoInsert(Node **r,struct _finddata_t FileInfo,char *SearchAddress){//向文件信息链表插入文件信息 
	Node *s;
	s = (Node *)malloc(sizeof(Node));
	if(s == NULL){
		printf("文件信息链表创建失败"); 
	} 
	if(s != NULL){
		s->Info = FileInfo;
		strcpy(s->address,SearchAddress);//因为 SearchAddress为字符串常量，不可直接赋值到address 
		*(strrchr(s->address,'\\') + 1) = '\0';//将通配符去掉 
		strcat(s->address,s->Info.name);//将路径和文件名连起来 
		s->next = NULL;
		(*r)->next = s;
		(*r) = s;
	}	
}

void LLPrint(FileInfoLL L){//检查文件信息链表 
	Node *p = L->next;
	while(p->next != NULL){
		printf("%s\n%u\n%s\n",p->Info.name,p->Info.size,p->address);
		p = p->next;
	}
	printf("%s\n%s\n",p->Info.name,p->address);
}

void LLdestory(FileInfoLL L){//文件信息链表销毁 
	int count=0;
	Node *p = L;
	Node *t; 
	while(p->next!=NULL){
		t = p;
		p = p->next;
		free(t);
		count++;
	} 
	printf("%d已删除",count);
	free(p);
}

void file_write(char* OutAddress,FileInfoLL L){//写文件	 
	FileinfoL fileinfo_temp;//临时文件信息存储 
	fileinfo_temp.size = 0;//最开始文件大小为0 
	fileinfo_temp.file_offset = sizeof(unsigned) + filenum * sizeof(FileinfoL);
	FILE *file_out = fopen(OutAddress,"wb");//以创建一个二进制文件的方式打开 
	if(file_out == NULL){
		printf("文件创建失败，可能导出路径有误\n");//创建失败 
	}
	else{
		printf("==================================================\n");
		fwrite(&filenum,sizeof(unsigned),1,file_out);//首先写入文件个数
		printf("文件个数写入成功\n");//写入成功输出ok
		printf("--------------------------------------------------\n");
	
		Node *p = L->next;
		while(p->next != NULL){//通过临时信息存储存入文件信息并写入文件 
			strcpy(fileinfo_temp.address,p->address);
			strcpy(fileinfo_temp.name,p->Info.name);
			fileinfo_temp.size = p->Info.size;
			fileinfo_temp.time_create = p->Info.time_create;
			fwrite(&fileinfo_temp,sizeof(FileinfoL),1,file_out);
			fileinfo_temp.file_offset += p->Info.size;//必须在write后面
//			printf("%s\n%u\n",fileinfo_temp.name,fileinfo_temp.size);
			printf("%s	信息写入成功\n",fileinfo_temp.name);//信息成功输出ok 
			p=p->next;
	}
		strcpy(fileinfo_temp.address,p->address);//最后一个 
		strcpy(fileinfo_temp.name,p->Info.name);
		fileinfo_temp.size = p->Info.size;
		fileinfo_temp.time_create = p->Info.time_create;
		fwrite(&fileinfo_temp,sizeof(FileinfoL),1,file_out);
		fileinfo_temp.file_offset += p->Info.size;//必须在write后面
		printf("%s	信息写入成功\n",fileinfo_temp.name);
		printf("--------------------------------------------------\n"); 

		p = L->next;//指针回到头上 
		FILE *file_temp;//建立临时文件 
		while(p->next != NULL){//通过临时信息存储存入文件信息并写入文件 
			file_temp = fopen(p->address,"rb");//以二进读取制方式打开文件 
			char *buffer = (char*) malloc (p->Info.size);//申请动态内存存储文件内容 
			fread(buffer,p->Info.size,1,file_temp);// 读出文件内容到内存 
			fwrite(buffer,p->Info.size,1,file_out);//将内容写入到文件 
			printf("%s	内容写入成功\n",p->Info.name);//文件写入成功输出ok 
			free(buffer);
			p = p->next;
		}
		file_temp = fopen(p->address,"rb");
		char *buffer = (char*) malloc (p->Info.size);
		fread(buffer,p->Info.size,1,file_temp);
		fwrite(buffer,p->Info.size,1,file_out);
		printf("%s	内容写入成功\n",p->Info.name);//文件写入成功输出ok 
		free(buffer);

		fclose(file_out);//关闭文件，结束写入
		printf("--------------------------------------------------\n");
		printf("打包完成\n"); 
		printf("==================================================\n\n");	 
	}
} 

void file_readlist(char *outfile_address){//查询全部文件信息 
	unsigned num_out;//用于存放读出的文件个数 
	unsigned count;//用于记数 
	FILE* out_file = fopen(outfile_address,"rb");//以二进制形式读取包文件 
	if(out_file == NULL){
		printf("文件读取失败,可能读包文件路径有误或文件不存在\n");
	}
	else{
		fread(&num_out,sizeof(unsigned),1,out_file);//先将文件个数读出 
		printf("**************************************************\n");
		printf("包含%u个文件\n",num_out);
		printf("--------------------------------------------------\n");
		printf("文件名\t\t\t文件大小\t创建日期\n");	
		FileinfoL temp;//创建临时文件信息结构体 
		for(count = 1;count <= num_out;count++){
			fread(&temp,sizeof(FileinfoL),1,out_file);
			printf("%s\t\t%lu\t\t%s",temp.name,temp.size,ctime(&temp.time_create));
		}
		printf("**************************************************\n");
		fclose(out_file);
	} 	
} 

void file_readlist_p(int a,char *outfile_address){//查询n个文件信息
//	system("cls"); 
	unsigned num_out;//用于存放读出的文件个数
	unsigned count;//用于判断一页读取了多少结构体 
	unsigned count_all;//用于记已读取的结构体个数 
	int page; //存放页码 
	int page_all;//总共有几页 
	FILE* out_file = fopen(outfile_address,"rb");
	if(out_file == NULL){
		printf("文件读取失败,可能读包文件路径有误或文件不存在\n");
	} 
	else{ 
		fread(&num_out,sizeof(unsigned),1,out_file);
		printf("**************************************************\n");
		printf("包含%u个文件\n",num_out);

		if(num_out%a == 0){
			page_all = num_out/a;//判断总页数 
		}
		else{
			page_all = num_out/a+1;
		}
	
		FileinfoL temp;
		char key1;//存放键盘输入的字符 
	
		for(page = 1,count_all = 0;page <= page_all;page++){
			printf("--------------------------------------------------\n");
			printf("文件名\t\t\t文件大小\t创建日期\n");
			for(count = 1;count <= a;count++){
				count_all++;
				fread(&temp,sizeof(FileinfoL),1,out_file);
				printf("%s\t\t%lu\t\t%s",temp.name,temp.size,ctime(&temp.time_create));
				if(count_all == num_out){
					break; 
				} 
			}
			printf("第%d页/共%d页\n输入字符b后回车返回第一页，按任意键下一页\n",page,page_all); 
			printf("--------------------------------------------------\n");
			key1 = getch(); 
			if(key1 == 'b'){
				page = 0;
				count_all = 0;
				count = 1;
				fseek(out_file,sizeof(unsigned),SEEK_SET);
			}
			getch();    // 按任意键继续
        	system("cls");    // 清屏后重新显示表头信息
		}
		printf("--------------------------------------------------\n");
		printf("列表已显示完毕\n"); 
		printf("**************************************************\n");
		fclose(out_file);
	} 
} 

void file_unpack(char *outfile_address,char *upack_address){//全部解包函数 
	unsigned num_out;//文件个数 
	unsigned count;//计数 
	FileinfoL temp; //临时文件信息结构体 
	FILE* out_file = fopen(outfile_address,"rb");//二进制打开导出文件 
	fread(&num_out,sizeof(unsigned),1,out_file);//读出文件个数 
	for(count = 1;count <= num_out;count++){ 
		fread(&temp,sizeof(FileinfoL),1,out_file);//读出文件信息 
		strcpy(temp.address,upack_address);
		strcat(temp.address,temp.name);//链接字符串成为文件绝对路径 
		FILE *u = fopen(temp.address,"wb");//新建文件 
		char *buffer = (char*) malloc (temp.size);//创建文件内容暂存 
		fseek(out_file,temp.file_offset,SEEK_SET);//通过文件信息中文件偏移定位到文件 
		fread(buffer,temp.size,1,out_file);//读出文件内容到暂存 
		fwrite(buffer,temp.size,1,u);//写入新建的文件 
		fclose(u);
		printf("--------------------------------------------------\n");
		printf("%s解包成功\n文件路径为:\n%s\n",temp.name,temp.address);
		printf("--------------------------------------------------\n");
		fseek(out_file,(sizeof(unsigned)+count*sizeof(FileinfoL)),SEEK_SET);//回到下一个文件信息位置	
	}
}

void file_unpack_s(int a,char *outfile_address,char *upack_address){//解指定包 
	unsigned num_out;//文件个数 
	unsigned count;//计数
	FileinfoL temp; //临时文件信息结构体 
	FILE* out_file = fopen(outfile_address,"rb");//二进制打开导出文件
	if(out_file == NULL){
		printf("解包失败,可能读包文件路径有误或文件不存在\n");
	}
	else{ 
		fread(&num_out,sizeof(unsigned),1,out_file);//读出文件个数 
		if(a > num_out||a < 1){
			printf("文件编号错误");
		}
		else{
			for(count = 1;count <= a;count++){
				fread(&temp,sizeof(FileinfoL),1,out_file);//读出文件信息 
				if(count == a){
					strcpy(temp.address,upack_address);
					strcat(temp.address,temp.name);//链接字符串成为文件绝对路径 
					FILE *u = fopen(temp.address,"wb");//新建文件 
					char *buffer = (char*) malloc (temp.size);//创建文件内容暂存 
					fseek(out_file,temp.file_offset,SEEK_SET);//通过文件信息中文件偏移定位到文件 
					fread(buffer,temp.size,1,out_file);//读出文件内容到暂存 
					fwrite(buffer,temp.size,1,u);//写入新建的文件 
					fclose(u);
					printf("==================================================\n");
					printf("%s解包成功\n文件路径为:\n%s\n",temp.name,temp.address);
					printf("==================================================\n");
				}	
			}
		} 
	}
}

//----------------------------------------函数体

//----------------------------------------主函数 
int main(int argc, char *argv[] ){//调用命令行
	if(argv[1][0]=='-'){
		switch(argv[1][1]){
			case 'r':break;
			case 'l':{
						if(argv[1][2]=='\0'){
							file_readlist(argv[2]);
						}
						else{
							 file_readlist_p(atoi((const char*)(&argv[1][0]+2)),argv[2]);
						}				
			}break;
			case 'u':{
						if(argv[1][2]=='\0'){
							file_unpack(argv[2],argv[3]);
						}
						else{
							 file_unpack_s(atoi((const char*)(&argv[1][0]+2)),argv[2],argv[3]);
						}				
			}break;
		}
	}
	else{
		FileInfoLL L=get_fileinfo (argv[1]);
		//LLPrint(L);
		//printf("%u",filenum);
		file_write(argv[2],L);
		LLdestory(L);
	}
//	char *SearchAddress="C:\\Users\\john\\Desktop\\新建文件夹\\tmp\\*.*";//argv[1];//测试，别忘了改 
//	char *outaddress="C:\\Users\\john\\Desktop\\新建文件夹\\out\\my.pack";//argv[2];
//	char *unpackaddress="C:\\Users\\john\\Desktop\\新建文件夹\\my\\";

//	file_readlist(outaddress);
////	file_readlist_p(4,outaddress);
////	file_unpack(outaddress,unpackaddress);
//	file_unpack_s(4,outaddress,unpackaddress);
	return 0;	
}
//----------------------------------------


 
