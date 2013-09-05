#include <windows.h>
#ifndef _ALIGNED_HEAP_FLOAT_
#define _ALIGNED_HEAP_FLOAT_

class AlignedHeapFloat
{
public:
    AlignedHeapFloat();
   ~AlignedHeapFloat();

    FLOAT *p(DWORD index);
	FLOAT   *aligned;

    void AllocateHeap(DWORD size, DWORD a);	// a ‚Í®—ñ‹«ŠE‚Ì‘å‚«‚³
    void ReleaseHeap();

	void operator=(AlignedHeapFloat &r);
	
private:
    FLOAT	*heap_;
    bool   allocated_;
    DWORD  alignment_;
    DWORD  size_;
};

#endif // _ALIGNED_HEAP_FLOAT_

