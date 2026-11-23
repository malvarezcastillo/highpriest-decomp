#include "common.h"
#include "GPak.h"
#include "File.h"
#include "hash.h"

#include <windows.h>
#include <string.h>
#include <list>
#include <utility>



bool            g_readFolderFirst = false;



CFileMgr g_fileMgr;






CFileMgr::~CFileMgr()
{
	for (GPakListIter iter = m_pakList.begin(); iter != m_pakList.end(); ++iter) {
		delete iter->first;
		delete iter->second;
	}
}

bool CFileMgr::AddPak(const char* name)
{
	typedef std::pair<CMemFile*, CGPak*> GDataPair;
	m_pakList.push_front(GDataPair(new CMemMapFile(name), new CGPak));
	return m_pakList.front().second->Open(m_pakList.front().first);
}

void* CFileMgr::GetData(const char* name, unsigned long* size)
{
	if (g_readFolderFirst) {
		void* data;
		if (data = GetFile(name, size)) {
			return data;
		} else if (data = GetPak(name, size)) {
			return data;
		} else {
			return NULL;
		}
	} else {
		void* data;
		if (data = GetPak(name, size)) {
			return data;
		} else if (data = GetFile(name, size)) {
			return data;
		} else {
			return NULL;
		}
	}
}

void CFileMgr::ReleaseData(void* data)
{
	VirtualFree(data, 0, MEM_RELEASE);
}

bool CFileMgr::IsDataExist(const char* name)
{
	for (GPakListIter iter = m_pakList.begin(); iter != m_pakList.end(); ++iter) {
		CHash tmp(name);
		if (iter->second->GetInfo(tmp, NULL)) {
			return true;
		}
	}
	return IsFileExist(name);
}

void* CFileMgr::GetPak(const char* name, unsigned long* size)
{
	for (GPakListIter iter = m_pakList.begin(); iter != m_pakList.end(); ++iter) {
		PakPack pakPack;
		CHash tmp(name);
		if (iter->second->GetInfo(tmp, &pakPack)) {
			*size = pakPack.m_size;
			void* data = VirtualAlloc(NULL, pakPack.m_size + 8, MEM_COMMIT, PAGE_READWRITE);
			if (iter->second->GetData(pakPack, data)) {
				return data;
			}
			VirtualFree(data, 0, MEM_RELEASE);
		}
	}
	return NULL;
}

void* CFileMgr::GetFile(const char* name, unsigned long* size)
{
	DWORD RByte;
	HANDLE hFile = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL,
							  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		*size = GetFileSize(hFile, NULL);
		if (size > 0) {
			void* data = VirtualAlloc(NULL, *size, MEM_COMMIT, PAGE_READWRITE);
			if (data) {
				ReadFile(hFile, data, *size, &RByte, NULL);
				CloseHandle(hFile);
				return data;
			}
			VirtualFree(data, 0, MEM_RELEASE);
		}
		CloseHandle(hFile);
	}
	return NULL;
}

bool CFileMgr::IsFileExist(const char* name)
{
	WIN32_FIND_DATA findData;
	HANDLE fFind = FindFirstFile(name, &findData);
	if (fFind == INVALID_HANDLE_VALUE) {
		return false;
	} else {
		FindClose(fFind);
		return true;
	}
}






CFile::CFile()
{
	m_hFile = NULL;
	m_buf = NULL;
}

CFile::~CFile()
{
	Close();
}

bool CFile::Open(const char* fName, unsigned int nOpenFlags)
{
	if (nOpenFlags == modeRead) {
		MakeFileName(m_fileName, fName);
		m_cursor = 0;

		m_buf = (char*)g_fileMgr.GetData(m_fileName, &m_size);

		if (m_buf != NULL) {
			return true;
		} else {
			return false;
		}
	} else if (nOpenFlags & modeWrite) {
		strcpy(m_fileName, fName);
		m_hFile = CreateFile(fName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
							 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile != INVALID_HANDLE_VALUE) {
			m_size = GetFileSize(m_hFile, NULL);
			return true;
		} else {
			ErrorMsg(m_fileName);
			return false;
		}
	} else {
		return false;
	}
}

void CFile::Close()
{
	if (m_hFile) {
		CloseHandle(m_hFile);
		m_hFile = NULL;
	} else if (m_buf) {
		VirtualFree(m_buf, 0, MEM_RELEASE);
		m_buf = NULL;
	}
}

bool CFile::Read(void* lpBuf, unsigned int nCount)
{
	if (m_hFile) {
		DWORD readed;
		return ReadFile(m_hFile, lpBuf, nCount, &readed, NULL) ? true : false;
	} else if (m_buf) {
		if (m_cursor + nCount > m_size) {
			return false;
		}
		memcpy(lpBuf, m_buf + m_cursor, nCount);
		m_cursor += nCount;
		return true;
	} else {
		return false;
	}
}

bool CFile::Write(const void* lpBuf, unsigned int nCount)
{
	if (m_hFile) {
		DWORD written;
		return WriteFile(m_hFile, lpBuf, nCount, &written, NULL) ? true : false;
	} else {
		return false;
	}
}

bool CFile::Seek(long lOff, unsigned int nFrom)
{
	if (m_hFile) {
		SetFilePointer(m_hFile, lOff, 0, nFrom);
		return true;
	} else if (m_buf) {
		switch (nFrom) {
		case begin:   m_cursor = lOff;          break;
		case current: m_cursor += lOff;         break;
		case end:     m_cursor = m_size + lOff; break;
		}
		if (m_cursor > m_size) {
			m_cursor = m_size;
			return false;
		} else {
			return true;
		}
	} else {
		return false;
	}
}

bool CFile::IsFileExist(const char* fName)
{
	char fullName[256];
	MakeFileName(fullName, fName);

	return g_fileMgr.IsDataExist(fullName);
}

void CFile::MakeFileName(char* output, const char* input)
{
	strcpy(output, g_baseDir);
	strcat(output, input);

	if (strstr(output, ":")) {
		char temp[1024];
		int pos = strstr(output, ":") - output - 1;
		memcpy(temp, output + pos, strlen(output) - pos + 1);
		strcpy(output, temp);
	}

	while (strstr(output, "//")) {
		int pos = strstr(output, "//") - output;
		char temp[1024];
		memcpy(temp, output, pos);
		strcpy(temp + pos, output + pos + 1);
		strcpy(output, temp);
	}
}
