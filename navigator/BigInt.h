
// Author : Mick ONeill
// http://www.codeguru.com/cpp/data/data-misc/values/article.php/c4563/


#ifndef BIGINT_H
#define BIGINT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory.h>
#include <string.h>
#include <ctype.h>

class CBigInt {
protected:
// Data
	int m_ndw;					// Number of elements in the m_value array
	unsigned long* m_value;		// pointer to the actual data
	mutable char* m_stringBuf;	// Temporary string buffer for formatted output
// Internal procedures
	void Div(const CBigInt& Divisor, CBigInt* Quotient, CBigInt *Remainder) const;
	// Integer Divisions will be faster
	void Div(unsigned long Divisor, CBigInt *Quotient, CBigInt *Remainder) const;
	void Div(long Divisor, CBigInt *Quotient, CBigInt *Remainder) const;
	void Negate();
	bool IsNegative() const { return m_ndw == 0 ? false : ((*(m_value + m_ndw - 1) & 0x80000000) != 0); }
	void ExpandTo(int nWords, bool bNegative = false);
	void Expand(int nWords, bool bNegative = false) { ExpandTo(nWords + m_ndw, bNegative); }
	void Optimize();
	CBigInt& FromHex(const char *szVal);
	CBigInt& FromOct(const char *szVal);
	CBigInt& FromBin(const char *szVal);
	CBigInt& FromDec(const char *szVal);
public:
// Constructors/Destructor
	CBigInt();
	CBigInt(const CBigInt&);
	CBigInt(int Value);
	CBigInt(unsigned int Value);
	CBigInt(long Value);
	CBigInt(unsigned long Value);
	CBigInt(const __int64 &Value);
	CBigInt(const unsigned __int64 &Value);
	CBigInt(const unsigned long *Data, int DataSize);
	CBigInt(const char* szValue);

	virtual ~CBigInt();

// Conversion Operators
	operator bool() const;
	operator short() const { return (short)m_value[0]; }
	operator int() const { return (int)m_value[0]; }
	operator long() const { return (long)m_value[0]; }
	operator __int64() const;
	operator unsigned short() const { return (unsigned short)m_value[0]; }
	operator unsigned int() const { return (unsigned int)m_value[0]; }
	operator unsigned long() const { return (unsigned long) m_value[0]; }
	operator unsigned __int64() const;
	operator char*() const;
	operator const char*() const;

// Unary Operators
	CBigInt& operator ++();		// Prefix increment
	CBigInt operator ++(int);	// Postfix increment
	CBigInt& operator --();		// Prefix decrement
	CBigInt operator --(int);	// Postfix decrement
	CBigInt operator -() const;	// Negation
	CBigInt operator ~() const;	// Ones complement
	bool operator !() const;	// Logical Not

// Binary Operators
	void Div(const CBigInt &Divisor, CBigInt &Quotient, CBigInt &Remainder) const {
		Div(Divisor, &Quotient, &Remainder);
	}
	void Div(unsigned long Divisor, CBigInt &Quotient, CBigInt &Remainder) const {
		Div(Divisor, &Quotient, &Remainder);
	}
	void Div(long Divisor, CBigInt &Quotient, CBigInt &Remainder) const {
		Div(Divisor, &Quotient, &Remainder);
	}
	void Div(int Divisor, CBigInt &Quotient, CBigInt &Remainder) const {
		Div((long)Divisor, &Quotient, &Remainder);
	}
	void Div(unsigned int Divisor, CBigInt &Quotient, CBigInt &Remainder) const {
		Div((unsigned long)Divisor, &Quotient, &Remainder);
	}

	CBigInt operator +(const CBigInt& Value) const;
	CBigInt operator +(int Value) const { return operator +(CBigInt(Value)); }
	CBigInt operator +(unsigned int Value) const { return operator +(CBigInt(Value)); }
	CBigInt operator +(long Value) const { return operator +(CBigInt(Value)); }
	CBigInt operator +(unsigned long Value) const { return operator +(CBigInt(Value)); }
	CBigInt operator +(const __int64 &Value) const { return operator +(CBigInt(Value)); }
	CBigInt operator +(const unsigned __int64 &Value) const { return operator +(CBigInt(Value)); }

