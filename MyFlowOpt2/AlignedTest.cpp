#include "AlignedTest.h"

/*
void MMX_Deswizzle(pDColor ApColor, DWORD* pColors, int ASize)
{
	_asm {
		mov ecx, [ASize]
		mov ebx, ApColor
		mov edx, out
		movq mm0, [ebx] 
	}

}
*/

void SSE_AOStoSOA(Vertex_aos* aosIn, Vertex_soa* soaOut)
{
	// xyzc xyzc,... -> xxxx 
	_asm {
		mov		eax, aosIn;	
		mov		ebx, soaOut;

		movlps	xmm7, [eax]			// xmm7 = -- -- y1 x1
		movhps	xmm7, [eax+16]		// xmm7 = y2 x2 y1 x1 
		movlps  xmm0, [eax+32]		// xmm0 = -- -- y3 x3
		movhps  xmm0, [eax+48]		// xmm0 = y4 x4 y3 x3
		movaps  xmm6, xmm7			// xmm6 = y2 x2 y1 x1 
		shufps	xmm7, xmm0, 0x88	// xmm7 = x4 x3 x2 x1
		shufps	xmm6, xmm0, 0xDD	// xmm6 = y4 y3 y2 y1

		movlps	xmm2, [eax+8]		// xmm2 = -- -- w1 z1
		movhps	xmm2, [eax+24]		// xmm2 = w2 z2 w1 z1
		movlps	xmm1, [eax+40]		// xmm1 = -- -- w3 z3
		movhps	xmm1, [eax+56]		// xmm1 = w4 z4 w3 z3
		movaps	xmm3, xmm2			// xmm3 = w2 z2 w1 z1
		shufps	xmm2, xmm1, 0x88	// xmm2 = z4 z3 z2 z1
		shufps	xmm3, xmm1, 0xDD	// xmm3 = w4 w3 w2 w1

		movaps	[ebx],		xmm7
		movaps  [ebx+16],	xmm6
		movaps  [ebx+32],	xmm2
		movaps	[ebx+48],	xmm3
	}
}

void SSE_AOStoSOP(Vertex_aos* aosIn,Vertex_sop* sopOut,const int size)
{
	int AlignedSize = size/4;
	_asm{
		mov		eax, sopOut
		mov		ecx, [AlignedSize];
		mov		edi, [eax].pcolor;
		mov		esi, [eax].pz;
		mov		edx, [eax].py;
		mov		ebx, [eax].px;
		mov		eax,  aosIn;
	LOOP1:
		movlps	xmm7, [eax]			// xmm7 = -- -- y1 x1
		movhps	xmm7, [eax+16]		// xmm7 = y2 x2 y1 x1 
		movlps  xmm0, [eax+32]		// xmm0 = -- -- y3 x3
		movhps  xmm0, [eax+48]		// xmm0 = y4 x4 y3 x3
		movaps  xmm6, xmm7			// xmm6 = y2 x2 y1 x1 
		shufps	xmm7, xmm0, 0x88	// xmm7 = x4 x3 x2 x1
		shufps	xmm6, xmm0, 0xDD	// xmm6 = y4 y3 y2 y1

		movlps	xmm2, [eax+8]		// xmm2 = -- -- w1 z1
		movhps	xmm2, [eax+24]		// xmm2 = w2 z2 w1 z1
		movlps	xmm1, [eax+40]		// xmm1 = -- -- w3 z3
		movhps	xmm1, [eax+56]		// xmm1 = w4 z4 w3 z3
		movaps	xmm3, xmm2			// xmm3 = w2 z2 w1 z1
		shufps	xmm2, xmm1, 0x88	// xmm2 = z4 z3 z2 z1
		shufps	xmm3, xmm1, 0xDD	// xmm3 = w4 w3 w2 w1

		movaps	[ebx],	xmm7
		movaps  [edx],	xmm6
		movaps  [esi],	xmm2
		movaps	[edi],	xmm3
		
		add		ebx, 16;
		add		edx, 16;
		add		esi, 16;
		add		edi, 16;
		add		eax, 64;
		
		loop LOOP1
	}
}

