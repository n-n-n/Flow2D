#include "ForSIMD.h"
//------------------------------------
// Assume Pointer Width is 32 Bit
//-----------------------------------
#ifndef NULL
#define NULL 0
#endif // NULL

void CPUID(DWORD param, cpuinfo *result)
{

    __asm{
		// 退避
        push    eax			
        push    ebx			
        push    ecx			
        push    edx			
        push    esi	
		mov		esi,	result
        xor     eax,    eax	// ゼロにセット
        cpuid	// eax = 0 が引数
        mov     [esi + 0],	ebx
        mov     [esi + 4],  edx
        mov     [esi + 8],  ecx
        mov     eax,		1
        cpuid	// eax = 1 が引数
        mov     [esi].eax_,	eax
        mov     [esi].ebx_,   ebx
        mov     [esi].ecx_,   ecx
        mov     [esi].edx_,   edx
		// 復帰
        pop     esi
        pop     edx		
        pop     ecx
        pop     ebx
        pop     eax 
    }

   
}


 bool IsCPUID()
 {
	DWORD dwPreEFlags, dwPostEFlags;

    _asm {
		// EFlags の値の取得 
		pushfd
		pop     eax
		mov     dwPreEFlags, eax

		// CPUID 命令の存在のチェック (ID フラグのチェック) 
		xor     eax, 00200000h
		push    eax
		popfd
           
		// EFlags の値の取得 
		pushfd
		pop     eax
        mov     dwPostEFlags, eax
	}

	if(dwPreEFlags == dwPostEFlags) return false;
    return true;
 }

//---------------------------------------------------------------

FeatureInfo::FeatureInfo()
{
    UINT8	*p;
    DWORD  _eax, _ebx, _ecx, _edx;

    p = VendorID;

    __asm{
		// 退避
        push    eax			
        push    ebx			
        push    ecx			
        push    edx			
        push    esi	

        mov     esi,	p
        xor     eax,    eax
        cpuid
        mov     [esi + 0],	ebx
        mov     [esi + 4],  edx
        mov     [esi + 8],  ecx
        mov     eax,		1
        cpuid
        mov     _eax,	eax
        mov     _ebx,   ebx
        mov     _ecx,   ecx
        mov     _edx,   edx
		// 復帰
        pop     esi
        pop     edx		
        pop     ecx
        pop     ebx
        pop     eax
    }

    p[12] = '\0';
    feature = _edx;
}

FeatureInfo::~FeatureInfo()
{
}

UINT32 FeatureInfo::GetFeatureInfo()
{
    return feature;
}

UINT8 *FeatureInfo::GetVendorID()
{
    return VendorID;
}

bool FeatureInfo::HasMMX()
{
    return ((feature & 0x00800000) != 0);
}

bool FeatureInfo::HasSSE()
{
    return ((feature & 0x02000000) != 0);
}

bool FeatureInfo::HasSSE2()
{
    return ((feature & 0x04000000) != 0);
}

//-----------------------------------------------------------------------------
void AlignedHeap8::AllocateHeap(DWORD w, DWORD h, UINT32 a)
{
    if(allocated) delete[] heap;
    heap = new UINT8[w * h + a - 1];
  
	aligned = (UINT8 *)((UINT32)(heap + a - 1) & ~(a - 1));
    allocated = true;
    alignment = a;
    width = w;
    height = h;
}

void AlignedHeap8::AllocateHeap(UINT32 size, UINT32 a)
{
    if(allocated)  delete[] heap;
    heap = new UINT8[size + a - 1];
   
	aligned = (UINT8 *)((UINT32)(heap + a - 1) & ~(a - 1));
    allocated = true;
    alignment = a;
    width = size;
    height = 1;
}

void AlignedHeap8::ReleaseHeap()
{
    if (allocated)  delete[] heap;
    allocated = false;
}

UINT8 *AlignedHeap8::p(DWORD w, DWORD h)
{
    return aligned + h * width + w;
}

void *AlignedHeap8::p(UINT32 offset)
{
    if (height == 1)	return aligned + offset;
    else return NULL;
}

AlignedHeap8::AlignedHeap8()
{
    allocated = false;
}

AlignedHeap8::~AlignedHeap8()
{
    if (allocated)   delete[] heap;
}

