#include "CheckSSE.h"

#define ALIGNED __declspec(align(32)) // (1)

float Calc_PI( int div_count )
{
        float width, x, pi = 0.0f;

        width = 1.0f / ( float )div_count; // (1)

        for( int i = 0; i < div_count; i++ ) {
                x = ( i + 0.5f ) * width; // (2)
                pi += 1.0f/( x * x +1.0f );
        }
        pi *= width; // (3)

        return pi;
}

static const ALIGNED float _0_5[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
static const ALIGNED float _1_0[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
static const ALIGNED float _4_0[4] = { 4.0f, 4.0f, 4.0f, 4.0f };

float Calc_PI_SSE_rcpps( int div_count )
{
    
        ALIGNED float temp[4]       = { 0.0f, 1.0f, 2.0f, 3.0f };
        ALIGNED float width_arr[4];
        float width, pi;

        width = 1.0f / ( float )div_count;

        width_arr[0] = width_arr[1] = width_arr[2] = width_arr[3] = width;

        __asm { 
                movaps xmm5, [temp]
                xorps  xmm7, xmm7  //  pi = 0
                xor    eax,	 eax    // eax = 0
			loop1:
                // x[] = ( tmep[] + _0_5[] ) * width_arr[]
                movaps xmm3, xmm5
                addps  xmm3, [_0_5]
                mulps  xmm3, [width_arr]

                // result[] = 1 / ( 1 + x[]^2 )
                mulps  xmm3, xmm3
                addps  xmm3, [_1_0]
                rcpps  xmm3, xmm3   //  �t���v�Z

                // result[0] ���� result[3]�܂ł̍��v�̌v�Z
				// �ȉ��̃V�t�g�Łi�I�v�V�����͔���񂪁j�e�����ɑS�������l��������
                movaps xmm1, xmm3
                shufps xmm1, xmm3, 0B1h
                addps  xmm1, xmm3
                movaps xmm3, xmm1
                shufps xmm1, xmm3, 04Eh
                addps  xmm1, xmm3

                addss  xmm7, xmm1   // pi += result[0]����[3])     // �܂�Ƃ���g������    
                addps  xmm5, [_4_0] // temp[] += 4�@// ���W���V�t�g

                add    eax, 4
                cmp    eax, div_count
                jl     loop1

                mulss xmm7, [width_arr]  // ���W���[��������

                mulss xmm7, [_4_0]	// 4�|����Ɠ���
                movss [pi], xmm7	// ����32bit���݂̂��g���B
        }

        return pi;
}

float Calc_PI_SSE_divps( int div_count )
{
        ALIGNED float temp[4]       = { 0.0f, 1.0f, 2.0f, 3.0f };
        ALIGNED float width_arr[4];
        float width, pi;

        width = 1.0f / ( float )div_count;

        width_arr[0] = width_arr[1] = width_arr[2] = width_arr[3] = width;

        __asm {
                prefetcht0  [_1_0]		//	diff �����BT0�e���|�����E�f�[�^-�L���b�V���K�w�̑S�Ẵ��x���Ƀf�[�^���v���t�F�b�`����A����
                movaps xmm5, [temp]		
                movaps xmm6, [_1_0]		// diff 
                xorps  xmm7, xmm7  // pi = 0�@�e�N�j�b�N���
                xor    eax, eax    // eax = 0
			loop1:
                // x[] = ( tmep[] + _0_5[] ) * width_arr[];
                movaps xmm3, xmm5
                addps  xmm3, [_0_5]
                mulps  xmm3, [width_arr]

                // result[] = 1 / ( 1 + x[]^2 )
             //   movaps xmm0, [_1_0]
				movaps xmm0, xmm6
                mulps  xmm3, xmm3
//                addps  xmm3, [_1_0]
				addps  xmm3, xmm6
				divps  xmm0, xmm3

                // result[0] ���� result[3]�܂ł̍��v�̌v�Z
                movaps xmm1, xmm0
                shufps xmm1, xmm0, 0B1h
                addps  xmm1, xmm0
                movaps xmm0, xmm1
                shufps xmm1, xmm0, 4Eh
                addps  xmm1, xmm0

                addss  xmm7, xmm1 // // pi += result[0]����[3]          
                addps  xmm5, [_4_0] // temp[] += 4

                add    eax, 4
                cmp    eax, div_count
                jl     loop1

                mulss xmm7, [width_arr]
                mulss xmm7, [_4_0]
                movss [pi], xmm7
        }

        return pi;
}