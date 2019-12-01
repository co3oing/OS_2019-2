/**
 *  file    Main.c
 *  date    2009/01/02
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ���� �ۼ��� Ŀ���� ��Ʈ�� ����Ʈ ����
 */

#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"
#include "Task.h"
#include "PIT.h"
#include "DynamicMemory.h"
#include "HardDisk.h"
#include "FileSystem.h"

// �Լ� ����
void kPrintStringMapped( int iX, int iY, const char* pcString );
void kRWROTest();

/**
 *  �Ʒ� �Լ��� C ��� Ŀ���� ���� �κ���
 */
void Main( void )
{
    int iCursorX, iCursorY;

    // �ܼ��� ���� �ʱ�ȭ�� ��, ���� �۾��� ����
    kInitializeConsole( 0, 10 );    
    kPrintf( "Switch To IA-32e Mode Success~!!\n" );
    kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
    kPrintf( "Initialize Console..........................[Pass]\n" );
    kSetCursor( 45, iCursorY++ );
	iCursorY += 13;
    kPrintStringMapped( 0, 13, "This message is printed through the video memory relocated to 0xAB8000" );
    kSetCursor( 0, iCursorY++ );

    
    // ���� ��Ȳ�� ȭ�鿡 ���
    kGetCursor( &iCursorX, &iCursorY );
    kPrintf( "GDT Initialize And Switch For IA-32e Mode...[    ]" );
    kInitializeGDTTableAndTSS();
    kLoadGDTR( GDTR_STARTADDRESS );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );
    
    kPrintf( "TSS Segment Load............................[    ]" );
    kLoadTR( GDT_TSSSEGMENT );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );
    
    kPrintf( "IDT Initialize..............................[    ]" );
    kInitializeIDTTables();    
    kLoadIDTR( IDTR_STARTADDRESS );
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );
    
    kPrintf( "Total RAM Size Check........................[    ]" );
    kCheckTotalRAMSize();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass], Size = %d MB\n", kGetTotalRAMSize() );
    
    kPrintf( "TCB Pool And Scheduler Initialize...........[Pass]\n" );
    iCursorY++;
    kInitializeScheduler();
    
    // ���� �޸� �ʱ�ȭ
    kPrintf( "Dynamic Memory Initialize...................[Pass]\n" );
    iCursorY++;
    kInitializeDynamicMemory();

    // 1ms�� �ѹ��� ���ͷ�Ʈ�� �߻��ϵ��� ����
    kInitializePIT( MSTOCOUNT( 1 ), 1 );
    
    kPrintf( "Keyboard Activate And Queue Initialize......[    ]" );
    // Ű���带 Ȱ��ȭ
    if( kInitializeKeyboard() == TRUE )
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Pass\n" );
        kChangeKeyboardLED( FALSE, FALSE, FALSE );
    }
    else
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Fail\n" );
        while( 1 ) ;
    }
    
    kPrintf( "PIC Controller And Interrupt Initialize.....[    ]" );
    // PIC ��Ʈ�ѷ� �ʱ�ȭ �� ��� ���ͷ�Ʈ Ȱ��ȭ
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kSetCursor( 45, iCursorY++ );
    kPrintf( "Pass\n" );

    // ���� �ý����� �ʱ�ȭ
    kPrintf( "File System Initialize......................[    ]" );
    if( kInitializeFileSystem() == TRUE )
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Pass\n" );
    }
    else
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Fail\n" );
    }
    
    // ���� �½�ũ�� �ý��� ������� �����ϰ� ���� ����
    kCreateTask( TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM | TASK_FLAGS_IDLE, 0, 0, 
            ( QWORD ) kIdleTask );
    kStartConsoleShell();
}


void kPrintStringMapped( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xAB8000;
    int i;
    
    // X, Y ��� ���� ���� ��� ����� ��
    pstScreen += ( iY * 80 ) + iX;

    // NULL� �� ��� ��� ��
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

void kRWROTest(){

	DWORD **kernel1fe;
	DWORD **kernel1ff;
	kernel1fe = 0x1fe000;

	kPrintStringMapped( 0, 14, "Read from 0x1fe000 [  ]");

	if( *kernel1fe == 0x050011FE ){				// OS Life~~
		kPrintStringMapped( 20, 14, "OK");
		kPrintStringMapped( 0, 15, "Write to 0x1fe000 [  ]");
	}
	else{
		kPrintStringMapped( 20, 14, "FAIL]");
		while(1);
	}
	*kernel1fe = 0xDEADBEEF;
	kPrintStringMapped( 19, 15, "OK");
	kPrintStringMapped( 0, 16, "Read from 0x1ff000 [  ]");

	kernel1ff = 0x1ff000;
	if( *kernel1ff == 0xCAFEBABE ){
		kPrintStringMapped( 20, 16, "OK");
	}
	else{
		kPrintStringMapped( 20, 16, "FAIL]");
		while(1);
	}
	// write to read-only page(0x1ff000)
	/**	kPrintStringMapped( 0, 18, "Write to 0x1ff000 [  ]");
	 *	*kernel1ff = 0xDEADBEEF;
	 *	kPrintStringMapped( 19, 18, "OK");
	 */

	return;
}
