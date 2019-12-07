#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h> 
//----------------------------------------ȫ�ֱ��� 
unsigned filenum = 0;//�ļ����� 
//---------------------------------------- 

//----------------------------------------�ṹ�� 
typedef struct Node{//�ļ���Ϣ����Ԫ ��һ��������  �ṹ�� 
	struct	_finddata_t Info;
	char	address[260];//�ļ����Ե�ַ 
	struct	Node *next; 
}Node,*FileInfoLL; //file_information_linklist

typedef struct{//�ļ���ϢĿ¼ �ṹ�� 
	char	name[260];//��ļ�������
	char 	address[260];//[260]��ļ�������
	__time64_t time_create;//�������� 
	_fsize_t size;//���4g 
	unsigned long file_offset;//ƫ���� 
}FileinfoL; //file_information_list
//---------------------------------------- 

//----------------------------------------�������� 
FileInfoLL get_fileinfo (char *search_address);//�����ļ����ļ�
void InfoInsert(Node **r,struct _finddata_t FileInfo,char *SearchAddress);//���ļ���Ϣ��������ļ���Ϣ
void LLPrint(FileInfoLL L);//����ļ���Ϣ����
void LLdestory(FileInfoLL L);//�ļ���Ϣ��������  
void file_write(char* OutAddress,FileInfoLL L);//д�ļ�
void file_readlist(char *outfile_address);//��ѯȫ���ļ���Ϣ 
void file_readlist_p(int a,char *outfile_address);//��ѯǰn���ļ���Ϣ
void file_unpack(char *outfile_address,char *upack_address);//ȫ���������
void file_unpack_s(int a,char *outfile_address,char *upack_address);//��ָ����  
//----------------------------------------

//----------------------------------------������ 
FileInfoLL get_fileinfo (char *search_address){//�����ļ����ļ�	 
	long Handle;//���
	struct _finddata_t FileInfo;//�洢�ļ���Ϣ�Ľṹ�� 
	
	FileInfoLL L;//�����յ�����
	L = (FileInfoLL)malloc(sizeof(Node));
	if(L == NULL){
		printf("�ļ���Ϣ����ͷ����ʧ��"); 
	} 
	L->next = NULL;
	Node *r = L;//ָ�����һ��Ԫ�� 
	
	Handle = _findfirst(search_address,&FileInfo);//findfirst�����ҵ�һ���ļ�
	if((Handle = _findfirst(search_address,&FileInfo)) == -1L){ 
		printf("�ļ�·�������û���ļ�\n");
	}
	else{
		if(strcmp(FileInfo.name,".") != 0 && strcmp(FileInfo.name,"..") != 0){//����������������ļ�
			InfoInsert(&r,FileInfo,search_address);
			filenum++;
		}		 
		while(_findnext(Handle,&FileInfo) == 0){//findnext�����������ļ�
			if(strcmp(FileInfo.name,".") != 0&&strcmp(FileInfo.name,"..") != 0){
				InfoInsert(&r,FileInfo,search_address);
				filenum++;
			}		 
		}
	}
	_findclose(Handle);
	return  L; 
}

void InfoInsert(Node **r,struct _finddata_t FileInfo,char *SearchAddress){//���ļ���Ϣ��������ļ���Ϣ 
	Node *s;
	s = (Node *)malloc(sizeof(Node));
	if(s == NULL){
		printf("�ļ���Ϣ������ʧ��"); 
	} 
	if(s != NULL){
		s->Info = FileInfo;
		strcpy(s->address,SearchAddress);//��Ϊ SearchAddressΪ�ַ�������������ֱ�Ӹ�ֵ��address 
		*(strrchr(s->address,'\\') + 1) = '\0';//��ͨ���ȥ�� 
		strcat(s->address,s->Info.name);//��·�����ļ��������� 
		s->next = NULL;
		(*r)->next = s;
		(*r) = s;
	}	
}