	friend CBigInt operator +(int A, const CBigInt &B) { return CBigInt(A).operator +(B); }
	friend CBigInt operator +(unsigned int A, const CBigInt &B) { return CBigInt(A).operator +(B); }
	friend CBigInt operator +(long A, const CBigInt &B) { return CBigInt(A).operator +(B); }
	friend CBigInt operator +(unsigned long A, const CBigInt &B) { return CBigInt(A).operator +(B); }
	friend CBigInt operator +(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator +(B); }
	friend CBigInt operator +(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator +(B);  }

	CBigInt operator -(const CBigInt& Value) const;
	CBigInt operator -(int Value) const { return operator -(CBigInt(Value)); }
	CBigInt operator -(unsigned int Value) const { return operator -(CBigInt(Value)); }
	CBigInt operator -(long Value) const { return operator -(CBigInt(Value)); }
	CBigInt operator -(unsigned long Value) const { return operator -(CBigInt(Value)); }
	CBigInt operator -(const __int64 &Value) const { return operator -(CBigInt(Value)); }
	CBigInt operator -(const unsigned __int64 &Value) const { return operator -(CBigInt(Value)); }

	friend CBigInt operator -(int A, const CBigInt &B) { return CBigInt(A).operator -(B); }
	friend CBigInt operator -(unsigned int A, const CBigInt &B) { return CBigInt(A).operator -(B); }
	friend CBigInt operator -(long A, const CBigInt &B) { return CBigInt(A).operator -(B); }
	friend CBigInt operator -(unsigned long A, const CBigInt &B) { return CBigInt(A).operator -(B); }
	friend CBigInt operator -(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator -(B); }
	friend CBigInt operator -(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator -(B);  }

	CBigInt operator *(const CBigInt& Value) const;
	CBigInt operator *(unsigned long Value) const;
	CBigInt operator *(unsigned int Value) const { return operator *((unsigned long)Value); }
	CBigInt operator *(long Value) const;
	CBigInt operator *(int Value) const { return operator *(long(Value)); }
	CBigInt operator *(const __int64 &Value) const { return operator *(CBigInt(Value)); }
	CBigInt operator *(const unsigned __int64 &Value) const { return operator *(CBigInt(Value)); }
	
	friend CBigInt operator *(int A, const CBigInt &B) { return B.operator *(A); }
	friend CBigInt operator *(unsigned int A, const CBigInt &B) { return B.operator *(A); }
	friend CBigInt operator *(long A, const CBigInt &B) { return B.operator *(A); }
	friend CBigInt operator *(unsigned long A, const CBigInt &B) { return B.operator *(A); }
	friend CBigInt operator *(const __int64 &A, const CBigInt &B) { return B.operator *(A); }
	friend CBigInt operator *(const unsigned __int64 A, const CBigInt &B) { return B.operator *(A);  }

	CBigInt operator /(const CBigInt& Value) const;
	CBigInt operator /(int Value) const;
	CBigInt operator /(unsigned int Value) const;
	CBigInt operator /(long Value) const;
	CBigInt operator /(unsigned long Value) const;
	CBigInt operator /(const __int64 &Value) const { return operator /(CBigInt(Value)); }
	CBigInt operator /(const unsigned __int64 &Value) const { return operator /(CBigInt(Value)); }
	
	friend int operator /(int A, const CBigInt &B);
	friend unsigned int operator /(unsigned int A, const CBigInt &B);
	friend long operator /(long A, const CBigInt &B);
	friend unsigned long operator /(unsigned long A, const CBigInt &B);
	friend __int64 operator /(const __int64 &A, const CBigInt &B);
	friend unsigned __int64 operator /(const unsigned __int64 A, const CBigInt &B);

