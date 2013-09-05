#ifndef _FOR_SIMD_H_
#define _FOR_SIMD_H_
#include <windows.h>

typedef unsigned char UINT8;
typedef unsigned int  UINT32;
typedef float		  FLOAT;

//typedef unsigned int DWORD;	

typedef struct _cpu_info_ { DWORD   eax_, ebx_, ecx_, edx_;} cpuinfo;

void CPUID(DWORD param, cpuinfo *result);

bool IsCPUID();
//--------------------------------------------------------------
// CPUID をみてサポート状況を調べる
//--------------------------------------------------------------
class FeatureInfo
{
public:
    FeatureInfo();
   ~FeatureInfo();
    UINT32 GetFeatureInfo();
    UINT8 *GetVendorID();
    bool HasMMX();
    bool HasSSE();
    bool HasSSE2();
private:
    UINT32	feature;
    UINT8   VendorID[13];
};

//--------------------------------------------------------------
// Heap領域のアライメントは保障されないので、
// 広めに確保してアラインできる領域を使用領域にする。
//--------------------------------------------------------------
class AlignedHeap8
{
public:
    AlignedHeap8();
   ~AlignedHeap8();
    void operator=(AlignedHeap8 &r);
    void AllocateHeap(DWORD w, DWORD h, UINT32 a);
    void AllocateHeap(UINT32, UINT32 a);
    void ReleaseHeap();
    UINT8 *p(DWORD w, DWORD h);
    void *p(UINT32 offset);
private:
    UINT8	*heap;
    UINT8   *aligned;
    bool    allocated;
    UINT32  alignment;
    DWORD   width, height;
};

class AlignedHeapFloat
{
public:
    AlignedHeapFloat();
   ~AlignedHeapFloat();
    void operator=(AlignedHeapFloat &r);
    void AllocateHeap(DWORD w, DWORD h, UINT32 a);
    void AllocateHeap(UINT32, UINT32 a);
    void ReleaseHeap();

    FLOAT *p(DWORD w, DWORD h);
    void *p(UINT32 offset);
private:
    FLOAT	*heap;
    FLOAT   *aligned;
    bool    allocated;
    UINT32  alignment;
    DWORD   width, height;
};
#endif // _FOR_SIMD_H_