void AlignedHeap8::operator=(AlignedHeap8 &r)
{
    UINT8	*src_s, *src_e, *dst;
    FeatureInfo	info;

    if(!allocated || (allocated && ((r.width != width) || (r.height != height) || (r.alignment != alignment)))) {
        ReleaseHeap();
        AllocateHeap(r.width, r.height, r.alignment);
    }

    src_s = r.aligned;
    src_e = r.aligned + width * height;
    dst	  = aligned;

    if (info.HasMMX()) {

        __asm{
            push	eax
            push	ebx
            push    ecx
            mov		eax,    src_s
            mov		ebx,    src_e
            mov		ecx,    dst
        COPY_LOOP:
            movq	mm0,	[eax +  0]
            movq    mm1,    [eax +  8]
            movq    mm2,    [eax + 16]
            movq    mm3,    [eax + 24]
            movq    [ecx +  0],		mm0
            movq    [ecx +  8],     mm1
            movq    [ecx + 16],     mm2
            movq    [ecx + 24],     mm3
            add		eax,    32
            add		ecx,    32
            cmp		eax,    ebx
            jl		COPY_LOOP
            emms
            pop ecx
            pop ebx
            pop eax
        }

    } else {
        for(unsigned int i = 0; i < width * height; i++) aligned[i] = r.aligned[i];
    }
}

//-----------------------------------------------------------------------------
void AlignedHeapFloat::AllocateHeap(DWORD w, DWORD h, UINT32 a)
{
    if(allocated) delete[] heap;
    heap = new FLOAT[w * h + a - 1];
  
	aligned = (FLOAT *)((UINT32)(heap + a - 1) & ~(a - 1));
    allocated = true;
    alignment = a;
    width = w;
    height = h;
}

void AlignedHeapFloat::AllocateHeap(UINT32 size, UINT32 a)
{
    if(allocated)  delete[] heap;
    heap = new FLOAT[size + a - 1];
   
	aligned = (FLOAT *)((UINT32)(heap + a - 1) & ~(a - 1));
    allocated = true;
    alignment = a;
    width = size;
    height = 1;
}

void AlignedHeapFloat::ReleaseHeap()
{
    if (allocated)  delete[] heap;
    allocated = false;
}

FLOAT *AlignedHeapFloat::p(DWORD w, DWORD h)
{
    return aligned + h * width + w;
}

void *AlignedHeapFloat::p(UINT32 offset)
{
    if (height == 1)	return aligned + offset;
    else return NULL;
}

AlignedHeapFloat::AlignedHeapFloat()
{
    allocated = false;
}

AlignedHeapFloat::~AlignedHeapFloat()
{
    if (allocated)   delete[] heap;
}

void AlignedHeapFloat::operator=(AlignedHeapFloat &r)
{
    FLOAT	*src_s, *src_e, *dst;
    FeatureInfo	info;

    if(!allocated || (allocated && ((r.width != width) || (r.height != height) || (r.alignment != alignment)))) {
        ReleaseHeap();
        AllocateHeap(r.width, r.height, r.alignment);
    }

    src_s = r.aligned;
    src_e = r.aligned + width * height;
    dst	  = aligned;

    if (info.HasSSE()) {

        __asm{
            push	eax
            push	ebx
            push    ecx
            mov		eax,    src_s
            mov		ebx,    src_e
            mov		ecx,    dst
			COPY_LOOP:
            movaps	xmm0,	DWORD PTR [eax + 0]
            movaps  xmm1,   DWORD PTR [eax + 4]
            movaps  xmm2,   DWORD PTR [eax + 8]
            movaps  xmm3,   DWORD PTR [eax + 12]
            movaps  DWORD PTR [ecx + 0], xmm0
            movaps  DWORD PTR [ecx + 4], xmm1
            movaps  DWORD PTR [ecx + 8], xmm2
            movaps  DWORD PTR [ecx + 12], xmm3
            add		eax,    16
            add		ecx,    16
            cmp		eax,    ebx
            jl		COPY_LOOP
            emms
            pop ecx
            pop ebx
            pop eax
        }

    } else {
        for(unsigned int i = 0; i < width * height; i++) aligned[i] = r.aligned[i];
    }
}

