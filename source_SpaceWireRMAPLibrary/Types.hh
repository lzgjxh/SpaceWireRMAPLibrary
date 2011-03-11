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
typedef char				B;			/*	�����t��8�r�b�g����			*/
typedef short				H;			/*	�����t��16�r�b�g����		*/
typedef long				W;			/*	�����t��32�r�b�g����		*/
typedef unsigned char		UB;			/*	�����Ȃ�8�r�b�g����			*/
typedef unsigned short		UH;			/*	�����Ȃ�16�r�b�g����		*/
typedef unsigned long		UW;			/*	�����Ȃ�32�r�b�g����		*/
typedef char				VB;			/*	�f�[�^�^�C�v�Ȃ�8�r�b�g		*/
typedef short				VH;			/*	�f�[�^�^�C�v�Ȃ�16�r�b�g	*/
typedef long				VW;			/*	�f�[�^�^�C�v�Ȃ�32�r�b�g	*/
typedef void				*VP;		/*	�f�[�^�^�C�v�Ȃ��|�C���^	*/
typedef void				(*FP)();	/*	�v���O�����X�^�[�g�A�h���X	*/
typedef int					INT;		/*	�����t�������v���Z�b�T�ˑ�	*/
typedef unsigned int		UINT;		/*	�����Ȃ������v���Z�b�T�ˑ�	*/

typedef H					ID;			/*	�I�u�W�F�N�gID				*/
typedef H					HNO;		/*	�n���h���ԍ�				*/
typedef W					ER;			/*	�����t���G���[�R�[�h		*/
typedef W					TMO;		/*	�^�C���A�E�g				*/
typedef H					FN;			/*	�@�\�R�[�h					*/

#ifndef  TRUE
#ifndef  WIN32
typedef H					BOOL;		/*	�u�[���l FALSE/TRUE			*/
#endif
#define TRUE				(0 == 0)
#define FALSE				(!TRUE)
#endif

#endif  

#endif 
 
#endif   /* __TYPES_HH */ 
