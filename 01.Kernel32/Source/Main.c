/**
 *  file    Main.c
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   C ���� �ۼ��� Ŀ���� ��Ʈ�� ����Ʈ ����
 */

#include "Types.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );

void Main( void )
{
    kPrintString( 0, 4, "C Language Kernel Started~!!!" );

    kPrintString( 0, 5, "Minimum Memory Size Check...................[    ]" );
    if( kIsMemoryEnough() == FALSE )
    {
        kPrintString( 45, 5, "Fail" );
        kPrintString( 0, 6, "Not Enough Memory~!! MINT64 OS Requires Over "
                "64Mbyte Memory~!!" );
        while( 1 ) ;
    }
    else
    {
        kPrintString( 45, 5, "Pass" );
    }
    
    // IA-32e
    kPrintString( 0, 6, "IA-32e Kernel Area Initialize...............[    ]" );
    if( kInitializeKernel64Area() == FALSE )
    {
        kPrintString( 45, 6, "Fail" );
        kPrintString( 0, 7, "Kernel Area Initialization Fail~!!" );
        while( 1 ) ;
    }
    kPrintString( 45, 6, "Pass" );

    while( 1 ) ;
}

void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    pstScreen += ( iY * 80 ) + iX;
    
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}

BOOL kInitializeKernel64Area( void )
{
    DWORD* pdwCurrentAddress;
    
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    while( ( DWORD ) pdwCurrentAddress < 0x600000 )
    {        
        *pdwCurrentAddress = 0x00;

        if( *pdwCurrentAddress != 0 )
        {
            return FALSE;
        }
        
        pdwCurrentAddress++;
    }
    
    return TRUE;
}

/**
 *  MINT64 OS
 *      64Mbyte
 */
BOOL kIsMemoryEnough( void )
{
    DWORD* pdwCurrentAddress;
   
    // 0x100000(1MB)
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // 0x4000000(64MB)
    while( ( DWORD ) pdwCurrentAddress < 0x4000000 )
    {
        *pdwCurrentAddress = 0x12345678;
        
        // 0x12345678
        if( *pdwCurrentAddress != 0x12345678 )
        {
           return FALSE;
        }
        
        // 1MB
        pdwCurrentAddress += ( 0x100000 / 4 );
    }
    return TRUE;
}