void SSE_SOAtoAOS2(Vertex_soa* soaIn, Vertex_aos* aosOut)
{
	// xxxx, yyyy, zzzz, wwww -> xyzw, xyzw,....

	// Hi---Lo
	_asm{
		mov		ecx, soaIn
		mov		edx, aosOut
		movaps xmm7, [ecx]		// xmm7 = x4, x3, x2, x1
		movaps xmm6, [ecx+16]	// xmm6 = y4, y3, y2, y1
		movaps xmm5, [ecx+32]	// xmm5 = z4, z3, z2, z1
		movaps xmm4, [ecx+48]	// xmm4 = w4, w3, w2, w1

		movaps xmm0, xmm7		
		unpcklps xmm7, xmm6		// xmm7 = x4, y4, x3, y3
		movlps [edx], xmm7		// out0 = x4, y4, --, --
		movhps [edx+16], xmm7	// out1 = x3, y3, --, --
		unpckhps xmm0, xmm6		// xmm0 = x2, y2, x4, y4
		movlps [edx+32], xmm0	// out2 = x2, y2, --, --
		movhps [edx+48], xmm0	// out3 = x1, y1, --, --
		movaps xmm0, xmm5		//
		unpcklps xmm5, xmm4		// xmm5 = z1, w1, z2, w2
		unpckhps xmm0, xmm4		// xmm0 = z3, w3, z4, w4
		movlps	[edx+8], xmm5	// out0 = w1, z1, y1, x1
		movhps  [edx+24], xmm5	// out1 = w2, z2, y2, x2
		movlps	[edx+40], xmm0	// out2 = w3, z3, y3, x3 
		movhps	[edx+56], xmm0	// out3 = w4, z3, y4, x4
	}
}

void SSE_SOAtoAOS( Vertex_soa* soaIn, Vertex_aos* aosOut)
{
	_asm{
		mov		ecx, soaIn
		mov		edx, aosOut
		
		// Lo---Hi 
		movaps xmm1, [ecx]		// xmm1 = x1, x2, x3, x4
		movaps xmm2, [ecx+16]	// xmm2 = y1, y2, y3, y4
		movaps xmm3, [ecx+32]	// xmm3 = z1, z2, z3, z4
		movaps xmm4, [ecx+48]	// xmm4 = w1, w2, w3, w4

		movaps	xmm7, xmm4		// xmm7 = w1, w2, w3, w4 
		movhlps xmm7, xmm3		// xmm7 = z3, z4, w3, w4
		
		movaps	xmm6, xmm2		// xmm6 = y1, y2, y3, y4
		movlhps	xmm3, xmm4		// xmm3 = z1, z2, w1, w2
		movhlps xmm2, xmm1		// xmm2 = x3, x4, y3, y4
		movlhps xmm1, xmm6		// xmm1 = x1, x2, y1, y2
		
		movaps	xmm6, xmm2		// xmm6 = x3, x4, y3, y4
		movaps	xmm5, xmm1		// xmm5 = x1, x2, y1, y2

		shufps  xmm2, xmm7, 0xDD	// xmm2 = x4, y4, z4, w4 // xmm2 = x3, x4, y3, y4,  xmm7 = z3, z4, w3, w4 
		shufps	xmm1, xmm3, 0x88	// xmm1 = x1, y1, z1, w1 // xmm1 = x1, x2, y1, y2,  xmm3 = z1, z2, w1, w2,
		shufps	xmm5, xmm3, 0xDD	// xmm5 = x2, y2, z2, w2 // xmm5 = x1, x2, y1, y2,  xmm3 = z1, z2, w1, w2
		shufps	xmm6, xmm7, 0x88	// xmm6 = x3, y3, z3, w3 // xmm6 = x3, x4, y3, y4,  xmm7 = z3, z3, w3, w4
		movaps  [edx],	  xmm1
		movaps  [edx+16], xmm5
		movaps	[edx+32], xmm6
		movaps  [edx+48], xmm2
	}
}

