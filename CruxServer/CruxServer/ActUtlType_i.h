

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ActUtlType_i_h__
#define __ActUtlType_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IActUtlType_FWD_DEFINED__
#define __IActUtlType_FWD_DEFINED__
typedef interface IActUtlType IActUtlType;
#endif 	/* __IActUtlType_FWD_DEFINED__ */


#ifndef __IActMLUtlType_FWD_DEFINED__
#define __IActMLUtlType_FWD_DEFINED__
typedef interface IActMLUtlType IActMLUtlType;
#endif 	/* __IActMLUtlType_FWD_DEFINED__ */


#ifndef ___IActUtlTypeEvents_FWD_DEFINED__
#define ___IActUtlTypeEvents_FWD_DEFINED__
typedef interface _IActUtlTypeEvents _IActUtlTypeEvents;
#endif 	/* ___IActUtlTypeEvents_FWD_DEFINED__ */


#ifndef __ActUtlType_FWD_DEFINED__
#define __ActUtlType_FWD_DEFINED__

#ifdef __cplusplus
typedef class ActUtlType ActUtlType;
#else
typedef struct ActUtlType ActUtlType;
#endif /* __cplusplus */

#endif 	/* __ActUtlType_FWD_DEFINED__ */


#ifndef ___IActMLUtlTypeEvents_FWD_DEFINED__
#define ___IActMLUtlTypeEvents_FWD_DEFINED__
typedef interface _IActMLUtlTypeEvents _IActMLUtlTypeEvents;
#endif 	/* ___IActMLUtlTypeEvents_FWD_DEFINED__ */


#ifndef __ActMLUtlType_FWD_DEFINED__
#define __ActMLUtlType_FWD_DEFINED__

#ifdef __cplusplus
typedef class ActMLUtlType ActMLUtlType;
#else
typedef struct ActMLUtlType ActMLUtlType;
#endif /* __cplusplus */

#endif 	/* __ActMLUtlType_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IActUtlType_INTERFACE_DEFINED__
#define __IActUtlType_INTERFACE_DEFINED__