void LLPrint(FileInfoLL L){//����ļ���Ϣ���� 
	Node *p = L->next;
	while(p->next != NULL){
		printf("%s\n%u\n%s\n",p->Info.name,p->Info.size,p->address);
		p = p->next;
	}
	printf("%s\n%s\n",p->Info.name,p->address);
}

void LLdestory(FileInfoLL L){//�ļ���Ϣ�������� 
	int count=0;
	Node *p = L;
	Node *t; 
	while(p->next!=NULL){
		t = p;
		p = p->next;
		free(t);
		count++;
	} 
	printf("%d��ɾ��",count);
	free(p);
}

void file_write(char* OutAddress,FileInfoLL L){//д�ļ�	 
	FileinfoL fileinfo_temp;//��ʱ�ļ���Ϣ�洢 
	fileinfo_temp.size = 0;//�ʼ�ļ���СΪ0 
	fileinfo_temp.file_offset = sizeof(unsigned) + filenum * sizeof(FileinfoL);
	FILE *file_out = fopen(OutAddress,"wb");//�Դ���һ���������ļ��ķ�ʽ�� 
	if(file_out == NULL){
		printf("�ļ�����ʧ�ܣ����ܵ���·������\n");//����ʧ�� 
	}
	else{
		printf("==================================================\n");
		fwrite(&filenum,sizeof(unsigned),1,file_out);//����д���ļ�����
		printf("�ļ�����д��ɹ�\n");//д��ɹ����ok
		printf("--------------------------------------------------\n");
	
		Node *p = L->next;
		while(p->next != NULL){//ͨ����ʱ��Ϣ�洢�����ļ���Ϣ��д���ļ� 
			strcpy(fileinfo_temp.address,p->address);
			strcpy(fileinfo_temp.name,p->Info.name);
			fileinfo_temp.size = p->Info.size;
			fileinfo_temp.time_create = p->Info.time_create;
			fwrite(&fileinfo_temp,sizeof(FileinfoL),1,file_out);
			fileinfo_temp.file_offset += p->Info.size;//������write����
//			printf("%s\n%u\n",fileinfo_temp.name,fileinfo_temp.size);
			printf("%s	��Ϣд��ɹ�\n",fileinfo_temp.name);//��Ϣ�ɹ����ok 
			p=p->next;
	}
		strcpy(fileinfo_temp.address,p->address);//���һ�� 
		strcpy(fileinfo_temp.name,p->Info.name);
		fileinfo_temp.size = p->Info.size;
		fileinfo_temp.time_create = p->Info.time_create;
		fwrite(&fileinfo_temp,sizeof(FileinfoL),1,file_out);
		fileinfo_temp.file_offset += p->Info.size;//������write����
		printf("%s	��Ϣд��ɹ�\n",fileinfo_temp.name);
		printf("--------------------------------------------------\n"); 

		p = L->next;//ָ��ص�ͷ�� 
		FILE *file_temp;//������ʱ�ļ� 
		while(p->next != NULL){//ͨ����ʱ��Ϣ�洢�����ļ���Ϣ��д���ļ� 
			file_temp = fopen(p->address,"rb");//�Զ�����ȡ�Ʒ�ʽ���ļ� 
			char *buffer = (char*) malloc (p->Info.size);//���붯̬�ڴ�洢�ļ����� 
			fread(buffer,p->Info.size,1,file_temp);// �����ļ����ݵ��ڴ� 
			fwrite(buffer,p->Info.size,1,file_out);//������д�뵽�ļ� 
			printf("%s	����д��ɹ�\n",p->Info.name);//�ļ�д��ɹ����ok 
			free(buffer);
			p = p->next;
		}
		file_temp = fopen(p->address,"rb");
		char *buffer = (char*) malloc (p->Info.size);
		fread(buffer,p->Info.size,1,file_temp);
		fwrite(buffer,p->Info.size,1,file_out);
		printf("%s	����д��ɹ�\n",p->Info.name);//�ļ�д��ɹ����ok 
		free(buffer);

		fclose(file_out);//�ر��ļ�������д��
		printf("--------------------------------------------------\n");
		printf("������\n"); 
		printf("==================================================\n\n");	 
	}
} 

