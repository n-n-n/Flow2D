#include "AlignedHeapFloat.h"

AlignedHeapFloat::AlignedHeapFloat(): aligned(0), heap_(0)
{
}

AlignedHeapFloat::~AlignedHeapFloat()
{
    delete[] heap_;
}

FLOAT *AlignedHeapFloat::p(DWORD index)
{
    return aligned + index;
}

void AlignedHeapFloat::AllocateHeap(DWORD size, DWORD a)
{
    delete[] heap_;
    
	// マージンは　a -1
	heap_ = new FLOAT[size + a - 1];
  
	// a = 16 での例
	// y & ~(a-1) で yの端数15を差っぴくのでスタートは16からになる.
	// y = x + 15
	// でxから使わずに、yから使えば境界にそろった連続領域を使える
	aligned = (FLOAT *)((DWORD)(heap_ + a - 1) & ~(a - 1));
    alignment_ = a;
    size_ = size;
}


void AlignedHeapFloat::ReleaseHeap()
{
	delete[] heap_;
}


void AlignedHeapFloat::operator=(AlignedHeapFloat &r)
{
    FLOAT	*src_s, *src_e, *dst;

    if(0 == heap_ || (heap_ && ((r.size_ != size_) || (r.alignment_ != alignment_)))) {
        ReleaseHeap();
        AllocateHeap(r.size_, r.alignment_);
    }

    src_s = r.aligned;
    src_e = r.aligned + size_;
    dst  = aligned;

    __asm{

        push    eax
        push    ebx
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

}



