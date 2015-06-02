// stdafx.cpp : source file that includes just the standard includes
//	串口通信.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information
// 2015/6/1



#include <stdio.h>
#include "serial.h"
#include <stdlib.h>

//************************************
// Method:    asciiToHex
// FullName:  asciiToHex
// Access:    public 
// Returns:   char*
// Qualifier:
// Parameter: const char * src
// Parameter: char * dest
// 
//************************************
char* asciiToHex(const char* src,char* dest){
	unsigned int lenth=strlen(src);				//
	memset(dest,0,lenth/2+1);					//清空结果数组
	char *buf=(char*)malloc(lenth+1);			//临时数组
	memset(buf,0,lenth+1);						//
	strcpy(buf,src);							//保存参数
	unsigned char tmp;							//
	for (int i=0;i<lenth;i++){					//
		tmp=*(buf+i);							//
		if (tmp>='A' && tmp<='F'){				//先转化成实际数值
			*(buf+i)=tmp-'A'+10;				//'A'-'F'
		}
		else if (tmp>='a' && tmp<='f'){
			*(buf+i)=tmp-'a'+10;
		}
		else{
			*(buf+i)=tmp-'0';					//'1'-'9'
		}
	}
	for (i=0;i<lenth/2;i++){
		*(dest+i)=*(buf+2*i)<<4 | *(buf+2*i+1);	//内存低地址内容放到字节的高4位
	}
	free(buf);
	return dest;
}

//************************************
// Method:    hexToAscii
// FullName:  hexToAscii
// Access:    public 
// Returns:   char*
// Qualifier:
// Parameter: const char * src
// Parameter: char * dest
// Parameter: int n       //信息所包含的字节数,因为有传回数值0的可能，所以不能使用strlen
//************************************

char* hexToAscii(const char* src,char* dest,int n){
	char* buf=(char*)malloc(2*n+1);
	memset(buf,0,2*n+1);
	unsigned char tmpH,tmpL,tmp;    //tmpH存储字节的高4位,tmpL存储字节的低4位
	for(int i=0;i<n;i++){
		tmp=(unsigned char)*(src+i);
		tmpH=(tmp>> 4)& unsigned char(0x0f);
		tmp=(unsigned char)*(src+i);
		tmpL=tmp & (unsigned char)0x0f;
		*(buf+2*i)=tmpH;
		*(1+buf+2*i)=tmpL;
	}
	for(i=0;i<2*n;i++){
		if(*(buf+i)>=10 && *(buf+i)<=15){	// 10~15
			*(buf+i)=*(buf+i)+'a'-10;
		}
		else{
			*(buf+i)=*(buf+i)+'0';			//0~9
		}
		*(dest+i)=*(buf+i);
	}
	return NULL;
}


int main(int argc,char* argv[]){
	char send[100],recv[100];	//send data buffer,receive data buffer
	memset(send,0,sizeof(send));
	memset(recv,0,sizeof(recv));
	
	//open com 1
	printf("--------------------------\n");
	printf("OPEN COM 1\n");
	printf("--------------------------\n");
	HANDLE com_handle= initialCom("COM1",128);
	if( com_handle == INVALID_HANDLE_VALUE ){
		printf("<OPEN COM1 ERROR>");
		exit(0);
	}	
	if(!setComPara(com_handle,"baud=2400 parity=N data=8 stop=1")){
		printf("<SET COM PARAMETER FAILED>\n");
		exit(0);
	}
	COMMTIMEOUTS timeouts={5,10,1,10,1};
	if(!setComTimeout(com_handle,timeouts)){
		printf("<SET COM TIMEOUT FAILED>\n");
		exit(0);
	}
/*typedef struct_COMMTIMEOUTS {
    DWORD ReadIntervalTimeout;
    DWORD ReadTotalTimeoutMultiplier;
    DWORD ReadTotalTimeoutConstant;
	DWORD WriteTotalTimeoutMultiplier;
	DWORD WriteTotalTimeoutConstant;
} COMMTIMEOUTS,*LPCOMMTIMEOUTS;	*/
	//
	int numOfWrite=0,numOfRead=0,n=0;	//n 代表你想接收的字节数
	char *buf=(char*)malloc(250);
	memset(buf,0,250);
	printf("请输入ups命令[command N](n for the bytes you want to receive):");
	while(scanf("%s %d",send,&n)){
		numOfWrite=strlen(send)/2;
		asciiToHex(send,buf);
		printf("Hex send:%s\n",buf);
		numOfWrite=writeToCom(com_handle,buf,numOfWrite);
		printf("    %d    bytes sent!\n",numOfWrite);
		numOfRead=readFromCom(com_handle,recv,128);
		memset(buf,0,250);
		hexToAscii(recv,buf,n);
		printf("UPS response :%s\n",buf);
		printf("    %d    bytes received!\n",numOfRead);
		memset(send,0,sizeof(send));
		memset(recv,0,sizeof(recv));
		memset(buf,0,250);
		printf("\n\n请输入ups命令[command N](n for the bytes you want to receive):");
	}

	return 0;
}