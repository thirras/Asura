#pragma once
#pragma comment (lib, "ntdll")


#include <windows.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <shlobj.h>
#include <tchar.h>
#include <thread>

#define __kernel_entry __allowed(on_function)
#define FILE_SUPERSEDED 0x00000000
#define FILE_OPENED 0x00000001
#define FILE_CREATED 0x00000002
#define FILE_OVERWRITTEN 0x00000003
#define FILE_EXISTS 0x00000004
#define FILE_DOES_NOT_EXIST 0x00000005
#define OBJ_PROTECT_CLOSE 0x00000001
#define OBJ_INHERIT 0x00000002
#define OBJ_AUDIT_OBJECT_CLOSE 0x00000004
#define OBJ_PERMANENT 0x00000010
#define OBJ_EXCLUSIVE 0x00000020
#define OBJ_CASE_INSENSITIVE 0x00000040
#define OBJ_OPENIF 0x00000080
#define OBJ_OPENLINK 0x00000100
#define OBJ_KERNEL_HANDLE 0x00000200
#define OBJ_FORCE_ACCESS_CHECK 0x00000400
#define OBJ_IGNORE_IMPERSONATED_DEVICEMAP 0x00000800
#define OBJ_DONT_REPARSE 0x00001000
#define OBJ_VALID_ATTRIBUTES 0x00001ff2

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

#ifdef BUILDING_DLL
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif

//------------------------ Structures ------------------------

typedef struct _PS_ATTRIBUTE {
    ULONGLONG Attribute;
    SIZE_T Size;
    union {
        ULONG_PTR Value;
        PVOID ValuePtr;
    };
    PSIZE_T ReturnLength;
} PS_ATTRIBUTE, * PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST {
    SIZE_T TotalLength;
    PS_ATTRIBUTE Attributes[2];
} PS_ATTRIBUTE_LIST, * PPS_ATTRIBUTE_LIST;

//0x10 bytes (sizeof)
typedef struct _IO_STATUS_BLOCK
{
    union
    {
        LONG Status;                                                        //0x0
        VOID* Pointer;                                                      //0x0
    };
    ULONGLONG Information;                                                  //0x8
}IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;                                                           //0x0
    VOID* RootDirectory;                                                    //0x8
    struct _UNICODE_STRING* ObjectName;                                     //0x10
    ULONG Attributes;                                                       //0x18
    VOID* SecurityDescriptor;                                               //0x20
    VOID* SecurityQualityOfService;                                         //0x28
}OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

//------------------------ Native function structures ------------------------
#ifndef InitializeObjectAttributes
#define InitializeObjectAttributes(p, n, a, r, s) \
    {                                             \
        (p)->Length = sizeof(OBJECT_ATTRIBUTES);  \
        (p)->RootDirectory = r;                   \
        (p)->Attributes = a;                      \
        (p)->ObjectName = n;                      \
        (p)->SecurityDescriptor = s;              \
        (p)->SecurityQualityOfService = NULL;     \
    }
#endif
typedef NTSTATUS(NTAPI* NtCreateFile)(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_opt_ PLARGE_INTEGER AllocationSize,
    _In_ ULONG FileAttributes,
    _In_ ULONG ShareAccess,
    _In_ ULONG CreateDisposition,
    _In_ ULONG CreateOptions,
    _In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
    _In_ ULONG EaLength
);

typedef NTSTATUS(NTAPI* RtlInitUnicodeStringEx)(
    _Out_ PUNICODE_STRING DestinationString,
    _In_opt_z_ PCWSTR SourceString
);

typedef NTSTATUS (NTAPI* NtClose)(
    _In_ HANDLE handle
);

// ------------------------ Not Native stuff ------------------------

extern "C" LIB_API int fileCreation (const std::string &path);
extern "C" LIB_API void iterate_subdirs(const std::string &dir_path, std::vector <std::string> &dirs);
extern "C" LIB_API HMODULE getMod(IN LPCWSTR modName);
extern "C" LIB_API wchar_t *AnsiToUnicode(const char* str);
extern "C" LIB_API void testfoo1(); // make another one and put them in asuradll
extern "C" LIB_API void testfoo2();
extern "C" LIB_API DWORD CLEANUP(
    _In_opt_ HANDLE _hProcess_,
    _In_opt_ HMODULE _dllHandle_,
    _In_opt_ HANDLE _hFileHandle_,
    _In_opt_ HANDLE _hThread_
    );

typedef void (*Piterate_sub)(const std::string &dir_path, std::vector<std::string> &dirs);
typedef int (*PfileCreation)(const std::string &path);
typedef void (*foo1)();
typedef void (*foo2)();
typedef wchar_t *(*PAnsiToUnicode)(const char* str);
typedef DWORD (*PCLEANUP)(
    _In_opt_ HANDLE _hProcess_,
    _In_opt_ HMODULE _dllHandle_,
    _In_opt_ HANDLE _hFileHandle_,
    _In_opt_ HANDLE _hThread_
);