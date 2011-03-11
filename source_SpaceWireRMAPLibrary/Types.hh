//from http://www.sage-p.com/compone/

typedef bool boolean;
typedef void VOID;

#ifndef __TYPES_HH_  /* avoid for types.h define of borland */
#define __TYPES_HH_

#ifndef USES_PRIORITY_HEADER 
#define  USES_TYPES

#ifndef  TYPES_NOT_DEFINE_U_INT
typedef  unsigned char   u_char;
typedef  unsigned short  u_short;
typedef  unsigned int    u_int;
typedef  unsigned long   u_long;
#endif

 
#ifndef  NULL
 #ifdef __cplusplus
  #define   NULL  0
 #else
  #define   NULL  ((void*)0)
 #endif
#endif

#ifndef  TYPES_NOT_DEFINE_STATIC
#ifdef  NDEBUG
  #define  STATIC  static
#else
  #define  STATIC
#endif
#endif

#ifndef  TYPES_NOT_DEFINE_BOOL
#ifndef __cplusplus
  #define  TYPES_BOOL_CPP_NEED
#else
  #if _MSC_VER < 1100
    #define  TYPES_BOOL_CPP_NEED
  #endif
#endif

#ifndef __cplusplus
#undef  TYPES_BOOL_CPP_NEED
#endif

#define  Bool_chk(var)   ( (var)==true || (var)==false )
#endif  /* TYPES_NOT_DEFINE_BOOL */



#ifndef  TYPES_NOT_DEFINE_TYPE_OF_OS

/* uITRON DATA TYPE definition */
typedef char				B;			/*	符号付き8ビット整数			*/
typedef short				H;			/*	符号付き16ビット整数		*/
typedef long				W;			/*	符号付き32ビット整数		*/
typedef unsigned char		UB;			/*	符号なし8ビット整数			*/
typedef unsigned short		UH;			/*	符号なし16ビット整数		*/
typedef unsigned long		UW;			/*	符号なし32ビット整数		*/
typedef char				VB;			/*	データタイプなし8ビット		*/
typedef short				VH;			/*	データタイプなし16ビット	*/
typedef long				VW;			/*	データタイプなし32ビット	*/
typedef void				*VP;		/*	データタイプなしポインタ	*/
typedef void				(*FP)();	/*	プログラムスタートアドレス	*/
typedef int					INT;		/*	符号付き整数プロセッサ依存	*/
typedef unsigned int		UINT;		/*	符号なし整数プロセッサ依存	*/

typedef H					ID;			/*	オブジェクトID				*/
typedef H					HNO;		/*	ハンドラ番号				*/
typedef W					ER;			/*	符号付きエラーコード		*/
typedef W					TMO;		/*	タイムアウト				*/
typedef H					FN;			/*	機能コード					*/

#ifndef  TRUE
#ifndef  WIN32
typedef H					BOOL;		/*	ブール値 FALSE/TRUE			*/
#endif
#define TRUE				(0 == 0)
#define FALSE				(!TRUE)
#endif

#endif  

#endif 
 
#endif   /* __TYPES_HH */ 