void file_readlist(char *outfile_address){//��ѯȫ���ļ���Ϣ 
	unsigned num_out;//���ڴ�Ŷ������ļ����� 
	unsigned count;//���ڼ��� 
	FILE* out_file = fopen(outfile_address,"rb");//�Զ�������ʽ��ȡ���ļ� 
	if(out_file == NULL){
		printf("�ļ���ȡʧ��,���ܶ����ļ�·��������ļ�������\n");
	}
	else{
		fread(&num_out,sizeof(unsigned),1,out_file);//�Ƚ��ļ��������� 
		printf("**************************************************\n");
		printf("����%u���ļ�\n",num_out);
		printf("--------------------------------------------------\n");
		printf("�ļ���\t\t\t�ļ���С\t��������\n");	
		FileinfoL temp;//������ʱ�ļ���Ϣ�ṹ�� 
		for(count = 1;count <= num_out;count++){
			fread(&temp,sizeof(FileinfoL),1,out_file);
			printf("%s\t\t%lu\t\t%s",temp.name,temp.size,ctime(&temp.time_create));
		}
		printf("**************************************************\n");
		fclose(out_file);
	} 	
} 

void file_readlist_p(int a,char *outfile_address){//��ѯn���ļ���Ϣ
//	system("cls"); 
	unsigned num_out;//���ڴ�Ŷ������ļ�����
	unsigned count;//�����ж�һҳ��ȡ�˶��ٽṹ�� 
	unsigned count_all;//���ڼ��Ѷ�ȡ�Ľṹ����� 
	int page; //���ҳ�� 
	int page_all;//�ܹ��м�ҳ 
	FILE* out_file = fopen(outfile_address,"rb");
	if(out_file == NULL){
		printf("�ļ���ȡʧ��,���ܶ����ļ�·��������ļ�������\n");
	} 
	else{ 
		fread(&num_out,sizeof(unsigned),1,out_file);
		printf("**************************************************\n");
		printf("����%u���ļ�\n",num_out);

		if(num_out%a == 0){
			page_all = num_out/a;//�ж���ҳ�� 
		}
		else{
			page_all = num_out/a+1;
		}
	
		FileinfoL temp;
		char key1;//��ż���������ַ� 
	
		for(page = 1,count_all = 0;page <= page_all;page++){
			printf("--------------------------------------------------\n");
			printf("�ļ���\t\t\t�ļ���С\t��������\n");
			for(count = 1;count <= a;count++){
				count_all++;
				fread(&temp,sizeof(FileinfoL),1,out_file);
				printf("%s\t\t%lu\t\t%s",temp.name,temp.size,ctime(&temp.time_create));
				if(count_all == num_out){
					break; 
				} 
			}
			printf("��%dҳ/��%dҳ\n�����ַ�b��س����ص�һҳ�����������һҳ\n",page,page_all); 
			printf("--------------------------------------------------\n");
			key1 = getch(); 
			if(key1 == 'b'){
				page = 0;
				count_all = 0;
				count = 1;
				fseek(out_file,sizeof(unsigned),SEEK_SET);
			}
			getch();    // �����������
        	system("cls");    // ������������ʾ��ͷ��Ϣ
		}
		printf("--------------------------------------------------\n");
		printf("�б�����ʾ���\n"); 
		printf("**************************************************\n");
		fclose(out_file);
	} 
} 