void SSE_SOPtoAOS(Vertex_sop* sopIn, Vertex_aos* aosOut,const  int size)
{
	int AlignedSize = size/4;
	_asm{
		mov		eax, sopIn
		mov		ecx, [AlignedSize];
		mov		edi, [eax].pcolor;
		mov		esi, [eax].pz;
		mov		edx, [eax].py;
		mov		ebx, [eax].px;
		mov		eax, aosOut;
	LOOP1:
								// Lo---Hi 
		movaps xmm1, [ebx]		// xmm1 = x1, x2, x3, x4
		movaps xmm2, [edx]		// xmm2 = y1, y2, y3, y4
		movaps xmm3, [esi]		// xmm3 = z1, z2, z3, z4
		movaps xmm4, [edi]		// xmm4 = w1, w2, w3, w4

		movaps	xmm7, xmm4		// xmm7 = w1, w2, w3, w4 
		movhlps xmm7, xmm3		// xmm7 = z3, z4, w3, w4
		
		movaps	xmm6, xmm2		// xmm6 = y1, y2, y3, y4
		movlhps	xmm3, xmm4		// xmm3 = z1, z2, w1, w2
		movhlps xmm2, xmm1		// xmm2 = x3, x4, y3, y4
		movlhps xmm1, xmm6		// xmm1 = x1, x2, y1, y2
		
		movaps	xmm6, xmm2		// xmm6 = x3, x4, y3, y4
		movaps	xmm5, xmm1		// xmm5 = x1, x2, y1, y2

		shufps  xmm2, xmm7, 0xDD	// xmm2 = x4, y4, z4, w4 // xmm2 = x3, x4, y3, y4,  xmm7 = z3, z4, w3, w4 
		shufps	xmm1, xmm3, 0x88	// xmm1 = x1, y1, z1, w1 // xmm1 = x1, x2, y1, y2,  xmm3 = z1, z2, w1, w2,
		shufps	xmm5, xmm3, 0xDD	// xmm5 = x2, y2, z2, w2 // xmm5 = x1, x2, y1, y2,  xmm3 = z1, z2, w1, w2
		shufps	xmm6, xmm7, 0x88	// xmm6 = x3, y3, z3, w3 // xmm6 = x3, x4, y3, y4,  xmm7 = z3, z3, w3, w4
		movaps  [eax],	  xmm1
		movaps  [eax+16], xmm5
		movaps	[eax+32], xmm6
		movaps  [eax+48], xmm2
		add		ebx,	16
		add		edx,	16
		add		edi,	16
		add		esi,	16
		add		eax,	64
		loop	LOOP1
	}
}


void SSE_SetZero(Vertex_sop* sop, const int size)
{
	int Asize = size/4;

	// __declspec(align(32)) float temp[4] = { 0.0f, 0.0f, 2.0f, 3.0f };
	 // 上記の文を入れると
	 //  warning C4731: 'SSE_SetZero' : インライン アセンブラ コードにより変更されたフレーム ポインタ レジスタ 'ebx' です。
	 // とか２回もいわれて、なんかうるさい。意味が判らんし。
	__asm{
		movaps  xmm0, [SSE_CONST::_0_0]; // アライメントが上手くいってないとコケまくるようだ。ぐれそうになる。
		mov		eax, sop;
		mov		ecx, [Asize];
		mov		edi, [eax].pcolor;
		mov		esi, [eax].pz;
		mov		edx, [eax].py;
		mov		ebx, [eax].px;	
LOOP1:
		movaps	[ebx], xmm0;
		movaps	[edx], xmm0;
		movaps  [esi], xmm0;
		movaps	[edi], xmm0;
		add		ebx,	16
		add		edx,	16
		add		edi,	16
		add		esi,	16
		loop	LOOP1
	}

}
