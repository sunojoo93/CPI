/** 
@addtogroup	UTILITY
@{
*/ 

/**
@file		SmartPointer.h
@brief		Smart Pointer 의 기능을 제공한다.

@remark		
-			Smart Pointer 는 기존 C/C++ 의 Pointer 개념을 대체 하기 위해 개발 되었다.
-			Smart Pointer 로 동적 생성된 객체는 자신을 더이상 참조 하는 곳이 없을 경우 자동 소멸한다.
@author		김용태
@date		2006
*/

#pragma once

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

/** 
@brief		Smart Pointer

@remark		
@author		김용태
@date		2006
*/

//_sp<int> ii = new int; 이런식으로 쓰면됨.. delete 필요 없음

template <class _T>
class _sp
{
private:
	/** 
	@brief		객체 참조 제어자

	@remark		동적 생성된 Template 객체의 주소를 기억하고 참조 Count 를 제어한다.
	@author		김용태
	@date		2006
	*/
	class obj
	{
	public:
		/** 
		@brief		생성자 1

		@return		없음
		@remark		생성된 Template 객체의 포인터를 기억하고 참조회수를 초기화 후 증가시킨다.
		@author		김용태
		@date		2006
		*/
		obj(
			_T* _p	/// 생성된 Template 객체
			)
		{
			ref = 0;
			p = _p;
			addref();
		}

		/** 
		@brief		소멸자

		@return		없음
		@remark		생성된 Template 객체가 더이상 참조되지 않는 다면 소멸시킨다.
		@author		김용태
		@date		2006
		*/
		~obj()
		{ 
			release(); 
		}

		/** 
		@brief		객체 참조 회수를 증가 시킨다.

		@return		없음
		@remark		
		@author		김용태
		@date		2006
		*/
		void addref()
		{ 
			ref++; 
		}

		/** 
		@brief		생성된 Template 객체의 주소를 반환한다.

		@return		Template 객체의 주소
		@remark		
		@author		김용태
		@date		2006
		*/
		_T* get()
		{ 
			return p; 
		}

		/** 
		@brief		생성된 Template 객체가 더이상 참조되지 않는 다면 소멸시킨다.

		@return		없음
		@remark		
		@author		김용태
		@date		2006
		*/
		void release()
		{ 
			if(--ref == 0) 
			{ 
				if(p) 
				{ 
					delete p;
					p = 0;
				}
				delete this;
			} 
		}
	
		_T*				p;		///< Template 객체 주소
		int				ref;	///< Template 객체 참조 회수
	};

	obj* _o;					///< 객체 참조 제어자

public:
	/** 
	@brief		생성자 1

	@return		없음
	@remark		객체 참조 제어자의 Pointer를 Null로 초기화 한다.
	@author		김용태
	@date		2006
	*/
	_sp()
	{ 
		_o = 0; 
	}

	/** 
	@brief		생성자 2

	@return		없음
	@remark		Template 객체 생성여부에 따라 객체를 생성하거나, 초기화 한다.
	@author		김용태
	@date		2006
	*/
	_sp(
		bool bNew			///< Template 객체를 새로 생성하는지 여부
		)
	{
		bNew ? _o = new obj(new _T) : _o = 0; 
	}  

	/** 
	@brief		생성자 3

	@return		없음
	@remark		Template 객체 생성여부에 따라 객체를 생성하거나, 초기화 한다.
	@author		김용태
	@date		2006
	*/
	_sp(
		int bNew			///< Template 객체를 새로 생성하는지 여부
		) 
	{ 
		bNew ? _o = new obj(new _T) : _o = 0; 
	}  

	/** 
	@brief		생성자 4

	@return		없음
	@remark		Template 객체 주소를 이용하여 객체 참조자를 생성하여 주소를 가지고 있는다.
	@author		김용태
	@date		2006
	*/
	_sp(
		_T* p				///< Template 객체 주소
		) 
	{ 
		_o = new obj(p); 
	}

	/** 
	@brief		생성자 5

	@return		없음
	@remark		Template 객체 주소를 이용하여 객체 참조자를 생성하여 주소를 가지고 있는다.
	@author		김용태
	@date		2006
	*/
	_sp(
		const _T*& p		///< Template 객체 주소
		) 
	{ 
		_o = new obj(p); 
	}


	/** 
	@brief		생성자 6

	@return		없음
	@remark		이미 생성된 Smart Pointer 를 통해 새로 생성되면 파라메터로 넘어온 객체 참조자의 참조 회수를 증가 시킨다.
	@author		김용태
	@date		2006
	*/
	_sp(
		const _sp<_T>& sp2	///< Smart Pointer
		)
	{ 
		_o = sp2._o; 
		if(_o)
		{
			_o->addref();
		}
	}

	/** 
	@brief		소멸자

	@return		없음
	@remark		Smart Pointer가 사용된 지역내에서 소멸시 참조 회수를 보고 소멸한다.
	@author		김용태
	@date		2006
	*/
	~_sp() 
	{ 
		if(_o) 
		{
			_o->release(); 
		}
	}