	CBigInt operator %(const CBigInt& Value) const;
	CBigInt operator %(int Value) const;
	CBigInt operator %(unsigned int Value) const;
	CBigInt operator %(long Value) const;
	CBigInt operator %(unsigned long Value) const;
	CBigInt operator %(const __int64 &Value) const { return operator %(CBigInt(Value)); }
	CBigInt operator %(const unsigned __int64 &Value) const { return operator %(CBigInt(Value)); }
	
	friend CBigInt operator %(int A, const CBigInt &B);
	friend CBigInt operator %(unsigned int A, const CBigInt &B);
	friend CBigInt operator %(long A, const CBigInt &B);
	friend CBigInt operator %(unsigned long A, const CBigInt &B);
	friend CBigInt operator %(const __int64 &A, const CBigInt &B);
	friend CBigInt operator %(const unsigned __int64 A, const CBigInt &B);

	CBigInt operator &(const CBigInt& Value) const;
	CBigInt operator &(int Value) const { return operator &(CBigInt(Value)); }
	CBigInt operator &(unsigned int Value) const { return operator &(CBigInt(Value)); }
	CBigInt operator &(long Value) const { return operator &(CBigInt(Value)); }
	CBigInt operator &(unsigned long Value) const { return operator &(CBigInt(Value)); }
	CBigInt operator &(const __int64 &Value) const { return operator &(CBigInt(Value)); }
	CBigInt operator &(const unsigned __int64 &Value) const { return operator &(CBigInt(Value)); }
	
	friend CBigInt operator &(int A, const CBigInt &B) { return CBigInt(A).operator &(B); }
	friend CBigInt operator &(unsigned int A, const CBigInt &B) { return CBigInt(A).operator &(B); }
	friend CBigInt operator &(long A, const CBigInt &B) { return CBigInt(A).operator &(B); }
	friend CBigInt operator &(unsigned long A, const CBigInt &B) { return CBigInt(A).operator &(B); }
	friend CBigInt operator &(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator &(B); }
	friend CBigInt operator &(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator &(B);  }

	CBigInt operator |(const CBigInt& Value) const;
	CBigInt operator |(int Value) const { return operator |(CBigInt(Value)); }
	CBigInt operator |(unsigned int Value) const { return operator |(CBigInt(Value)); }
	CBigInt operator |(long Value) const { return operator |(CBigInt(Value)); }
	CBigInt operator |(unsigned long Value) const { return operator |(CBigInt(Value)); }
	CBigInt operator |(const __int64 &Value) const { return operator |(CBigInt(Value)); }
	CBigInt operator |(const unsigned __int64 &Value) const { return operator |(CBigInt(Value)); }
	
	friend CBigInt operator |(int A, const CBigInt &B) { return CBigInt(A).operator |(B); }
	friend CBigInt operator |(unsigned int A, const CBigInt &B) { return CBigInt(A).operator |(B); }
	friend CBigInt operator |(long A, const CBigInt &B) { return CBigInt(A).operator |(B); }
	friend CBigInt operator |(unsigned long A, const CBigInt &B) { return CBigInt(A).operator |(B); }
	friend CBigInt operator |(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator |(B); }
	friend CBigInt operator |(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator |(B);  }

	CBigInt operator ^(const CBigInt& Value) const;
	CBigInt operator ^(int Value) const { return operator ^(CBigInt(Value)); }
	CBigInt operator ^(unsigned int Value) const { return operator ^(CBigInt(Value)); }
	CBigInt operator ^(long Value) const { return operator ^(CBigInt(Value)); }
	CBigInt operator ^(unsigned long Value) const { return operator ^(CBigInt(Value)); }
	CBigInt operator ^(const __int64 &Value) const { return operator ^(CBigInt(Value)); }
	CBigInt operator ^(const unsigned __int64 &Value) const { return operator ^(CBigInt(Value)); }
	
