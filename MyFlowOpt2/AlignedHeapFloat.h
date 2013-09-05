#include <windows.h>
#ifndef _ALIGNED_HEAP_FLOAT_
#define _ALIGNED_HEAP_FLOAT_

class AlignedHeapFloat
{
public:
    AlignedHeapFloat();
   ~AlignedHeapFloat();

    FLOAT *p(DWORD index);
	FLOAT   *aligned;						// �A���C�����g���Ă��镔���̃|�C���^�[

    void AllocateHeap(DWORD size, DWORD a);	// a �͐��񋫊E�̑傫��, (ex) 4, 8, 16,,,
    void ReleaseHeap();						// ���

	void operator=(AlignedHeapFloat &r);
	
private:
    FLOAT	*heap_;				//�@�{-�{�́A�}�[�W�������܂�
    DWORD  alignment_;			//	�A���C�����g�T�C�Y
    DWORD  size_;				//  �T�C�Y
};

#endif // _ALIGNED_HEAP_FLOAT_