void file_unpack(char *outfile_address,char *upack_address){//ȫ��������� 
	unsigned num_out;//�ļ����� 
	unsigned count;//���� 
	FileinfoL temp; //��ʱ�ļ���Ϣ�ṹ�� 
	FILE* out_file = fopen(outfile_address,"rb");//�����ƴ򿪵����ļ� 
	fread(&num_out,sizeof(unsigned),1,out_file);//�����ļ����� 
	for(count = 1;count <= num_out;count++){ 
		fread(&temp,sizeof(FileinfoL),1,out_file);//�����ļ���Ϣ 
		strcpy(temp.address,upack_address);
		strcat(temp.address,temp.name);//�����ַ�����Ϊ�ļ�����·�� 
		FILE *u = fopen(temp.address,"wb");//�½��ļ� 
		char *buffer = (char*) malloc (temp.size);//�����ļ������ݴ� 
		fseek(out_file,temp.file_offset,SEEK_SET);//ͨ���ļ���Ϣ���ļ�ƫ�ƶ�λ���ļ� 
		fread(buffer,temp.size,1,out_file);//�����ļ����ݵ��ݴ� 
		fwrite(buffer,temp.size,1,u);//д���½����ļ� 
		fclose(u);
		printf("--------------------------------------------------\n");
		printf("%s����ɹ�\n�ļ�·��Ϊ:\n%s\n",temp.name,temp.address);
		printf("--------------------------------------------------\n");
		fseek(out_file,(sizeof(unsigned)+count*sizeof(FileinfoL)),SEEK_SET);//�ص���һ���ļ���Ϣλ��	
	}
}

void file_unpack_s(int a,char *outfile_address,char *upack_address){//��ָ���� 
	unsigned num_out;//�ļ����� 
	unsigned count;//����
	FileinfoL temp; //��ʱ�ļ���Ϣ�ṹ�� 
	FILE* out_file = fopen(outfile_address,"rb");//�����ƴ򿪵����ļ�
	if(out_file == NULL){
		printf("���ʧ��,���ܶ����ļ�·��������ļ�������\n");
	}
	else{ 
		fread(&num_out,sizeof(unsigned),1,out_file);//�����ļ����� 
		if(a > num_out||a < 1){
			printf("�ļ���Ŵ���");
		}
		else{
			for(count = 1;count <= a;count++){
				fread(&temp,sizeof(FileinfoL),1,out_file);//�����ļ���Ϣ 
				if(count == a){
					strcpy(temp.address,upack_address);
					strcat(temp.address,temp.name);//�����ַ�����Ϊ�ļ�����·�� 
					FILE *u = fopen(temp.address,"wb");//�½��ļ� 
					char *buffer = (char*) malloc (temp.size);//�����ļ������ݴ� 
					fseek(out_file,temp.file_offset,SEEK_SET);//ͨ���ļ���Ϣ���ļ�ƫ�ƶ�λ���ļ� 
					fread(buffer,temp.size,1,out_file);//�����ļ����ݵ��ݴ� 
					fwrite(buffer,temp.size,1,u);//д���½����ļ� 
					fclose(u);
					printf("==================================================\n");
					printf("%s����ɹ�\n�ļ�·��Ϊ:\n%s\n",temp.name,temp.address);
					printf("==================================================\n");
				}	
			}
		} 
	}
}

//----------------------------------------������

//----------------------------------------������ 
int main(int argc, char *argv[] ){//����������
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
//	char *SearchAddress="C:\\Users\\john\\Desktop\\�½��ļ���\\tmp\\*.*";//argv[1];//���ԣ������˸� 
//	char *outaddress="C:\\Users\\john\\Desktop\\�½��ļ���\\out\\my.pack";//argv[2];
//	char *unpackaddress="C:\\Users\\john\\Desktop\\�½��ļ���\\my\\";

//	file_readlist(outaddress);
////	file_readlist_p(4,outaddress);
////	file_unpack(outaddress,unpackaddress);
//	file_unpack_s(4,outaddress,unpackaddress);
	return 0;	
}
//----------------------------------------


 
