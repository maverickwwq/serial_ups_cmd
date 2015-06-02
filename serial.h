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
//DESCRIPTION:�Ľ��Ĵ���ͨ�ŵĺ���
//****************************************************************************************************************




//�򿪲���ʼ������
extern HANDLE initialCom(const char* comX,const unsigned int bufSize);
//�رմ���
extern BOOL closeCom(HANDLE hCom);
//���ô��ڳ�ʱ����
extern BOOL setComTimeout(HANDLE hCom,COMMTIMEOUTS timeouts);
//���ô��ڲ���
extern BOOL setComPara(HANDLE hCom,const char* setup);
//�򴮿�д����
extern DWORD writeToCom(HANDLE hCom,const char* dataPtr,DWORD numToWrite);
//�Ӵ��ڶ�����
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
	char com[20]="\\\\.\\";								//����com10���ϵ�com��
	strcat(com,comX);									//
	HANDLE hCom=CreateFile(comX,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,NULL);						//��ʼ�����
	SetupComm(hCom,bufSize,bufSize);					//����bufSize bytes�Ķ�д������
	return hCom;										//���ؾ��
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
	return(SetCommTimeouts(hCom,&timeouts));					//���ó�ʱ�趨
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
	GetCommState(hCom,&dcbBuf);							//��ȡԭʼdcb
	BuildCommDCB(setup,&dcbBuf);						//����dcb
	return (SetCommState(hCom,&dcbBuf));				//����com�ڲ���
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

// ���ͺ�ѭ���ȴ��汾
DWORD writeToCom(HANDLE hCom,const char* dataPtr,DWORD numToWrite){
	OVERLAPPED overlapped={0,0,0,0,0};					//�������û�г�ʼ��Ϊ{0,0,0,0,0}��ֻ������writefile�����
	DWORD numOfWritten=0,error=0;
	COMSTAT	comStat;
	PurgeComm(hCom,PURGE_TXCLEAR);						//������ͻ���
	ClearCommError(hCom,&error,&comStat);				//���io����
	BOOL writeState=WriteFile(hCom,dataPtr,numToWrite,NULL,&overlapped);
	if(GetLastError() == ERROR_IO_PENDING){
		GetOverlappedResult(hCom,&overlapped,&numOfWritten,TRUE);//�����ȴ��첽����������ŷ���
	}
	PurgeComm(hCom,PURGE_TXCLEAR);						//������ͻ���
	return numOfWritten;
}


DWORD readFromCom(HANDLE hCom,char* dataPtr,DWORD numToRead){
	OVERLAPPED overlapped={0,0,0,0,0};
	DWORD numOfRead=0,error;
	COMSTAT	comStat;
	ClearCommError(hCom,&error,&comStat);								//���IO����
	BOOL readState=ReadFile(hCom,dataPtr,numToRead,&numOfRead,&overlapped);
	if(GetLastError() == ERROR_IO_PENDING){
		GetOverlappedResult(hCom,&overlapped,&numOfRead,TRUE);	//�����ȴ��첽����������ŷ���
	}
	PurgeComm(hCom,PURGE_RXCLEAR);								//������ջ�����
	return numOfRead;
}


// ���ͺ���������汾
// DWORD writeToCom(HANDLE hCom,const char* dataPtr,DWORD numToWrite){
// 	char * writeBuf;												//д����
// 	unsigned long numOfWritten=0,									//ʵ��д����
// 		errors;														//
// 	OVERLAPPED	overlapped={0,0,0,0,0};								//
// 	COMSTAT	comStat;												//
// 	ClearCommError(hCom,&errors,&comStat);						//�������
// 	WriteFileEx(hCom,dataPtr,numToWrite,&overlapped,writeCallback);
// 	SleepEx(INFINITE,TRUE);
// 	GetOverlappedResult(hCom,&overlapped,&numOfWritten,FALSE);
// 	FlushFileBuffers(hCom);
// 	PurgeComm(hCom,PURGE_TXCLEAR);
// 	return numOfWritten;
// }

// ���ͺ������汾
// DWORD readFromCom(HANDLE hCom,char* dataPtr,DWORD numToRead){
// 	OVERLAPPED overlapped={0,0,0,0,0};
// 	DWORD numOfRead=0,error;
// 	COMSTAT	comStat;
// 	ClearCommError(hCom,&error,&comStat);								//���IO����
// 	ReadFileEx(hCom,dataPtr,numToRead,&overlapped,readCallback);
// 	SleepEx(INFINITE,TRUE);
// //	if(GetLastError() == ERROR_IO_PENDING){
// //		printf("Wait");
// 		GetOverlappedResult(hCom,&overlapped,&numOfRead,FALSE);	//�����ȴ��첽����������ŷ���
// //		printf(" For\n");
// //	}
// 	PurgeComm(hCom,PURGE_RXCLEAR);								//������ջ�����
// 	return numOfRead;
// }




// void CALLBACK writeCallback(DWORD errorCode,DWORD numOfWritten,OVERLAPPED* overlapped){
// 	printf("�򴮿ڷ���%d�ֽ�\n",numOfWritten);
// }
// 
// void CALLBACK readCallback(DWORD errorCode,DWORD numOfRead,OVERLAPPED* overlapped){
// 	printf("���ڽ���%d�ֽ�\n",numOfRead);
// }