/**
 *  file    InterruptHandler.c
 *  date    2009/01/24
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ���ͷ�Ʈ �� ���� �ڵ鷯�� ���õ� �ҽ� ����
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Console.h"

/**
 *  �������� ����ϴ� ���� �ڵ鷯
 */
static inline void invlpg(void* m);
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };

    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;
    
    kPrintStringXY( 0, 0, "====================================================" );
    kPrintStringXY( 0, 1, "                 Exception Occur~!!!!               " );
    kPrintStringXY( 0, 2, "                    Vector:                         " );
    kPrintStringXY( 27, 2, vcBuffer );
    kPrintStringXY( 0, 3, "====================================================" );

    while( 1 ) ;
}


void kPagefaultHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char ecBuffer[ 5 ] = { 0, };
    ecBuffer[ 0 ] = '0' + (qwErrorCode>>32) / 10;
    ecBuffer[ 1 ] = '0' + (qwErrorCode>>32) % 10;
    ecBuffer[ 2 ] = '0' + qwErrorCode / 10;
    ecBuffer[ 3 ] = '0' + qwErrorCode % 10;

	if(!(qwErrorCode & 0x01)){					// is page fault?
		DWORD entryIndex;
		QWORD CR2, CR3;

		__asm__ __volatile__(
			"mov %%cr2, %%rax \n\t"
			"mov %%rax, %0 \n\t"
			"mov %%cr3, %%rax \n\t"
			"mov %%rax, %1 \n\t"
		: "=m" (CR2), "=m" (CR3));			// CR2: fault addr(0x1ff000), CR3: P

		entryIndex = (CR2 >> 12);

		if(entryIndex > 511){
    		kPrintStringXY( 32, 4, "Entry index bound error");
			while(1);
		}

		QWORD pt_index, pd_index, pdpt_index, pml4_index;
		QWORD *ptr;
		pt_index = (CR2 >> 12) & 0x1ff;
		pd_index = (CR2 >> 21) & 0x1ff;
		pdpt_index = (CR2 >> 30) & 0x1ff;
		pml4_index = (CR2 >> 39) & 0x1ff;

		ptr = (QWORD *)CR3 + pml4_index;
		
		if(((*ptr) & 0x01) ^ 0x01){
			*ptr = (*ptr) ^ 0x01;
		}

		ptr = (QWORD *)((*ptr)&0xFFFFFFF000) + pdpt_index;

		if(((*ptr) & 0x01) ^ 0x01){
			*ptr = (*ptr) ^ 0x01;
		}

		ptr = (QWORD *)(*ptr&0xFFFFFFF000) + pd_index;

		if(((*ptr) & 0x01) ^ 0x01){
			*ptr = (*ptr) ^ 0x01;
		}

		ptr = (QWORD *)(*ptr&0xFFFFFFF000) + pt_index;

		if(((*ptr) & 0x01) ^ 0x01){
			*ptr = (*ptr) ^ 0x01;
		}
		QWORD var = *ptr;

		ptr = (QWORD *)(*ptr&0xFFFFFFF000) + pt_index;

		invlpg(&CR2);
		/*
		__asm__ __volatile__(
			"mov %%cr2, %%rax \n\t"
			"mov %%rax, %0 \n\t"
			"mov %%cr3, %%rax \n\t"
			"mov %%rax, %1 \n\t"
		: "=m" (CR2), "=m" (CR3));			// CR2: fault addr(0x1ff000), CR3: P
		*/
	//	asm volatile ( "invlpg (%0)" : : "b"(CR2) : "memory" );			// <<<<<<<<<<<<<<<<<<< This point maybe need fixing(TLB invalidation)


    	kPrintf("====================================================\n");
		kPrintf("                 Page Fault Occurs~!                \n" );
    	kPrintf("                  Address: 0x%x                     \n", CR2);
    	kPrintf("====================================================\n");
	}
	else if((qwErrorCode & 0x02)>>1){					// is protection fault?
		DWORD entryIndex;
		QWORD CR2, CR3;

		__asm__ __volatile__(
			"mov %%cr2, %%rax \n\t"
			"mov %%rax, %0 \n\t"
			"mov %%cr3, %%rax \n\t"
			"mov %%rax, %1 \n\t"
		: "=m" (CR2), "=m" (CR3));			// CR2: fault addr(0x1ff000), CR3: P

		entryIndex = (CR2 >> 12);

		if(entryIndex > 511){
    		kPrintStringXY( 32, 4, "Entry index bound error");
			while(1);
		}

		QWORD pt_index, pd_index, pdpt_index, pml4_index;
		QWORD *ptr;
		pt_index = (CR2 >> 12) & 0x1ff;
		pd_index = (CR2 >> 21) & 0x1ff;
		pdpt_index = (CR2 >> 30) & 0x1ff;
		pml4_index = (CR2 >> 39) & 0x1ff;

		ptr = (QWORD *)CR3 + pml4_index;
		
		if(((*ptr) & 0x02) ^ 0x02){
			*ptr = (*ptr) ^ 0x02;
		}

		ptr = (QWORD *)((*ptr)&0xFFFFFFF000) + pdpt_index;

		if(((*ptr) & 0x02) ^ 0x02){
			*ptr = (*ptr) ^ 0x02;
		}

		ptr = (QWORD *)(*ptr&0xFFFFFFF000) + pd_index;

		if(((*ptr) & 0x02) ^ 0x02){
			*ptr = (*ptr) ^ 0x02;
		}

		ptr = (QWORD *)(*ptr&0xFFFFFFF000) + pt_index;

		if(((*ptr) & 0x02) ^ 0x02){
			*ptr = (*ptr) ^ 0x02;
		}
		QWORD var = *ptr;

		ptr = (QWORD *)(*ptr&0xFFFFFFF000) + pt_index;

		invlpg(&CR2);

    	kPrintf("====================================================\n");
		kPrintf("              Protection Fault Occurs~!             \n" );
    	kPrintf("                  Address: 0x%x                     \n", CR2);
    	kPrintf("====================================================\n");
	}
}

/**
 *  �������� ����ϴ� ���ͷ�Ʈ �ڵ鷯
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ������ ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    kPrintStringXY( 70, 0, vcBuffer );
    //=========================================================================
    
    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  Ű���� ���ͷ�Ʈ�� �ڵ鷯
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    //=========================================================================
    // ���ͷ�Ʈ�� �߻������� �˸����� �޽����� ����ϴ� �κ�
    // ���ͷ�Ʈ ���͸� ȭ�� ���� ���� 2�ڸ� ������ ���
    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    // �߻��� Ƚ�� ���
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 0, 0, vcBuffer );
    //=========================================================================

    // Ű���� ��Ʈ�ѷ����� �����͸� �о ASCII�� ��ȯ�Ͽ� ť�� ����
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // EOI ����
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

static inline void invlpg(void* m)
{
	asm volatile ( "invlpg (%0)" : : "b"(m) : "memory" );
}