	friend CBigInt operator ^(int A, const CBigInt &B) { return CBigInt(A).operator ^(B); }
	friend CBigInt operator ^(unsigned int A, const CBigInt &B) { return CBigInt(A).operator ^(B); }
	friend CBigInt operator ^(long A, const CBigInt &B) { return CBigInt(A).operator ^(B); }
	friend CBigInt operator ^(unsigned long A, const CBigInt &B) { return CBigInt(A).operator ^(B); }
	friend CBigInt operator ^(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator ^(B); }
	friend CBigInt operator ^(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator ^(B);  }

	CBigInt operator <<(int nBits) const;
	CBigInt operator >>(int nBits) const;

// Logical Operators
	bool operator !=(const CBigInt& Value) const;
	bool operator !=(int Value) const { return operator !=(CBigInt(Value)); }
	bool operator !=(unsigned int Value) const { return operator !=(CBigInt(Value)); }
	bool operator !=(long Value) const { return operator !=(CBigInt(Value)); }
	bool operator !=(unsigned long Value) const { return operator !=(CBigInt(Value)); }
	bool operator !=(const __int64 &Value) const { return operator !=(CBigInt(Value)); }
	bool operator !=(const unsigned __int64 &Value) const { return operator !=(CBigInt(Value)); }

	friend bool operator !=(int A, const CBigInt &B) { return CBigInt(A).operator !=(B); }
	friend bool operator !=(unsigned int A, const CBigInt &B) { return CBigInt(A).operator !=(B); }
	friend bool operator !=(long A, const CBigInt &B) { return CBigInt(A).operator !=(B); }
	friend bool operator !=(unsigned long A, const CBigInt &B) { return CBigInt(A).operator !=(B); }
	friend bool operator !=(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator !=(B); }
	friend bool operator !=(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator !=(B);  }

	bool operator ==(const CBigInt& Value) const;
	bool operator ==(int Value) const { return operator ==(CBigInt(Value)); }
	bool operator ==(unsigned int Value) const { return operator ==(CBigInt(Value)); }
	bool operator ==(long Value) const { return operator ==(CBigInt(Value)); }
	bool operator ==(unsigned long Value) const { return operator ==(CBigInt(Value)); }
	bool operator ==(const __int64 &Value) const { return operator ==(CBigInt(Value)); }
	bool operator ==(const unsigned __int64 &Value) const { return operator ==(CBigInt(Value)); }
	
	friend bool operator ==(int A, const CBigInt &B) { return CBigInt(A).operator ==(B); }
	friend bool operator ==(unsigned int A, const CBigInt &B) { return CBigInt(A).operator ==(B); }
	friend bool operator ==(long A, const CBigInt &B) { return CBigInt(A).operator ==(B); }
	friend bool operator ==(unsigned long A, const CBigInt &B) { return CBigInt(A).operator ==(B); }
	friend bool operator ==(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator ==(B); }
	friend bool operator ==(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator ==(B);  }

	bool operator <(const CBigInt& Value) const;
	bool operator <(int Value) const { return operator <(CBigInt(Value)); }
	bool operator <(unsigned int Value) const { return operator <(CBigInt(Value)); }
	bool operator <(long Value) const { return operator <(CBigInt(Value)); }
	bool operator <(unsigned long Value) const { return operator <(CBigInt(Value)); }
	bool operator <(const __int64 &Value) const { return operator <(CBigInt(Value)); }
	bool operator <(const unsigned __int64 &Value) const { return operator <(CBigInt(Value)); }
	
	friend bool operator <(int A, const CBigInt &B) { return CBigInt(A).operator <(B); }
	friend bool operator <(unsigned int A, const CBigInt &B) { return CBigInt(A).operator <(B); }
	friend bool operator <(long A, const CBigInt &B) { return CBigInt(A).operator <(B); }
	friend bool operator <(unsigned long A, const CBigInt &B) { return CBigInt(A).operator <(B); }
	friend bool operator <(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator <(B); }
	friend bool operator <(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator <(B);  }