	/** 
	@brief		= Operator Overloading 1

	@return		Smart Pointer Reference
	@remark		Left Value(기존 Smart Pointer)에 새로운 Smart Pointer 가 대입 되면 기존 자료를 반환하고 새로운 Smart Pointer 를 대입한다.
	@author		김용태
	@date		2006
	*/
	_sp& operator = (
		const _sp& sp2		///< Left Value 에 대입될 Smart Pointer
		)
	{
		if(this == &sp2) 
		{
			return *this;
		}

		if(_o) 
		{
			_o->release(); 
		}

		_o = sp2._o; 

		if(_o)
		{
			_o->addref();
		}

		return *this;
	}

	/** 
	@brief		= Operator Overloading 2

	@return		Smart Pointer Reference
	@remark		Left Value(기존 Smart Pointer)에 새로운 Template 객체가 대입 되면 기존 자료를 반환하고 새로운 Template으로 Smart Pointer 객체를 생성하여 대입한다..
	@author		김용태
	@date		2006
	*/
	_sp& operator = (
		_T* p				///< Left Value 에 대입될 Template 객체
		)
	{
		if(_o) 
		{
			if(_o->p == p) 
			{
				return *this;
			}
		}

		if(_o) 
		{
			_o->release(); 
		}

		if(!p)
		{ 
			_o = 0; 
			return *this; 
		}

		_o = new obj(p);

		return *this;
	}

	/** 
	@brief		* Operator Overloading

	@return		Template객체 참조자 Reference
	@remark		Right Value 에 위치 하며 Smart pointer 에 붙는 * 연산은 현재 Smart pointer내 Template객체 참조자의 Reference 를 넘긴다.
	@author		김용태
	@date		2006
	*/
	_T& operator*() const 
	{ 
		return *get();
	}    

	/** 
	@brief		-> Operator Overloading

	@return		Template객체 참조자 Pointer
	@remark		Right Value 에 위치 하며 Smart pointer 에 붙는 -> 연산은 현재 Smart pointer내 Template객체 참조자의 Pointer 를 넘긴다.
	@author		김용태
	@date		2006
	*/
	_T* operator->() const 
	{
		return get();
	}

	/** 
	@brief		Left Value 의 * Operator Overloading

	@return		Template객체 참조자 Pointer
	@remark		Left Value 에 위치 하며 Smart pointer 에 붙는 * 연산은 현재 Smart pointer내 Template객체 참조자의 Pointer 를 넘긴다.
	@author		김용태
	@date		2006
	*/
	operator _T*() 
	{ 
		return get(); 
	}

	/** 
	@brief		비교문에서 사용되는 Operator Overloading

	@return		Smart Pointer 와 Template 객체 생성 유무, True or False
	@remark		
	@author		김용태
	@date		2006
	*/
	operator bool()
	{ 
		if( _o && _o->p )
		{
			return true;
		}
		return false; 
	}

	/** 
	@brief		비교문에서 사용되는 ! Operator Overloading

	@return		Smart Pointer 와 Template 객체 생성 유무에 대한 Convert 값, True or False
	@remark		
	@author		김용태
	@date		2006
	*/
	bool operator !() 
	{ 
		return !operator bool();
	}

	/** 
	@brief		Template 객체 Pointer 를 가져온다.

	@return		Template객체 참조자 Pointer
	@remark		
	@author		김용태
	@date		2006
	*/
	_T* get() const 
	{ 
		if(!_o) return NULL; 
		return _o->p; 
	}


	friend bool operator==(const _sp<_T>& sp1, const _sp<_T>& sp2);	///< == Operator Overloading 1

	friend bool operator==(const _sp<_T>& sp, const _T* p);			///< == Operator Overloading 2

	friend bool operator==(const _sp<_T>& sp, int n);				///< == Operator Overloading 3

};
/** 
@}
*/ 

/** 
@brief		== Operator Overloading 1

@return		비교의 결과, True or False
@remark		Smart Pointer 와 Smart Pointer 간의 비교를 한다.
@author		김용태
@date		2006
*/
template<class _T> inline
bool operator==(
				const _sp<_T>& sp1,	///< Smart pointer 객체 
				const _sp<_T>& sp2	///< Smart pointer 객체
				)
{ 
	if( sp1._o && sp2._o )
	{
		return (sp1._o->p == sp2._o->p); 
	}
	return false; 
}

/** 
@brief		== Operator Overloading 2

@return		비교의 결과, True or False
@remark		Smart Pointer 와 Template 객체 간의 비교를 한다.
@author		김용태
@date		2006
*/
template<class _T> inline
bool operator==(
				const _sp<_T>& sp,	///< Smart pointer 객체
				const _T* p			///< Template 객체
				)
{ 
	if( sp->_o ) 
	{
		return ( sp->_o->p == p ); 
	}
	return false;
}

/** 
@brief		== Operator Overloading 3

@return		비교의 결과, True or False
@remark		Smart Pointer 와 Template 객체 포인터에서 형변환된 정수형간의 비교를 한다.
@author		김용태
@date		2006
*/
template<class _T> inline
bool operator==(
				const _sp<_T>& sp,	///< Smart pointer 객체
				int n				///< 정수형 값
				)
{   
	int tmp = 0;

	if( !sp._o ) 
	{
		tmp = 0;
	}
	else
	{
		if( !sp._o->p ) 
		{
			tmp = 0; 
		}
		else 
		{
			tmp = (int)(sp._o->p); 
		}
	}
	return (tmp == n );
}

