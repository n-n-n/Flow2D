#include "AlignedHeapFloat.h"

AlignedHeapFloat::AlignedHeapFloat(): aligned(0), heap_(0)
{
    allocated_ = false;
}

AlignedHeapFloat::~AlignedHeapFloat()
{
    if (allocated_)   delete[] heap_;
}

FLOAT *AlignedHeapFloat::p(DWORD index)
{
    return aligned + index;
}

void AlignedHeapFloat::AllocateHeap(DWORD size, DWORD a)
{
    if(allocated_) delete[] heap_;
    heap_ = new FLOAT[size + a - 1];
  
	aligned = (FLOAT *)((DWORD)(heap_ + a - 1) & ~(a - 1));
    allocated_ = true;
    alignment_ = a;
    size_ = size;
}


void AlignedHeapFloat::ReleaseHeap()
{
    if (allocated_)  delete[] heap_;
    allocated_ = false;
}


void AlignedHeapFloat::operator=(AlignedHeapFloat &r)
{
    FLOAT	*src_s, *src_e, *dst;

    if(!allocated_ || (allocated_ && ((r.size_ != size_) || (r.alignment_ != alignment_)))) {
        ReleaseHeap();
        AllocateHeap(r.size_, r.alignment_);
    }

    src_s = r.aligned;
    src_e = r.aligned + size_;
    dst	  = aligned;

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

}



