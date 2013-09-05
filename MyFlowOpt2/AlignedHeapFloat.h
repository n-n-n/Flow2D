#include <windows.h>
#ifndef _ALIGNED_HEAP_FLOAT_
#define _ALIGNED_HEAP_FLOAT_

class AlignedHeapFloat
{
public:
    AlignedHeapFloat();
   ~AlignedHeapFloat();

    FLOAT *p(DWORD index);
	FLOAT   *aligned;						// アライメントしてある部分のポインター

    void AllocateHeap(DWORD size, DWORD a);	// a は整列境界の大きさ, (ex) 4, 8, 16,,,
    void ReleaseHeap();						// 解放

	void operator=(AlignedHeapFloat &r);
	
private:
    FLOAT	*heap_;				//　本-本体、マージン分を含む
    DWORD  alignment_;			//	アライメントサイズ
    DWORD  size_;				//  サイズ
};

#endif // _ALIGNED_HEAP_FLOAT_