	bool operator <=(const CBigInt& Value) const;
	bool operator <=(int Value) const { return operator <=(CBigInt(Value)); }
	bool operator <=(unsigned int Value) const { return operator <=(CBigInt(Value)); }
	bool operator <=(long Value) const { return operator <=(CBigInt(Value)); }
	bool operator <=(unsigned long Value) const { return operator <=(CBigInt(Value)); }
	bool operator <=(const __int64 &Value) const { return operator <=(CBigInt(Value)); }
	bool operator <=(const unsigned __int64 &Value) const { return operator <=(CBigInt(Value)); }
	
	friend bool operator <=(int A, const CBigInt &B) { return CBigInt(A).operator <=(B); }
	friend bool operator <=(unsigned int A, const CBigInt &B) { return CBigInt(A).operator <=(B); }
	friend bool operator <=(long A, const CBigInt &B) { return CBigInt(A).operator <=(B); }
	friend bool operator <=(unsigned long A, const CBigInt &B) { return CBigInt(A).operator <=(B); }
	friend bool operator <=(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator <=(B); }
	friend bool operator <=(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator <=(B);  }

	bool operator >(const CBigInt& Value) const;
	bool operator >(int Value) const { return operator >(CBigInt(Value)); }
	bool operator >(unsigned int Value) const { return operator >(CBigInt(Value)); }
	bool operator >(long Value) const { return operator >(CBigInt(Value)); }
	bool operator >(unsigned long Value) const { return operator >(CBigInt(Value)); }
	bool operator >(const __int64 &Value) const { return operator >(CBigInt(Value)); }
	bool operator >(const unsigned __int64 &Value) const { return operator >(CBigInt(Value)); }
	
	friend bool operator >(int A, const CBigInt &B) { return CBigInt(A).operator >(B); }
	friend bool operator >(unsigned int A, const CBigInt &B) { return CBigInt(A).operator >(B); }
	friend bool operator >(long A, const CBigInt &B) { return CBigInt(A).operator >(B); }
	friend bool operator >(unsigned long A, const CBigInt &B) { return CBigInt(A).operator >(B); }
	friend bool operator >(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator >(B); }
	friend bool operator >(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator >(B);  }

	bool operator >=(const CBigInt& Value) const;
	bool operator >=(int Value) const { return operator >=(CBigInt(Value)); }
	bool operator >=(unsigned int Value) const { return operator >=(CBigInt(Value)); }
	bool operator >=(long Value) const { return operator >=(CBigInt(Value)); }
	bool operator >=(unsigned long Value) const { return operator >=(CBigInt(Value)); }
	bool operator >=(const __int64 &Value) const { return operator >=(CBigInt(Value)); }
	bool operator >=(const unsigned __int64 &Value) const { return operator >=(CBigInt(Value)); }
	
	friend bool operator >=(int A, const CBigInt &B) { return CBigInt(A).operator >=(B); }
	friend bool operator >=(unsigned int A, const CBigInt &B) { return CBigInt(A).operator >=(B); }
	friend bool operator >=(long A, const CBigInt &B) { return CBigInt(A).operator >=(B); }
	friend bool operator >=(unsigned long A, const CBigInt &B) { return CBigInt(A).operator >=(B); }
	friend bool operator >=(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator >=(B); }
	friend bool operator >=(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator >=(B);  }

	bool operator &&(const CBigInt& Value) const;
	bool operator &&(int Value) const { return operator &&(CBigInt(Value)); }
	bool operator &&(unsigned int Value) const { return operator &&(CBigInt(Value)); }
	bool operator &&(long Value) const { return operator &&(CBigInt(Value)); }
	bool operator &&(unsigned long Value) const { return operator &&(CBigInt(Value)); }
	bool operator &&(const __int64 &Value) const { return operator &&(CBigInt(Value)); }
	bool operator &&(const unsigned __int64 &Value) const { return operator &&(CBigInt(Value)); }
	
