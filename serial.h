#ifndef WINDOWS_H

#define WINDOWS_H
#define _WIN32_WINNT 0x0400	
#include <windows.h>
#endif


#define SERIAL_H


// #ifdef _DEBUG_
// #define WRITE_CALL_BACK_ROUTINE writeCallback
// #define READ_CALL_BACK_ROUTINE readCallback
// #else	
// #define	WRITE_CALL_BACK_ROUTINE	NULL
// #define READ_CALL_BACK_ROUTINE	NULL
// #endif

//****************************************************************************************************************
//AUTHOR:KIMI WANG
//DATE:2013-03-05
//DESCRIPTION:改进的串口通信的函数
//****************************************************************************************************************




//打开并初始化串口
extern HANDLE initialCom(const char* comX,const unsigned int bufSize);
//关闭串口
extern BOOL closeCom(HANDLE hCom);
//设置串口超时参数
extern BOOL setComTimeout(HANDLE hCom,COMMTIMEOUTS timeouts);
//设置串口参数
extern BOOL setComPara(HANDLE hCom,const char* setup);
//向串口写数据
extern DWORD writeToCom(HANDLE hCom,const char* dataPtr,DWORD numToWrite);
//从串口读数据
extern DWORD readFromCom(HANDLE hCom,char* dataPtr,DWORD numToRead);

//
void CALLBACK writeCallback(DWORD errorCode,DWORD numOfWritten,OVERLAPPED* overlapped);
//
void CALLBACK readCallback(DWORD errorCode,DWORD numOfRead,OVERLAPPED* overlapped);



//************************************
// Method:    initialCom
// FullName:  initialCom
// Access:    public 
// Returns:   HANDLE
// Qualifier:
// Parameter: const char * comX
// Parameter: const unsigned int bufSize
//************************************
HANDLE initialCom(const char* comX,const unsigned int bufSize){
	char com[20]="\\\\.\\";								//兼容com10以上的com口
	strcat(com,comX);									//
	HANDLE hCom=CreateFile(comX,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,NULL);						//初始化句柄
	SetupComm(hCom,bufSize,bufSize);					//设置bufSize bytes的读写缓冲区
	return hCom;										//返回句柄
}

//************************************
// Method:    setComTimeout
// FullName:  setComTimeout
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: HANDLE hCom
// Parameter: COMMTIMEOUTS timeouts
//************************************
BOOL setComTimeout(HANDLE hCom,COMMTIMEOUTS timeouts){
	return(SetCommTimeouts(hCom,&timeouts));					//设置超时设定
}


//************************************
// Method:    setComPara
// FullName:  setComPara
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: HANDLE hCom
// Parameter: const char * setup
//************************************
BOOL setComPara(HANDLE hCom,const char* setup){
	DCB	dcbBuf;
	GetCommState(hCom,&dcbBuf);							//获取原始dcb
	BuildCommDCB(setup,&dcbBuf);						//设置dcb
	return (SetCommState(hCom,&dcbBuf));				//设置com口参数
}

//************************************
// Method:    closeCom
// FullName:  closeCom
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: HANDLE hCom
//************************************
BOOL closeCom(HANDLE hCom){
	return CloseHandle(hCom);
}

// 发送后循环等待版本
DWORD writeToCom(HANDLE hCom,const char* dataPtr,DWORD numToWrite){
	OVERLAPPED overlapped={0,0,0,0,0};					//这里如果没有初始化为{0,0,0,0,0}而只是声明writefile会出错
	DWORD numOfWritten=0,error=0;
	COMSTAT	comStat;
	PurgeComm(hCom,PURGE_TXCLEAR);						//清除发送缓存
	ClearCommError(hCom,&error,&comStat);				//清除io错误
	BOOL writeState=WriteFile(hCom,dataPtr,numToWrite,NULL,&overlapped);
	if(GetLastError() == ERROR_IO_PENDING){
		GetOverlappedResult(hCom,&overlapped,&numOfWritten,TRUE);//函数等待异步操作结束后才返回
	}
	PurgeComm(hCom,PURGE_TXCLEAR);						//清除发送缓存
	return numOfWritten;
}


DWORD readFromCom(HANDLE hCom,char* dataPtr,DWORD numToRead){
	OVERLAPPED overlapped={0,0,0,0,0};
	DWORD numOfRead=0,error;
	COMSTAT	comStat;
	ClearCommError(hCom,&error,&comStat);								//清除IO错误
	BOOL readState=ReadFile(hCom,dataPtr,numToRead,&numOfRead,&overlapped);
	if(GetLastError() == ERROR_IO_PENDING){
		GetOverlappedResult(hCom,&overlapped,&numOfRead,TRUE);	//函数等待异步操作结束后才返回
	}
	PurgeComm(hCom,PURGE_RXCLEAR);								//清除接收缓冲区
	return numOfRead;
}


// 发送后进程阻塞版本
// DWORD writeToCom(HANDLE hCom,const char* dataPtr,DWORD numToWrite){
// 	char * writeBuf;												//写缓冲
// 	unsigned long numOfWritten=0,									//实际写字数
// 		errors;														//
// 	OVERLAPPED	overlapped={0,0,0,0,0};								//
// 	COMSTAT	comStat;												//
// 	ClearCommError(hCom,&errors,&comStat);						//清除错误
// 	WriteFileEx(hCom,dataPtr,numToWrite,&overlapped,writeCallback);
// 	SleepEx(INFINITE,TRUE);
// 	GetOverlappedResult(hCom,&overlapped,&numOfWritten,FALSE);
// 	FlushFileBuffers(hCom);
// 	PurgeComm(hCom,PURGE_TXCLEAR);
// 	return numOfWritten;
// }

// 发送后阻塞版本
// DWORD readFromCom(HANDLE hCom,char* dataPtr,DWORD numToRead){
// 	OVERLAPPED overlapped={0,0,0,0,0};
// 	DWORD numOfRead=0,error;
// 	COMSTAT	comStat;
// 	ClearCommError(hCom,&error,&comStat);								//清除IO错误
// 	ReadFileEx(hCom,dataPtr,numToRead,&overlapped,readCallback);
// 	SleepEx(INFINITE,TRUE);
// //	if(GetLastError() == ERROR_IO_PENDING){
// //		printf("Wait");
// 		GetOverlappedResult(hCom,&overlapped,&numOfRead,FALSE);	//函数等待异步操作结束后才返回
// //		printf(" For\n");
// //	}
// 	PurgeComm(hCom,PURGE_RXCLEAR);								//清除接收缓冲区
// 	return numOfRead;
// }




// void CALLBACK writeCallback(DWORD errorCode,DWORD numOfWritten,OVERLAPPED* overlapped){
// 	printf("向串口发送%d字节\n",numOfWritten);
// }
// 
// void CALLBACK readCallback(DWORD errorCode,DWORD numOfRead,OVERLAPPED* overlapped){
// 	printf("串口接收%d字节\n",numOfRead);
// }