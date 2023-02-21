

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Aug 17 13:44:42 2012
 */
/* Compiler settings for ActUtlType.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IActUtlType,0xF4A6601A,0x130D,0x4FEE,0xB4,0xCD,0x7F,0xAC,0xAE,0x10,0x11,0xC6);


MIDL_DEFINE_GUID(IID, IID_IActMLUtlType,0xB05A87E9,0x21CB,0x47A1,0x84,0xA0,0x82,0xAB,0xAC,0xF8,0x79,0xCD);


MIDL_DEFINE_GUID(IID, LIBID_ActUtlTypeLib,0xD217E54E,0x4A26,0x4A76,0xB0,0xAB,0x57,0x16,0x6B,0x90,0xF9,0xAF);


MIDL_DEFINE_GUID(IID, DIID__IActUtlTypeEvents,0x0D40C842,0xE0AE,0x40FE,0xBB,0x36,0x89,0x4B,0x27,0xD1,0x70,0xBF);


MIDL_DEFINE_GUID(CLSID, CLSID_ActUtlType,0x63885648,0x1785,0x41A4,0x82,0xD5,0xC5,0x78,0xD2,0x9E,0x4D,0xA8);


MIDL_DEFINE_GUID(IID, DIID__IActMLUtlTypeEvents,0xEC0956E5,0xA882,0x4855,0x87,0xA9,0xCA,0x57,0x10,0x90,0xF3,0x6E);


MIDL_DEFINE_GUID(CLSID, CLSID_ActMLUtlType,0x29DE4B06,0xC429,0x420A,0xB3,0xB7,0xBF,0x7D,0xC6,0x26,0x49,0xB2);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