	friend bool operator &&(bool A, const CBigInt &B) { return A && (bool)B; }
	friend bool operator &&(int A, const CBigInt &B) { return CBigInt(A).operator &&(B); }
	friend bool operator &&(unsigned int A, const CBigInt &B) { return CBigInt(A).operator &&(B); }
	friend bool operator &&(long A, const CBigInt &B) { return CBigInt(A).operator &&(B); }
	friend bool operator &&(unsigned long A, const CBigInt &B) { return CBigInt(A).operator &&(B); }
	friend bool operator &&(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator &&(B); }
	friend bool operator &&(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator &&(B);  }

	bool operator ||(const CBigInt& Value) const;
	bool operator ||(int Value) const { return operator ||(CBigInt(Value)); }
	bool operator ||(unsigned int Value) const { return operator ||(CBigInt(Value)); }
	bool operator ||(long Value) const { return operator ||(CBigInt(Value)); }
	bool operator ||(unsigned long Value) const { return operator ||(CBigInt(Value)); }
	bool operator ||(const __int64 &Value) const { return operator ||(CBigInt(Value)); }
	bool operator ||(const unsigned __int64 &Value) const { return operator ||(CBigInt(Value)); }

	friend bool operator ||(bool A, const CBigInt &B) { return A || (bool)B; }
	friend bool operator ||(int A, const CBigInt &B) { return CBigInt(A).operator ||(B); }
	friend bool operator ||(unsigned int A, const CBigInt &B) { return CBigInt(A).operator ||(B); }
	friend bool operator ||(long A, const CBigInt &B) { return CBigInt(A).operator ||(B); }
	friend bool operator ||(unsigned long A, const CBigInt &B) { return CBigInt(A).operator ||(B); }
	friend bool operator ||(const __int64 &A, const CBigInt &B) { return CBigInt(A).operator ||(B); }
	friend bool operator ||(const unsigned __int64 A, const CBigInt &B) { return CBigInt(A).operator ||(B);  }

// Assignment Operators