/* interface IActUtlType */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IActUtlType;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F4A6601A-130D-4FEE-B4CD-7FACAE1011C6")
    IActUtlType : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActLogicalStationNumber( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ActLogicalStationNumber( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( 
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCpuType( 
            /* [string][out] */ BSTR *szCpuName,
            /* [out] */ LONG *lplCpuCode,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCpuStatus( 
            /* [in] */ LONG lOperation,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadDeviceBlock( 
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteDeviceBlock( 
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadDeviceRandom( 
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteDeviceRandom( 
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadBuffer( 
            /* [in] */ LONG lStartIO,
            /* [in] */ LONG lAddress,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteBuffer( 
            /* [in] */ LONG lStartIO,
            /* [in] */ LONG lAddress,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetClockData( 
            /* [out] */ SHORT *lpsYear,
            /* [out] */ SHORT *lpsMonth,
            /* [out] */ SHORT *lpsDay,
            /* [out] */ SHORT *lpsDayOfWeek,
            /* [out] */ SHORT *lpsHour,
            /* [out] */ SHORT *lpsMinute,
            /* [out] */ SHORT *lpsSecond,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetClockData( 
            /* [in] */ SHORT sYear,
            /* [in] */ SHORT sMonth,
            /* [in] */ SHORT sDay,
            /* [in] */ SHORT sDayOfWeek,
            /* [in] */ SHORT sHour,
            /* [in] */ SHORT sMinute,
            /* [in] */ SHORT sSecond,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDevice( 
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDevice( 
            /* [string][in] */ BSTR szDevice,
            /* [out] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckDeviceString( 
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lCheckType,
            /* [in] */ LONG lSize,
            /* [out] */ LONG *lplDeviceType,
            /* [string][out] */ BSTR *lpszDeviceName,
            /* [out] */ LONG *lplDeviceNumber,
            /* [out] */ LONG *lplDeviceRadix,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EntryDeviceStatus( 
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [in] */ LONG lMonitorCycle,
            /* [size_is][in] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FreeDeviceStatus( 
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActPassword( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ActPassword( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadDeviceBlock2( 
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteDeviceBlock2( 
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadDeviceRandom2( 
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteDeviceRandom2( 
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDevice2( 
            /* [string][in] */ BSTR szDevice,
            /* [out] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDevice2( 
            /* [in] */ BSTR szDevice,
            /* [in] */ SHORT sData,
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Connect( 
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Disconnect( 
            /* [retval][out] */ LONG *lplReturnCode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActUtlTypeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActUtlType * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActUtlType * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActUtlType * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActUtlType * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActUtlType * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActUtlType * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActUtlType * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ActLogicalStationNumber )( 
            IActUtlType * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ActLogicalStationNumber )( 
            IActUtlType * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IActUtlType * This,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IActUtlType * This,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCpuType )( 
            IActUtlType * This,
            /* [string][out] */ BSTR *szCpuName,
            /* [out] */ LONG *lplCpuCode,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCpuStatus )( 
            IActUtlType * This,
            /* [in] */ LONG lOperation,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadDeviceBlock )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteDeviceBlock )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadDeviceRandom )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteDeviceRandom )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadBuffer )( 
            IActUtlType * This,
            /* [in] */ LONG lStartIO,
            /* [in] */ LONG lAddress,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteBuffer )( 
            IActUtlType * This,
            /* [in] */ LONG lStartIO,
            /* [in] */ LONG lAddress,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetClockData )( 
            IActUtlType * This,
            /* [out] */ SHORT *lpsYear,
            /* [out] */ SHORT *lpsMonth,
            /* [out] */ SHORT *lpsDay,
            /* [out] */ SHORT *lpsDayOfWeek,
            /* [out] */ SHORT *lpsHour,
            /* [out] */ SHORT *lpsMinute,
            /* [out] */ SHORT *lpsSecond,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetClockData )( 
            IActUtlType * This,
            /* [in] */ SHORT sYear,
            /* [in] */ SHORT sMonth,
            /* [in] */ SHORT sDay,
            /* [in] */ SHORT sDayOfWeek,
            /* [in] */ SHORT sHour,
            /* [in] */ SHORT sMinute,
            /* [in] */ SHORT sSecond,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDevice )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDevice )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDevice,
            /* [out] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckDeviceString )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lCheckType,
            /* [in] */ LONG lSize,
            /* [out] */ LONG *lplDeviceType,
            /* [string][out] */ BSTR *lpszDeviceName,
            /* [out] */ LONG *lplDeviceNumber,
            /* [out] */ LONG *lplDeviceRadix,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EntryDeviceStatus )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [in] */ LONG lMonitorCycle,
            /* [size_is][in] */ LONG *lplData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FreeDeviceStatus )( 
            IActUtlType * This,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ActPassword )( 
            IActUtlType * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ActPassword )( 
            IActUtlType * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadDeviceBlock2 )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteDeviceBlock2 )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDevice,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadDeviceRandom2 )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [size_is][out] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteDeviceRandom2 )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDeviceList,
            /* [in] */ LONG lSize,
            /* [size_is][in] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDevice2 )( 
            IActUtlType * This,
            /* [string][in] */ BSTR szDevice,
            /* [out] */ SHORT *lpsData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDevice2 )( 
            IActUtlType * This,
            /* [in] */ BSTR szDevice,
            /* [in] */ SHORT sData,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Connect )( 
            IActUtlType * This,
            /* [retval][out] */ LONG *lplReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            IActUtlType * This,
            /* [retval][out] */ LONG *lplReturnCode);
        
        END_INTERFACE
    } IActUtlTypeVtbl;

    interface IActUtlType
    {
        CONST_VTBL struct IActUtlTypeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActUtlType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IActUtlType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IActUtlType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IActUtlType_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IActUtlType_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IActUtlType_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IActUtlType_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IActUtlType_get_ActLogicalStationNumber(This,pVal)	\
    ( (This)->lpVtbl -> get_ActLogicalStationNumber(This,pVal) ) 

#define IActUtlType_put_ActLogicalStationNumber(This,newVal)	\
    ( (This)->lpVtbl -> put_ActLogicalStationNumber(This,newVal) ) 

#define IActUtlType_Open(This,lplReturnCode)	\
    ( (This)->lpVtbl -> Open(This,lplReturnCode) ) 

#define IActUtlType_Close(This,lplReturnCode)	\
    ( (This)->lpVtbl -> Close(This,lplReturnCode) ) 

#define IActUtlType_GetCpuType(This,szCpuName,lplCpuCode,lplReturnCode)	\
    ( (This)->lpVtbl -> GetCpuType(This,szCpuName,lplCpuCode,lplReturnCode) ) 

#define IActUtlType_SetCpuStatus(This,lOperation,lplReturnCode)	\
    ( (This)->lpVtbl -> SetCpuStatus(This,lOperation,lplReturnCode) ) 

#define IActUtlType_ReadDeviceBlock(This,szDevice,lSize,lplData,lplReturnCode)	\
    ( (This)->lpVtbl -> ReadDeviceBlock(This,szDevice,lSize,lplData,lplReturnCode) ) 

#define IActUtlType_WriteDeviceBlock(This,szDevice,lSize,lplData,lplReturnCode)	\
    ( (This)->lpVtbl -> WriteDeviceBlock(This,szDevice,lSize,lplData,lplReturnCode) ) 

#define IActUtlType_ReadDeviceRandom(This,szDeviceList,lSize,lplData,lplReturnCode)	\
    ( (This)->lpVtbl -> ReadDeviceRandom(This,szDeviceList,lSize,lplData,lplReturnCode) ) 

#define IActUtlType_WriteDeviceRandom(This,szDeviceList,lSize,lplData,lplReturnCode)	\
    ( (This)->lpVtbl -> WriteDeviceRandom(This,szDeviceList,lSize,lplData,lplReturnCode) ) 

#define IActUtlType_ReadBuffer(This,lStartIO,lAddress,lSize,lpsData,lplReturnCode)	\
    ( (This)->lpVtbl -> ReadBuffer(This,lStartIO,lAddress,lSize,lpsData,lplReturnCode) ) 

#define IActUtlType_WriteBuffer(This,lStartIO,lAddress,lSize,lpsData,lplReturnCode)	\
    ( (This)->lpVtbl -> WriteBuffer(This,lStartIO,lAddress,lSize,lpsData,lplReturnCode) ) 

#define IActUtlType_GetClockData(This,lpsYear,lpsMonth,lpsDay,lpsDayOfWeek,lpsHour,lpsMinute,lpsSecond,lplReturnCode)	\
    ( (This)->lpVtbl -> GetClockData(This,lpsYear,lpsMonth,lpsDay,lpsDayOfWeek,lpsHour,lpsMinute,lpsSecond,lplReturnCode) ) 

#define IActUtlType_SetClockData(This,sYear,sMonth,sDay,sDayOfWeek,sHour,sMinute,sSecond,lplReturnCode)	\
    ( (This)->lpVtbl -> SetClockData(This,sYear,sMonth,sDay,sDayOfWeek,sHour,sMinute,sSecond,lplReturnCode) ) 

#define IActUtlType_SetDevice(This,szDevice,lData,lplReturnCode)	\
    ( (This)->lpVtbl -> SetDevice(This,szDevice,lData,lplReturnCode) ) 

#define IActUtlType_GetDevice(This,szDevice,lplData,lplReturnCode)	\
    ( (This)->lpVtbl -> GetDevice(This,szDevice,lplData,lplReturnCode) ) 

#define IActUtlType_CheckDeviceString(This,szDevice,lCheckType,lSize,lplDeviceType,lpszDeviceName,lplDeviceNumber,lplDeviceRadix,lplReturnCode)	\
    ( (This)->lpVtbl -> CheckDeviceString(This,szDevice,lCheckType,lSize,lplDeviceType,lpszDeviceName,lplDeviceNumber,lplDeviceRadix,lplReturnCode) ) 

#define IActUtlType_EntryDeviceStatus(This,szDeviceList,lSize,lMonitorCycle,lplData,lplReturnCode)	\
    ( (This)->lpVtbl -> EntryDeviceStatus(This,szDeviceList,lSize,lMonitorCycle,lplData,lplReturnCode) ) 

#define IActUtlType_FreeDeviceStatus(This,lplReturnCode)	\
    ( (This)->lpVtbl -> FreeDeviceStatus(This,lplReturnCode) ) 

#define IActUtlType_get_ActPassword(This,pVal)	\
    ( (This)->lpVtbl -> get_ActPassword(This,pVal) ) 

#define IActUtlType_put_ActPassword(This,newVal)	\
    ( (This)->lpVtbl -> put_ActPassword(This,newVal) ) 

#define IActUtlType_ReadDeviceBlock2(This,szDevice,lSize,lpsData,lplReturnCode)	\
    ( (This)->lpVtbl -> ReadDeviceBlock2(This,szDevice,lSize,lpsData,lplReturnCode) ) 

#define IActUtlType_WriteDeviceBlock2(This,szDevice,lSize,lpsData,lplReturnCode)	\
    ( (This)->lpVtbl -> WriteDeviceBlock2(This,szDevice,lSize,lpsData,lplReturnCode) ) 

#define IActUtlType_ReadDeviceRandom2(This,szDeviceList,lSize,lpsData,lplReturnCode)	\
    ( (This)->lpVtbl -> ReadDeviceRandom2(This,szDeviceList,lSize,lpsData,lplReturnCode) ) 

#define IActUtlType_WriteDeviceRandom2(This,szDeviceList,lSize,lpsData,lplReturnCode)	\
    ( (This)->lpVtbl -> WriteDeviceRandom2(This,szDeviceList,lSize,lpsData,lplReturnCode) ) 

#define IActUtlType_GetDevice2(This,szDevice,lpsData,lplReturnCode)	\
    ( (This)->lpVtbl -> GetDevice2(This,szDevice,lpsData,lplReturnCode) ) 

#define IActUtlType_SetDevice2(This,szDevice,sData,lplReturnCode)	\
    ( (This)->lpVtbl -> SetDevice2(This,szDevice,sData,lplReturnCode) ) 

#define IActUtlType_Connect(This,lplReturnCode)	\
    ( (This)->lpVtbl -> Connect(This,lplReturnCode) ) 

#define IActUtlType_Disconnect(This,lplReturnCode)	\
    ( (This)->lpVtbl -> Disconnect(This,lplReturnCode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IActUtlType_INTERFACE_DEFINED__ */


#ifndef __IActMLUtlType_INTERFACE_DEFINED__
#define __IActMLUtlType_INTERFACE_DEFINED__

/* interface IActMLUtlType */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IActMLUtlType;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B05A87E9-21CB-47A1-84A0-82ABACF879CD")
    IActMLUtlType : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( 
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDevice( 
            /* [in] */ VARIANT varDevice,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDevice( 
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadDeviceBlock( 
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteDeviceBlock( 
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadDeviceRandom( 
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteDeviceRandom( 
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadBuffer( 
            /* [in] */ VARIANT varStartIO,
            /* [in] */ VARIANT varAddress,
            /* [in] */ VARIANT varReadSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteBuffer( 
            /* [in] */ VARIANT varStartIO,
            /* [in] */ VARIANT varAddress,
            /* [in] */ VARIANT varWriteSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCpuType( 
            /* [out] */ VARIANT *lpvarCpuName,
            /* [out] */ VARIANT *lpvarCpuCode,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCpuStatus( 
            /* [in] */ VARIANT varOperation,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetClockData( 
            /* [out] */ VARIANT *lpvarYear,
            /* [out] */ VARIANT *lpvarMonth,
            /* [out] */ VARIANT *lpvarDay,
            /* [out] */ VARIANT *lpvarDayOfWeek,
            /* [out] */ VARIANT *lpvarHour,
            /* [out] */ VARIANT *lpvarMinute,
            /* [out] */ VARIANT *lpvarSecond,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetClockData( 
            /* [in] */ VARIANT varYear,
            /* [in] */ VARIANT varMonth,
            /* [in] */ VARIANT varDay,
            /* [in] */ VARIANT varDayOfWeek,
            /* [in] */ VARIANT varHour,
            /* [in] */ VARIANT varMinute,
            /* [in] */ VARIANT varSecond,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EntryDeviceStatus( 
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varMonitorCycle,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActLogicalStationNumber( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ActLogicalStationNumber( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FreeDeviceStatus( 
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ActPassword( 
            /* [retval][out] */ VARIANT *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ActPassword( 
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadDeviceBlock2( 
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteDeviceBlock2( 
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadDeviceRandom2( 
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteDeviceRandom2( 
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDevice2( 
            /* [in] */ VARIANT varDevice,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDevice2( 
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActMLUtlTypeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActMLUtlType * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActMLUtlType * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActMLUtlType * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IActMLUtlType * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IActMLUtlType * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IActMLUtlType * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IActMLUtlType * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IActMLUtlType * This,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IActMLUtlType * This,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDevice )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDevice,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDevice )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadDeviceBlock )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteDeviceBlock )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadDeviceRandom )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteDeviceRandom )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadBuffer )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varStartIO,
            /* [in] */ VARIANT varAddress,
            /* [in] */ VARIANT varReadSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteBuffer )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varStartIO,
            /* [in] */ VARIANT varAddress,
            /* [in] */ VARIANT varWriteSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCpuType )( 
            IActMLUtlType * This,
            /* [out] */ VARIANT *lpvarCpuName,
            /* [out] */ VARIANT *lpvarCpuCode,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCpuStatus )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varOperation,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetClockData )( 
            IActMLUtlType * This,
            /* [out] */ VARIANT *lpvarYear,
            /* [out] */ VARIANT *lpvarMonth,
            /* [out] */ VARIANT *lpvarDay,
            /* [out] */ VARIANT *lpvarDayOfWeek,
            /* [out] */ VARIANT *lpvarHour,
            /* [out] */ VARIANT *lpvarMinute,
            /* [out] */ VARIANT *lpvarSecond,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetClockData )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varYear,
            /* [in] */ VARIANT varMonth,
            /* [in] */ VARIANT varDay,
            /* [in] */ VARIANT varDayOfWeek,
            /* [in] */ VARIANT varHour,
            /* [in] */ VARIANT varMinute,
            /* [in] */ VARIANT varSecond,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EntryDeviceStatus )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varMonitorCycle,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ActLogicalStationNumber )( 
            IActMLUtlType * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ActLogicalStationNumber )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FreeDeviceStatus )( 
            IActMLUtlType * This,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ActPassword )( 
            IActMLUtlType * This,
            /* [retval][out] */ VARIANT *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ActPassword )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadDeviceBlock2 )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteDeviceBlock2 )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadDeviceRandom2 )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteDeviceRandom2 )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDeviceList,
            /* [in] */ VARIANT varSize,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDevice2 )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDevice,
            /* [out] */ VARIANT *lpvarData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDevice2 )( 
            IActMLUtlType * This,
            /* [in] */ VARIANT varDevice,
            /* [in] */ VARIANT varData,
            /* [retval][out] */ VARIANT *lpvarReturnCode);
        
        END_INTERFACE
    } IActMLUtlTypeVtbl;

    interface IActMLUtlType
    {
        CONST_VTBL struct IActMLUtlTypeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActMLUtlType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IActMLUtlType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IActMLUtlType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IActMLUtlType_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IActMLUtlType_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IActMLUtlType_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IActMLUtlType_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IActMLUtlType_Open(This,lpvarReturnCode)	\
    ( (This)->lpVtbl -> Open(This,lpvarReturnCode) ) 

#define IActMLUtlType_Close(This,lpvarReturnCode)	\
    ( (This)->lpVtbl -> Close(This,lpvarReturnCode) ) 

#define IActMLUtlType_GetDevice(This,varDevice,lpvarData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> GetDevice(This,varDevice,lpvarData,lpvarReturnCode) ) 

#define IActMLUtlType_SetDevice(This,varDevice,varData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> SetDevice(This,varDevice,varData,lpvarReturnCode) ) 

#define IActMLUtlType_ReadDeviceBlock(This,varDevice,varSize,lpvarData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> ReadDeviceBlock(This,varDevice,varSize,lpvarData,lpvarReturnCode) ) 

#define IActMLUtlType_WriteDeviceBlock(This,varDevice,varSize,varData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> WriteDeviceBlock(This,varDevice,varSize,varData,lpvarReturnCode) ) 

#define IActMLUtlType_ReadDeviceRandom(This,varDeviceList,varSize,lpvarData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> ReadDeviceRandom(This,varDeviceList,varSize,lpvarData,lpvarReturnCode) ) 

#define IActMLUtlType_WriteDeviceRandom(This,varDeviceList,varSize,varData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> WriteDeviceRandom(This,varDeviceList,varSize,varData,lpvarReturnCode) ) 

#define IActMLUtlType_ReadBuffer(This,varStartIO,varAddress,varReadSize,lpvarData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> ReadBuffer(This,varStartIO,varAddress,varReadSize,lpvarData,lpvarReturnCode) ) 

#define IActMLUtlType_WriteBuffer(This,varStartIO,varAddress,varWriteSize,varData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> WriteBuffer(This,varStartIO,varAddress,varWriteSize,varData,lpvarReturnCode) ) 

#define IActMLUtlType_GetCpuType(This,lpvarCpuName,lpvarCpuCode,lpvarReturnCode)	\
    ( (This)->lpVtbl -> GetCpuType(This,lpvarCpuName,lpvarCpuCode,lpvarReturnCode) ) 

#define IActMLUtlType_SetCpuStatus(This,varOperation,lpvarReturnCode)	\
    ( (This)->lpVtbl -> SetCpuStatus(This,varOperation,lpvarReturnCode) ) 

#define IActMLUtlType_GetClockData(This,lpvarYear,lpvarMonth,lpvarDay,lpvarDayOfWeek,lpvarHour,lpvarMinute,lpvarSecond,lpvarReturnCode)	\
    ( (This)->lpVtbl -> GetClockData(This,lpvarYear,lpvarMonth,lpvarDay,lpvarDayOfWeek,lpvarHour,lpvarMinute,lpvarSecond,lpvarReturnCode) ) 

#define IActMLUtlType_SetClockData(This,varYear,varMonth,varDay,varDayOfWeek,varHour,varMinute,varSecond,lpvarReturnCode)	\
    ( (This)->lpVtbl -> SetClockData(This,varYear,varMonth,varDay,varDayOfWeek,varHour,varMinute,varSecond,lpvarReturnCode) ) 

#define IActMLUtlType_EntryDeviceStatus(This,varDeviceList,varSize,varMonitorCycle,varData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> EntryDeviceStatus(This,varDeviceList,varSize,varMonitorCycle,varData,lpvarReturnCode) ) 

#define IActMLUtlType_get_ActLogicalStationNumber(This,pVal)	\
    ( (This)->lpVtbl -> get_ActLogicalStationNumber(This,pVal) ) 

#define IActMLUtlType_put_ActLogicalStationNumber(This,newVal)	\
    ( (This)->lpVtbl -> put_ActLogicalStationNumber(This,newVal) ) 

#define IActMLUtlType_FreeDeviceStatus(This,lpvarReturnCode)	\
    ( (This)->lpVtbl -> FreeDeviceStatus(This,lpvarReturnCode) ) 

#define IActMLUtlType_get_ActPassword(This,pVal)	\
    ( (This)->lpVtbl -> get_ActPassword(This,pVal) ) 

#define IActMLUtlType_put_ActPassword(This,newVal)	\
    ( (This)->lpVtbl -> put_ActPassword(This,newVal) ) 

#define IActMLUtlType_ReadDeviceBlock2(This,varDevice,varSize,lpvarData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> ReadDeviceBlock2(This,varDevice,varSize,lpvarData,lpvarReturnCode) ) 

#define IActMLUtlType_WriteDeviceBlock2(This,varDevice,varSize,varData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> WriteDeviceBlock2(This,varDevice,varSize,varData,lpvarReturnCode) ) 

#define IActMLUtlType_ReadDeviceRandom2(This,varDeviceList,varSize,lpvarData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> ReadDeviceRandom2(This,varDeviceList,varSize,lpvarData,lpvarReturnCode) ) 

#define IActMLUtlType_WriteDeviceRandom2(This,varDeviceList,varSize,varData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> WriteDeviceRandom2(This,varDeviceList,varSize,varData,lpvarReturnCode) ) 

#define IActMLUtlType_GetDevice2(This,varDevice,lpvarData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> GetDevice2(This,varDevice,lpvarData,lpvarReturnCode) ) 

#define IActMLUtlType_SetDevice2(This,varDevice,varData,lpvarReturnCode)	\
    ( (This)->lpVtbl -> SetDevice2(This,varDevice,varData,lpvarReturnCode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IActMLUtlType_INTERFACE_DEFINED__ */



#ifndef __ActUtlTypeLib_LIBRARY_DEFINED__
#define __ActUtlTypeLib_LIBRARY_DEFINED__

/* library ActUtlTypeLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ActUtlTypeLib;

#ifndef ___IActUtlTypeEvents_DISPINTERFACE_DEFINED__
#define ___IActUtlTypeEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IActUtlTypeEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IActUtlTypeEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0D40C842-E0AE-40FE-BB36-894B27D170BF")
    _IActUtlTypeEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IActUtlTypeEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IActUtlTypeEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IActUtlTypeEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IActUtlTypeEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IActUtlTypeEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IActUtlTypeEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IActUtlTypeEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IActUtlTypeEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IActUtlTypeEventsVtbl;

    interface _IActUtlTypeEvents
    {
        CONST_VTBL struct _IActUtlTypeEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IActUtlTypeEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IActUtlTypeEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IActUtlTypeEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IActUtlTypeEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IActUtlTypeEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IActUtlTypeEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IActUtlTypeEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IActUtlTypeEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ActUtlType;

#ifdef __cplusplus

class DECLSPEC_UUID("63885648-1785-41A4-82D5-C578D29E4DA8")
ActUtlType;
#endif

#ifndef ___IActMLUtlTypeEvents_DISPINTERFACE_DEFINED__
#define ___IActMLUtlTypeEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IActMLUtlTypeEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IActMLUtlTypeEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EC0956E5-A882-4855-87A9-CA571090F36E")
    _IActMLUtlTypeEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IActMLUtlTypeEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IActMLUtlTypeEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IActMLUtlTypeEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IActMLUtlTypeEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IActMLUtlTypeEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IActMLUtlTypeEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IActMLUtlTypeEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IActMLUtlTypeEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IActMLUtlTypeEventsVtbl;

    interface _IActMLUtlTypeEvents
    {
        CONST_VTBL struct _IActMLUtlTypeEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IActMLUtlTypeEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _IActMLUtlTypeEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _IActMLUtlTypeEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _IActMLUtlTypeEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _IActMLUtlTypeEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _IActMLUtlTypeEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _IActMLUtlTypeEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IActMLUtlTypeEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ActMLUtlType;

#ifdef __cplusplus

class DECLSPEC_UUID("29DE4B06-C429-420A-B3B7-BF7DC62649B2")
ActMLUtlType;
#endif
#endif /* __ActUtlTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


