/**
 *  file    Main.c
 *  date    2009/01/02
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ���� �ۼ��� Ŀ���� ��Ʈ�� ����Ʈ ����
 */

#include "Types.h"
#include <string.h>

// �Լ� ����
void kPrintString( int iX, int iY, const char* pcString );
void kPrintStringMapped( int iX, int iY, const char* pcString );

/**
 *  �Ʒ� �Լ��� C ��� Ŀ���� ���� �κ���
 */
void Main( void )
{
    kPrintString( 0, 12, "Switch To IA-32e Mode Success~!!" );
    kPrintString( 0, 13, "IA-32e C Language Kernel Start..............[Pass]" );
    kPrintStringMapped( 0, 14, "This message is printed through the video memory relocated to 0xAB8000" );
	kPrintString( 0, 15, "Read from 0x1fe000 [  ]");

	DWORD **kernel1fe;
	DWORD **kernel1ff;
	kernel1fe = 0x1fe000;

	if( *kernel1fe == 0x050011FE ){				// OS Life~~
		kPrintString( 20, 15, "OK");
		kPrintString( 0, 16, "Write to 0x1fe000 [  ]");
	}
	else{
		kPrintString( 20, 15, "FAIL]");
		while(1);
	}
	*kernel1fe = 0xDEADBEEF;
	kPrintString( 19, 16, "OK");
	kPrintString( 0, 17, "Read from 0x1ff000 [  ]");

	kernel1ff = 0x1ff000;
	if( *kernel1ff == 0xCAFEBABE ){				// OS Life~~
		kPrintString( 20, 17, "OK");
		kPrintString( 0, 18, "Write to 0x1ff000 [  ]");
	}
	else{
		kPrintString( 22, 17, "FAIL]");
		while(1);
	}
	//*kernel1ff = 0xDEADBEEF;
	//kPrintString( 19, 18, "OK");



}

/**
 *  ���ڿ��� X, Y ��ġ�� ���
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    // X, Y ��ǥ�� �̿��ؼ� ���ڿ��� ����� ��巹���� ���
    pstScreen += ( iY * 80 ) + iX;

    // NULL�� ���� ������ ���ڿ� ���
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

void kPrintStringMapped( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y ��ǥ�� �̿��ؼ� ���ڿ��� ����� ��巹���� ���
    pstScreen += ( iY * 80 ) + iX;

    // NULL�� ���� ������ ���ڿ� ���
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}