	CBigInt& operator =(const CBigInt& Value);
	CBigInt& operator +=(const CBigInt& Value) { return *this = operator +(Value); }
	CBigInt& operator +=(unsigned __int64 Value) { return *this = operator +(Value); }
	CBigInt& operator +=(unsigned long Value) { return *this = operator +(Value); }
	CBigInt& operator +=(unsigned int Value) { return *this = operator +(Value); }
	CBigInt& operator +=(__int64 Value) { return *this = operator +(Value); }
	CBigInt& operator +=(long Value) { return *this = operator +(Value); }
	CBigInt& operator +=(int Value) { return *this = operator +(Value); }
	CBigInt& operator -=(const CBigInt& Value) { return *this = operator -(Value); }
	CBigInt& operator -=(unsigned __int64 Value) { return *this = operator -(Value); }
	CBigInt& operator -=(__int64 Value) { return *this = operator -(Value); }
	CBigInt& operator -=(unsigned long Value) { return *this = operator -(Value); }
	CBigInt& operator -=(unsigned int Value) { return *this = operator -(Value); }
	CBigInt& operator -=(long Value) { return *this = operator -(Value); }
	CBigInt& operator -=(int Value) { return *this = operator -(Value); }
	CBigInt& operator *=(const CBigInt& Value) { return *this = operator *(Value); }
	CBigInt& operator *=(unsigned __int64 Value) { return *this = operator *(Value); }
	CBigInt& operator *=(__int64 Value) { return *this = operator *(Value); }
	CBigInt& operator *=(unsigned long Value) { return *this = operator *(Value); }
	CBigInt& operator *=(unsigned int Value) { return *this = operator *(Value); }
	CBigInt& operator *=(long Value) { return *this = operator *(Value); }
	CBigInt& operator *=(int Value) { return *this = operator *(Value); }
	CBigInt& operator /=(const CBigInt& Value) { Div(Value, this, NULL); return *this; }
	CBigInt& operator /=(unsigned __int64 Value) { return *this = operator /(Value); }
	CBigInt& operator /=(__int64 Value) { return *this = operator /(Value); }
	CBigInt& operator /=(unsigned long Value) { Div(Value, this, NULL); return *this; }
	CBigInt& operator /=(unsigned int Value) { Div((unsigned long) Value, this, NULL); return *this; }
	CBigInt& operator /=(long Value) { Div(Value, this, NULL); return *this; }
	CBigInt& operator /=(int Value) { Div((long)Value, this, NULL); return *this; }
	CBigInt& operator %=(const CBigInt& Value) { Div(Value, NULL, this); return *this; }
	CBigInt& operator %=(unsigned __int64 Value)  { return *this = operator /(Value); }
	CBigInt& operator %=(__int64 Value)  { return *this = operator /(Value); }
	CBigInt& operator %=(unsigned long Value) { Div(Value, NULL, this); return *this; }
	CBigInt& operator %=(unsigned int Value) { Div((unsigned long)Value, NULL, this); return *this; }
	CBigInt& operator %=(long Value) { Div(Value, NULL, this); return *this; }
	CBigInt& operator %=(int Value) { Div((long)Value, NULL, this); return *this; }
	CBigInt& operator &=(const CBigInt& Value) { return *this = operator &(Value); }
	CBigInt& operator &=(unsigned long Value) { return *this = operator &(Value); }
	CBigInt& operator &=(unsigned __int64 Value) { return *this = operator &(Value); }
	CBigInt& operator &=(__int64 Value) { return *this = operator &(Value); }
	CBigInt& operator &=(unsigned int Value) { return *this = operator &(Value); }
	CBigInt& operator &=(long Value) { return *this = operator &(Value); }
	CBigInt& operator &=(int Value) { return *this = operator &(Value); }
	CBigInt& operator |=(const CBigInt& Value) { return *this = operator |(Value); }
	CBigInt& operator |=(unsigned __int64 Value) { return *this = operator |(Value); }
	CBigInt& operator |=(__int64 Value) { return *this = operator |(Value); }
	CBigInt& operator |=(unsigned long Value) { return *this = operator |(Value); }
	CBigInt& operator |=(unsigned int Value) { return *this = operator |(Value); }
	CBigInt& operator |=(long Value) { return *this = operator |(Value); }
	CBigInt& operator |=(int Value) { return *this = operator |(Value); }
	CBigInt& operator ^=(const CBigInt& Value) { return *this = operator ^(Value); }
	CBigInt& operator ^=(unsigned __int64 Value) { return *this = operator ^(Value); }
	CBigInt& operator ^=(__int64 Value) { return *this = operator ^(Value); }
	CBigInt& operator ^=(unsigned long Value) { return *this = operator ^(Value); }
	CBigInt& operator ^=(unsigned int Value) { return *this = operator ^(Value); }
	CBigInt& operator ^=(long Value) { return *this = operator ^(Value); }
	CBigInt& operator ^=(int Value) { return *this = operator ^(Value); }
	CBigInt& operator <<=(int nBits);
	CBigInt& operator >>=(int nBits);
	CBigInt& FromString(const char *szBuf, int Radix = 10);

// Output functions and operators
	char* Format(char *szBuf, unsigned long nBuffLen, unsigned long *pNeeded, unsigned int Radix = 10) const;
	char* Format(unsigned int Radix = 10) const;
	void ClearFormatString() { if (m_stringBuf) { delete [] m_stringBuf; m_stringBuf = NULL; } }

	unsigned long ByteSize() const { return (unsigned long)m_ndw * sizeof(unsigned long); }
	int DWordSize() const { return m_ndw; }

	bool IsNull() const { return m_ndw == 0; }
	CBigInt& MakeNull() { if (m_ndw) delete [] m_value; m_ndw = 0; m_value = NULL; return *this; }
};


#endif // !defined(BIGINT_H)
