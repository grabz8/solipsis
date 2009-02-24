/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Mick ONeill

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

// Author : Mick ONeill
// http://www.codeguru.com/cpp/data/data-misc/values/article.php/c4563/

// BigInt.cpp: implementation of the CBigInt class.
//
//////////////////////////////////////////////////////////////////////
#include "BigInt.h"

const int BYTES = sizeof(unsigned long);
const int NIBBLES = BYTES * 2;
const int BITS = BYTES * 8;
const unsigned long MAXULONG = 0xffffffff;
const unsigned long MAXLONG = 0x7fffffff;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBigInt::CBigInt() {
	m_stringBuf = NULL;
	*(m_value = new unsigned long[m_ndw = 1]) = 0;
	if (m_value == NULL) m_ndw = 0;
}

CBigInt::CBigInt(unsigned int Value) {
	m_stringBuf = NULL;
	m_value = new unsigned long[m_ndw = 2];
	if (m_value == NULL) 
		m_ndw = 0;
	else {
		m_value[0] = Value;
		if (IsNegative())
			Expand(1);
	}
}

CBigInt::CBigInt(unsigned long Value) {
	m_stringBuf = NULL;
	m_value = new unsigned long[m_ndw = 1];
	if (m_value == NULL) 
		m_ndw = 0;
	else {
		m_value[0] = Value;
		if (IsNegative())
			Expand(1);
	}
}

CBigInt::CBigInt(const unsigned __int64 &Value) {
	unsigned long HighValue = (unsigned long)(Value >> BITS);
	m_stringBuf = NULL;
	m_value = new unsigned long[m_ndw = HighValue ? 2 : 1];
	if (m_value == NULL) 
		m_ndw = 0;
	else {
		m_value[0] = (unsigned long) (Value & MAXULONG);
		if (HighValue) m_value[1] = HighValue;
		if (IsNegative())
			Expand(1);
	}
}

CBigInt::CBigInt(int Value) {
	m_stringBuf = NULL;
	m_value = new unsigned long[m_ndw = 1];
	if (m_value == NULL) 
		m_ndw = 0;
	else
		m_value[0] = (unsigned long)Value;
}

CBigInt::CBigInt(long Value) {
	m_stringBuf = NULL;
	m_value = new unsigned long[m_ndw = 1];
	if (m_value == NULL) 
		m_ndw = 0;
	else
		m_value[0] = (unsigned long)Value;
}

CBigInt::CBigInt(const __int64 &Value) {
	unsigned long HighValue = (unsigned long)(Value >> BITS);
	if ((Value & 0xffffffff80000000) == 0xffffffff80000000)
		HighValue = 0;
	m_stringBuf = NULL;
	m_value = new unsigned long[m_ndw = HighValue ? 2 : 1];
	if (m_value == NULL) 
		m_ndw = 0;
	else {
		m_value[0] = (unsigned long) (Value & MAXULONG);
		if (HighValue) m_value[1] = (unsigned long) HighValue;
	}
}

CBigInt::CBigInt(const CBigInt &Value) {
	if (Value.IsNull()) {
		m_ndw = 0;
		m_value = NULL;
	}
	else {
		m_value = new unsigned long[m_ndw = Value.m_ndw];
		if (m_value == NULL)
			m_ndw = 0;
		else
			memcpy(m_value, Value.m_value, m_ndw * BYTES);
	}
	m_stringBuf = NULL;
}

CBigInt::CBigInt(const unsigned long *Data, int DataSize) {
	m_value = new unsigned long[m_ndw = DataSize];
	if (m_value == NULL)
		m_ndw = 0;
	else
		memcpy(m_value, Data, DataSize * BYTES);
	m_stringBuf = NULL;
}

CBigInt::CBigInt(const char *szValue) {
	m_ndw = 0;
	m_value = NULL;
	m_stringBuf = NULL;
	switch (*szValue) {
	case '0':
		if (*(szValue + 1) == 'x' || *(szValue + 1) == 'X')
			FromHex(szValue + 2);
		else if (*(szValue + 1) == 'b' || *(szValue + 1) == 'B')
			FromBin(szValue + 2);
		else
			FromOct(szValue + 1);
		break;
	case 0:
		*(m_value = new unsigned long[m_ndw = 1]) = 0;
		break;
	default:
		FromDec(szValue);
		break;
	}
}

CBigInt::~CBigInt() {
	if (!IsNull()) delete [] m_value;
	if (m_stringBuf)
		delete [] m_stringBuf;
}

// Internal Data maintenance
void CBigInt::ExpandTo(int nWords, bool bNegative) {
	if (nWords > m_ndw) {
		unsigned long *tmp = new unsigned long[nWords];
		if (tmp == NULL) {
			MakeNull();
		}
		else {
			memset(tmp, bNegative ? 0xff : 0, nWords * BYTES);
			memcpy(tmp, m_value, m_ndw * BYTES);
			delete [] m_value;
			m_value = tmp;
			m_ndw = nWords;
		}
	}
}

void CBigInt::Optimize() {
	if (!IsNull()) {
		int newlen = m_ndw;
		if (IsNegative()) {
			while (newlen > 1 && m_value[newlen - 1] == MAXULONG)
				newlen--;
			if ((m_value[newlen - 1] & 0x80000000) == 0)
				newlen++;
		} else {
			while (newlen > 1 && m_value[newlen - 1] == 0)
				newlen--;
			if ((m_value[newlen - 1] & 0x80000000) != 0)
				newlen++;
		}
		if (m_ndw != newlen) {
			unsigned long *tmp = new unsigned long[newlen];
			if (tmp != NULL) {
				// We can get away with not etting this object to NULL,
				// as the value remains the same, even if not optimized
				memcpy(tmp, m_value, newlen * BYTES);
				if (IsNegative())
					*(tmp + newlen - 1) |= 0x80000000;
				m_ndw = newlen;
				delete [] m_value;
				m_value = tmp;
			}
		}
	}
}

void CBigInt::Negate() {
	bool bNeg = IsNegative();
	int i;

	if (bNeg)
		operator --();
	for (i = 0; i < m_ndw; ++i)
		m_value[i] = ~m_value[i];
	if (!bNeg)
		operator++();
	else if (m_value[m_ndw - 1] & 0x80000000)
		Expand(1, false);
}

// Conversion Operators
CBigInt::operator bool() const {
	bool rval = false;
	for (int i = 0; i < m_ndw && !rval; ++i)
		rval = m_value[i] != 0;
	return rval;
}

CBigInt::operator __int64() const {
	if (IsNull())
		return (__int64)0;
	if (m_ndw == 1)
		return (__int64)(long)m_value[0];
	return (__int64)((unsigned __int64)m_value[1] << BITS | m_value[0]);
}

CBigInt::operator unsigned __int64() const {
	if (IsNull())
		return (unsigned __int64)0;
	if (m_ndw == 1)
		return (unsigned __int64) m_value[0];
	return (unsigned __int64)m_value[1] << BITS | m_value[0];
}

CBigInt::operator char*() const {
	Format();
	return m_stringBuf;
}

CBigInt::operator const char*() const {
	Format();
	return m_stringBuf;
}

// Unary Operators
CBigInt& CBigInt::operator ++() {
	bool Neg = IsNegative();
	for (int i = 0; i < m_ndw; ++i) {
		if (++m_value[i])
			break;
	}
	if (IsNegative() && !Neg) // Crossed to MSBit being set
		Expand(1);
	return *this;
}

CBigInt CBigInt::operator++(int) {
	CBigInt tmp = *this;
	operator ++();
	return tmp;
}

CBigInt& CBigInt::operator--() {
	bool Neg = IsNegative();
	for (int i = 0; i < m_ndw; ++i) {
		if (m_value[i]--)
			break;
	}
	if (Neg && !IsNegative()) // Crossed to MSBit being cleared
		Expand(1, true);
	return *this;
}

CBigInt CBigInt::operator--(int) {
	CBigInt tmp(*this);
	operator --();
	return tmp;
}

CBigInt CBigInt::operator -() const {
	CBigInt tmp(*this);
	tmp.Negate();
	tmp.Optimize();
	return tmp;
}

CBigInt CBigInt::operator ~() const	{
	CBigInt tmp(*this);
	tmp.Expand(m_ndw - 1);
	if (!tmp.IsNull()) {
		for (int i = 0; i < m_ndw; ++i)
			tmp.m_value[i] = ~m_value[i];
		tmp.Optimize();
	}
	return tmp;
}

bool CBigInt::operator !() const {
	bool rval = true;
	for (int i = 0; i < m_ndw && rval; ++i)
		rval = m_value[i] == 0;
	return rval;
}

// Binary Operators
CBigInt CBigInt::operator +(const CBigInt& Value) const {
	CBigInt A(*this), B(Value);
	unsigned long carry = 0;
	int i;
	bool isNegA = A.IsNegative();

	if (IsNull() || A.IsNull() || B.IsNull()) {
		A.MakeNull();
		return A;
	}
	if (A.m_ndw < B.m_ndw)
		A.ExpandTo(B.m_ndw, isNegA);
	else if (B.m_ndw < A.m_ndw)
		B.ExpandTo(A.m_ndw, B.IsNegative());

	for (i = 0; i < A.m_ndw; ++i) {
		unsigned __int64 t = (unsigned __int64)A.m_value[i] + (unsigned __int64)B.m_value[i] + carry;
		A.m_value[i] = (unsigned long)(t & MAXULONG);
		carry = (unsigned long)(t >> BITS);
	}
	isNegA |= A.IsNegative();
	if (carry || isNegA) {
		A.Expand(1, isNegA);
		A.m_value[A.m_ndw - 1] += (unsigned long)carry;
	}
	A.Optimize();
	return A;
}

CBigInt CBigInt ::operator -(const CBigInt& Value) const {
	CBigInt A(Value);
	A.Negate();
	return operator+(A);
}

CBigInt CBigInt::operator /(const CBigInt& Value) const {
	CBigInt Quotient;
	Div(Value, &Quotient, NULL);
	return Quotient;
}

CBigInt CBigInt::operator %(const CBigInt& Value) const {
	CBigInt  Remainder;
	Div(Value, NULL, &Remainder);
	return Remainder;
}

CBigInt CBigInt::operator &(const CBigInt& Value) const {
	CBigInt tmpA(*this), tmpB(Value);
	int i;

	if (m_ndw > Value.m_ndw)
		tmpB.ExpandTo(m_ndw, Value.IsNegative());
	else if (Value.m_ndw > m_ndw)
		tmpA.ExpandTo(Value.m_ndw, IsNegative());
	if (tmpA.IsNull() || tmpB.IsNull() || IsNull()) {
		tmpA.MakeNull();
		return tmpA;
	}

	for (i = 0; i < tmpA.m_ndw; ++i)
		tmpA.m_value[i] &= tmpB.m_value[i];
	tmpA.Optimize();
	return tmpA;
}

CBigInt CBigInt::operator |(const CBigInt& Value) const {
	CBigInt tmpA(*this), tmpB(Value);
	int i;

	if (tmpA.m_ndw > tmpB.m_ndw)
		tmpB.ExpandTo(m_ndw, Value.IsNegative());
	else if (tmpB.m_ndw > tmpA.m_ndw)
		tmpA.ExpandTo(Value.m_ndw, IsNegative());

	if (tmpA.IsNull() || tmpB.IsNull() || IsNull())
		tmpA.MakeNull();
	else {
		for (i = 0; i < tmpA.m_ndw; ++i)
			tmpA.m_value[i] |= tmpB.m_value[i];
		tmpA.Optimize();
	}
	return tmpA;
}

CBigInt CBigInt::operator ^(const CBigInt& Value) const {
	CBigInt tmpA(*this), tmpB(Value);
	int i;

	if (m_ndw > Value.m_ndw)
		tmpB.ExpandTo(m_ndw, Value.IsNegative());
	else if (Value.m_ndw > m_ndw)
		tmpA.ExpandTo(Value.m_ndw, IsNegative());

	if (tmpA.IsNull() || tmpB.IsNull() || IsNull())
		tmpA.MakeNull();
	else {
		for (i = 0; i < tmpA.m_ndw; ++i)
			tmpA.m_value[i] ^= tmpB.m_value[i];
		tmpA.Optimize();
	}
	return tmpA;
}

CBigInt CBigInt::operator <<(int nBits) const {
	if (nBits < 0)
		return operator >>(-nBits);
	else {
		CBigInt rval(*this);
		if (rval.IsNull())
			return rval;

		unsigned long carry = 0;
		int i, j;

		j = nBits / BITS;
		if (j) {
			rval.Expand(j);
			if (rval.IsNull())
				return rval;
			memcpy(rval.m_value + j, m_value, m_ndw * BYTES);
			memset(rval.m_value, 0, j * BYTES);
			nBits %= BITS;
		}
		if (nBits) {
			for (i = 0; i < rval.m_ndw; ++i) {
				unsigned long tmp = rval.m_value[i] >> (BITS - nBits);
				rval.m_value[i] <<= nBits;
				rval.m_value[i] |= carry;
				carry = tmp;
			}
			if (carry) {
				rval.Expand(1, IsNegative());
				if (rval.IsNull()) return rval;
				rval.m_value[rval.m_ndw - 1] <<= nBits;
				rval.m_value[rval.m_ndw - 1] |= carry;
			}
		}
		if (IsNegative()) {
			rval.Optimize();
			rval.m_value[rval.m_ndw - 1] |= 0x80000000;
		}
		else if (rval.IsNegative())
			rval.Expand(1);
		return rval;
	}
}

CBigInt CBigInt::operator >>(int nBits) const {
	if (nBits < 0)
		return operator >>(-nBits);
	else {
		CBigInt rval(*this);
		unsigned long carry = 0;
		int i, j;
		if (rval.IsNull())
			return rval;
		j = nBits / BITS;
		if (j) {
			if (j >= m_ndw) {
				rval = 0;
				nBits = 0;
			}
			else {
				unsigned long *tmp = new unsigned long[rval.m_ndw = m_ndw - j];
				if (tmp == NULL) {
					rval.MakeNull();
					return rval;
				}
				memcpy(tmp, m_value + j, rval.m_ndw * BYTES);
				delete [] rval.m_value;
				rval.m_value = tmp;
				nBits %= BITS;
			}
		}

		if (rval.IsNull())
			return rval;
		if (nBits) {
			if (IsNegative())
				carry = MAXULONG << (BITS - nBits);
			for (i = rval.m_ndw - 1; i >= 0; --i) {
				unsigned long tmp = rval.m_value[i] << (BITS - nBits);
				if (tmp == NULL) {
					rval.MakeNull();
					return rval;
				}
				rval.m_value[i] >>= nBits;
				rval.m_value[i] |= carry;
				carry = tmp;
			}
		}
		rval.Optimize();
		return rval;
	}
}

// Logical Operators
bool CBigInt::operator !=(const CBigInt& Value) const {
	bool rval = true;
	for (int i = 0; i < m_ndw && rval; ++i) {
		if (i < Value.m_ndw)
			rval = m_value[i] == Value.m_value[i];
		else
			rval = m_value[i] == (Value.IsNegative() ? MAXULONG : 0L);
	}
	return !rval;
}

bool CBigInt::operator ==(const CBigInt& Value) const {
	bool rval = true;
	for (int i = 0; i < m_ndw && rval; ++i) {
		if (i < Value.m_ndw)
			rval = m_value[i] == Value.m_value[i];
		else
			rval = m_value[i] == (Value.IsNegative() ? MAXULONG : 0L);
	}
	return rval;
}

bool CBigInt::operator <(const CBigInt& Value) const {
	if (IsNegative() == Value.IsNegative()) 
		return (*this - Value).IsNegative();
	return IsNegative();
}

bool CBigInt::operator <=(const CBigInt& Value) const {
	return operator <(Value) || operator ==(Value);
}

bool CBigInt::operator >(const CBigInt& Value) const {
	if (IsNegative() == Value.IsNegative()) 
		return (Value - *this).IsNegative();
	return Value.IsNegative();
}

bool CBigInt::operator >=(const CBigInt& Value) const {
	return operator >(Value) || operator ==(Value);
}

bool CBigInt::operator &&(const CBigInt& Value) const {
	return operator bool() && Value.operator bool();
}

bool CBigInt::operator ||(const CBigInt& Value) const {
	return operator bool() || Value.operator bool();
}

// Assignment Operators
CBigInt& CBigInt::operator =(const CBigInt& Value) {
	MakeNull();
	if (!Value.IsNull()) {
		m_value = new unsigned long[m_ndw = Value.m_ndw];
		if (m_value == NULL)
			m_ndw = 0;
		else
			memcpy(m_value, Value.m_value, m_ndw * BYTES);
	}
	return *this;
}

CBigInt& CBigInt::operator <<=(int nBits) {
	return *this = operator <<(nBits);
}

CBigInt& CBigInt::operator >>=(int nBits) {
	return *this = operator >>(nBits);
}

void CBigInt::Div(const CBigInt &Divisor, CBigInt *Quotient, CBigInt *Remainder) const {
	if (!Divisor) { 		// Divide by zero - force the exception
		_asm {
		  mov EAX, 0
		  div EAX
		}
	}
	else if (operator !()) {
		if (Quotient) *Quotient = 0;
		if (Remainder) *Remainder = 0;
	}
	else if (Divisor.m_ndw == 1) {
		if (Divisor.IsNegative())
			Div((long)Divisor.m_value[0], Quotient, Remainder);
		else
			Div(Divisor.m_value[0], Quotient, Remainder);
	}
	else {
		int i;
		unsigned long mask;
		CBigInt tmpDividend(*this);
		CBigInt tmpDivisor(Divisor);
		CBigInt tmpQuotient, tmpRemainder;
		bool Neg = false;

		if ( tmpDividend.IsNegative() ) {
			Neg = true;
			tmpDividend.Negate();
		}
		if (tmpDivisor.IsNegative()) {
			Neg = !Neg;
			tmpDivisor.Negate();
		}

		for (i = tmpDividend.m_ndw -1 ; i >= 0; --i) {
			for (mask = 0x80000000; mask; mask >>= 1) {
				tmpRemainder <<= 1;
				if ( (tmpDividend.m_value[i] & mask) != 0 )
					tmpRemainder++;
				if (tmpRemainder >= tmpDivisor) {
					if (tmpQuotient.m_ndw <= i)
						tmpQuotient.ExpandTo(i + 1);
					tmpQuotient.m_value[i] |= mask;
					tmpRemainder -= tmpDivisor;
				}
			}
		}
		if (Quotient) {
			if (tmpQuotient.IsNegative())
				tmpQuotient.Expand(1);
			tmpQuotient.Optimize();
			*Quotient = tmpQuotient;
			if (Neg)
				Quotient->Negate();
		}
		if (Remainder) {
			if (tmpRemainder.IsNegative())
				tmpRemainder.Expand(1);
			if (IsNegative())
				tmpRemainder.Negate();
			tmpRemainder.Optimize();
			*Remainder = tmpRemainder;
		}
	}
}

// Faster Division for unsigned long sized divisor
void CBigInt::Div(unsigned long Divisor, CBigInt *Quotient, CBigInt *Remainder) const {
	if (Divisor == 0L) {
		_asm {
		  mov EAX, 0
		  div EAX
		}
	}
	else if (operator !()) {
		if (Quotient) *Quotient = 0;
		if (Remainder) *Remainder = 0;
	}
	else {
		unsigned __int64 tmpRemainder = 0;
		unsigned long *tmpQuotient = new unsigned long[m_ndw];
		CBigInt tmpDividend(*this);
		bool Neg;
		if ((Neg = IsNegative()) == true)
			tmpDividend.Negate();
		int nStartResult = 0;
		
		for (int i = m_ndw - 1; i >= 0; --i) {
			tmpRemainder <<= BITS;
			tmpRemainder |= tmpDividend.m_value[i];
			tmpQuotient[i] = (unsigned long)(tmpRemainder / Divisor);
			tmpRemainder %= Divisor;
		}
		if (Quotient) {
			delete [] Quotient->m_value;
			Quotient->m_value = tmpQuotient;
			Quotient->m_ndw = m_ndw;
			Quotient->Optimize();
			if (Neg)
				Quotient->Negate();
		}
		else
			delete [] tmpQuotient;
		if (Remainder) {
			*Remainder = tmpRemainder;
			if (Neg)
				Remainder->Negate();
		}
	}
}

void CBigInt::Div(long Divisor, CBigInt *Quotient, CBigInt *Remainder) const {
	if (Divisor < 0) {
		Div((unsigned long)-Divisor, Quotient, Remainder);
		if (Quotient)
			Quotient->Negate();
	}
	else
		Div((unsigned long)Divisor, Quotient, Remainder);
}

// Faster multiplication for unsigned long sized operand
CBigInt CBigInt::operator *(unsigned long Value) const {
	bool Neg;
	CBigInt tmpOp(*this);
	unsigned long carry = 0L;
	unsigned long *tmp;
	union {
		unsigned __int64 i64;
		unsigned long i32[2];
	} product;
	int i;

	if (tmpOp.IsNull())
		return tmpOp;
	if ((Neg = IsNegative()) == true)
		tmpOp.Negate();
	tmp = new unsigned long[tmpOp.m_ndw + 1];
	if (tmp == NULL) {
		tmpOp.MakeNull();
		return tmpOp;
	}
	memset(tmp, 0, (tmpOp.m_ndw + 1) * BYTES);

	product.i64 = 0;
	for (i = 0; i < tmpOp.m_ndw; ++i) {
		product.i64 = (unsigned __int64)tmpOp.m_value[i] * Value + product.i32[1];
		tmp[i] = product.i32[0];
	}
	delete [] tmpOp.m_value;

	if (product.i32[1])
		tmp[tmpOp.m_ndw] = product.i32[1];
	//else
		tmpOp.m_ndw++;
	tmpOp.m_value = tmp;
	if (tmpOp.IsNegative())
		tmpOp.Expand(1);
	tmpOp.Optimize();
	if (Neg)
		tmpOp.Negate();
	return tmpOp;
}

CBigInt CBigInt::operator *(long Value) const {
	if (Value < 0)
		return operator -().operator *((unsigned long)-Value);
	return operator *((unsigned long)Value);
}

CBigInt CBigInt::operator /(unsigned long Value) const {
	CBigInt Quotient;
	Div(Value, &Quotient, NULL);
	return Quotient;
}

CBigInt CBigInt::operator %(unsigned long Value) const {
	CBigInt Remainder;
	Div(Value, NULL, &Remainder);
	return Remainder;
}

CBigInt CBigInt::operator /(long Value) const {
	CBigInt Quotient;
	Div(Value, &Quotient, NULL);
	return Quotient;
}

CBigInt CBigInt::operator %(long Value) const {
	CBigInt Remainder;
	Div(Value, NULL, &Remainder);
	return Remainder;
}

CBigInt CBigInt::operator /(unsigned int Value) const {
	CBigInt Quotient;
	Div((unsigned long)Value, &Quotient, NULL);
	return Quotient;
}

CBigInt CBigInt::operator %(unsigned int Value) const {
	CBigInt Remainder;
	Div((unsigned long)Value, NULL, &Remainder);
	return Remainder;
}

CBigInt CBigInt::operator /(int Value) const {
	CBigInt Quotient;
	Div((long)Value, &Quotient, NULL);
	return Quotient;
}

CBigInt CBigInt::operator %(int Value) const {
	CBigInt Remainder;
	Div((long)Value, NULL, &Remainder);
	return Remainder;
}

int operator /(int A, const CBigInt &B) {
	if (B.m_ndw > 1) {
		if (B.m_ndw == 2)
			if (B.m_value[1] == 0)
				return A / B.m_value[0];
		return 0U;
	}
	return A / int(B.m_value[0]);
}

unsigned int operator /(unsigned int A, const CBigInt &B) {
	if (B.m_ndw > 1) {
		if (B.m_ndw == 2)
			if (B.m_value[1] == 0)
				return A / B.m_value[0];
		return 0;
	}
	return A / int(B.m_value[0]);
}

long operator /(long A, const CBigInt &B) {
	if (B.m_ndw > 1) {
		if (B.m_ndw == 2)
			if (B.m_value[1] == 0)
				return A / B.m_value[0];
		return 0UL;
	}
	return A / int(B.m_value[0]);
}

unsigned long operator /(unsigned long A, const CBigInt &B) {
	if (B.m_ndw > 1) {
		if (B.m_ndw == 2)
			if (B.m_value[1] == 0)
				return A / B.m_value[0];
		return 0L;
	}
	return A / int(B.m_value[0]);
}

__int64 operator /(const __int64 &A, const CBigInt &B) {
	if (B.m_ndw <= 2) {
		__int64 t = (__int64)B;
		return A / t;
	}
	else if (B.m_ndw == 3) {
		if (B.m_value[2] == 0) {
			unsigned __int64 t = (unsigned __int64)B;
			return A / t;
		}
	}
	return (__int64)0;
}

unsigned __int64 operator /(const unsigned __int64 A, const CBigInt &B) {
	if (B.m_ndw <= 2) {
		unsigned __int64 t = (unsigned __int64)B;
		return A / t;
	}
	else if (B.m_ndw == 3) {
		if (B.m_value[2] == 0) {
			unsigned __int64 t = (unsigned __int64)B;
			return A / t;
		}
	}
	return (__int64)0;
}

CBigInt operator %(int A, const CBigInt &B) {
	bool NegA = A < 0;
	bool NegB = B.IsNegative();
	
	if (B.m_ndw > 1) {
		if (NegA != NegB)
			return -B;
		return B;
	}
	return CBigInt(A % (int)B.m_value);
}

CBigInt operator %(unsigned int A, const CBigInt &B) {
	if (B.m_ndw > 1) {
		if (B.IsNegative())
			return -B;
		return B;
	}
	return CBigInt(A % (long)B.m_value[0]);
}

CBigInt operator %(long A, const CBigInt &B) {
	bool NegA = A < 0;
	bool NegB = B.IsNegative();
	
	if (B.m_ndw > 1) {
		if (NegA != NegB)
			return -B;
		return B;
	}
	return CBigInt(A % (long)B.m_value[0]);
}

CBigInt operator %(unsigned long A, const CBigInt &B) {
	if (B.m_ndw > 1) {
		if (B.m_value[1] != 0 || B.m_ndw > 2) {
			if (B.IsNegative())
				return -B;
			return B;
		}
	}
	return CBigInt(A % (long)B.m_value[0]);
}

CBigInt operator %(const __int64 &A, const CBigInt &B) {
	bool NegA = A < 0;
	bool NegB = B.IsNegative();
	
	if (B.m_ndw > 2) {
		if (NegA != NegB)
			return -B;
		return B;
	}
	return CBigInt(A % (__int64)B.m_value[0]);
}

CBigInt operator %(const unsigned __int64 A, const CBigInt &B) {
	if (B.m_ndw > 2) {
		if (B.m_value[2] != 0 || B.m_ndw > 3) {
			if (B.IsNegative())
				return -B;
			return B;
		}
	}
	return CBigInt(A % (__int64)B);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// String Parsing
// Decimal, Hex, Octal, and Binary are in seperate procedures, to improve performance
///////////////////////////////////////////////////////////////////////////////////////////////////////

CBigInt& CBigInt::FromDec(const char *szVal) {
	// Determine the number of valid characters
	const char *pStart = szVal;
	while (*pStart == ' ' || *pStart == '\t')
		pStart++;
	const char *pEnd = pStart;
	while ( *pEnd >= '0' && *pEnd <= '9')
		pEnd++;
	unsigned nLen = pEnd - pStart;
	*this = 0;
	if (IsNull()) return *this;
	if (nLen == 0)
		return *this;
	const unsigned long MAXDEC = (MAXLONG - 9) / 10;

	while (pStart < pEnd && m_value[0] <= MAXDEC) {
		m_value[0] = m_value[0] * 10 + *pStart - '0';
		pStart++;
	}
	while (pStart < pEnd) {
		unsigned long Power = 1;
		unsigned long dwTmp = 0;
		while (pStart < pEnd && Power <= MAXDEC) {
			dwTmp = dwTmp * 10 + *pStart - '0';
			pStart++;
			Power *= 10;
		}
		*this *= Power;
		*this += dwTmp;
		if (IsNull()) break;
	}
	return *this;
}

CBigInt& CBigInt::FromHex(const char *szVal) {
	// Determine the number of valid characters
	const char *pStart = szVal;
	while (*pStart == ' ' || *pStart == '\t')
		pStart++;
	const char *pEnd = pStart;
	while ( (*pEnd >= '0' && *pEnd <= '9') || (*pEnd >= 'A' && *pEnd <= 'F') || (*pEnd >='a' && *pEnd <= 'f'))
		pEnd++;
	unsigned nLen = pEnd - pStart;
	if (nLen == 0) {
		*this = 0;
		return *this;
	}
	// Calculate the size of the result
	long ndw = (nLen + NIBBLES - 1) / NIBBLES;
	if (ndw != m_ndw) {
		MakeNull();
		m_value = new unsigned long[m_ndw = ndw];
		if (m_value == NULL) {
			m_ndw = 0;
			return *this;
		}
	}
	memset(m_value, 0, m_ndw * BYTES);
	int nShift = 0;
	nLen = 0;
	while (pEnd > pStart) {
		pEnd--;
		if (*pEnd >= '0' && *pEnd <= '9')
			m_value[nLen] |= (*pEnd - '0') << nShift;
		else if (*pEnd >= 'A' && *pEnd <= 'F')
			m_value[nLen] |= (*pEnd - 'A' + 10) << nShift;
		else // if (*pEnd >= 'a' && *pEnd <= 'z')
			m_value[nLen] |= (*pEnd - 'a' + 10) << nShift;
		nShift += 4;
		if (nShift >= BITS) {
			nShift = 0;
			nLen++;
		}
	}
	if (IsNegative())
		Expand(1);
	return *this;
}

CBigInt& CBigInt::FromOct(const char *szVal) {
	const char *pStart = szVal;
	while (*pStart == ' ' || *pStart == '\t')
		pStart++;
	const char *pEnd = pStart;
	while (*pEnd >= '0' && *pEnd <= '7')
		pEnd++;
	int nLen = pEnd - pStart;
	if (nLen == 0) {
		*this = 0;
		return *this;
	}
	long ndw = (nLen * 3 + BITS - 1) / BITS;
	if (ndw != m_ndw) {
		if (m_ndw)
			delete [] m_value;
		m_value = new unsigned long[m_ndw = ndw];
		if (m_value == NULL) {
			m_ndw = 0;
			return *this;
		}
	}
	memset(m_value, 0, m_ndw * BYTES);
	int nShift = 0;
	nLen = 0;
	while (pEnd > pStart) {
		pEnd--;
		m_value[nLen] |= (*pEnd - '0') << nShift;
		if ((nShift + 3 ) >= BITS) {
			nShift = nShift + 3 - BITS;
			if (nLen < m_ndw)
				m_value[++nLen] = (*pEnd - '0') >> (3 - nShift);
		}
		else
			nShift += 3;
	}
	if (IsNegative())
		Expand(1);
	Optimize();
	return *this;
}

CBigInt& CBigInt::FromBin(const char *szVal) {
	const char *pStart = szVal;
	while (*pStart == ' ' || *pStart == '\t')
		pStart++;
	const char *pEnd = pStart;
	while (*pEnd >= '0' && *pEnd <= '1')
		pEnd++;
	unsigned nLen = pEnd - pStart;
	if (nLen == 0) {
		*this = 0;
		return *this;
	}
	long ndw = (nLen * 3 + BITS - 1) / BITS;
	if (ndw != m_ndw) {
		if (m_ndw)
			delete [] m_value;
		m_value = new unsigned long[m_ndw = ndw];
		if (m_value == NULL) {
			m_ndw = 0;
			return *this;
		}
	}
	memset(m_value, 0, m_ndw * BYTES);
	int nShift = 0;
	nLen = 0;
	while (pEnd > pStart) {
		pEnd--;
		if (*pEnd == '1')
			m_value[nLen] |= 1 << nShift;
		if (++nShift >= BITS) {
			nShift = 0;
			nLen++;
		}
	}
	if (IsNegative())
		Expand(1);
	Optimize();
	return *this;
}

inline int BaseDigitValue(char digit, int Radix) {
	if (digit >= '0' || digit <= '9') {
		digit -= '0';
		return digit < Radix ? digit : -1;
	}
	if (digit >= 'a' && digit <= 'z') {
		digit -= ('a' - 10);
		return digit < Radix ? digit : -1;
	}
	if (digit >= 'A' && digit <= 'Z') {
		digit -= ('A' - 10);
		return digit < Radix ? digit : -1;
	}
	return -1;
}

CBigInt& CBigInt::FromString(const char *szVal, int Radix /* = 10 */ ) {
	// Use the fater conversions for Binary, Octal, or Hexadecimal
	if (Radix == 2)
		return FromBin(szVal);
	if (Radix == 8)
		return FromOct(szVal);
	if (Radix == 10)
		return FromDec(szVal);
	if (Radix == 16)
		return FromHex(szVal);
	if (Radix > 36)
		return (*this = 0);
	while (*szVal == ' ' || *szVal == '\t')
		szVal++;
	bool neg = false;
	if (Radix == 10) {
		if (*szVal == '+')
			szVal++;
		else if (*szVal == '-') {
			neg = true;
			szVal++;
		}
	}
	*this = 0;
	int n;
	while ((n = BaseDigitValue(*szVal, Radix)) >= 0) {
		*this *= Radix;
		*this += n;
		if(IsNull())
			return *this;
		szVal++;
	}
	if (IsNegative())
		Expand(1);
	if (neg)
		Negate();
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Formatting Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////
char* CBigInt::Format(char *szBuf, unsigned long cbBuf, unsigned long *pNeeded, unsigned int Radix /* = 10 */) const {
	unsigned long cbNeeded = 0;
	CBigInt tmp(*this);
	CBigInt Remainder;
	bool neg = false;

	if (IsNull()) {
		cbNeeded = 5;
		if (cbBuf >= cbNeeded && szBuf != NULL)
			strcpy(szBuf, "Null");
		else
			szBuf = NULL;
	}
	else {
		if (m_ndw == 1 && m_value[0] == 0) {
			*szBuf = '0';
			*(szBuf + 1) = 0;
			if (pNeeded) *pNeeded = 2;
			return szBuf;
		}
		if (tmp.IsNegative()) {
			if (Radix == 10) {
				tmp.Negate();
				neg = true;
			}
			else
				tmp.Expand(1);
		}
		else {
			// The following is broken down into seperate division operations, the second one
			// being straight long integer division. This minimises the CBigInt divisions that 
			// are needed, improving performance with large numbers considerably.
			int Power = 1;
			unsigned long Divisor = (unsigned long)Radix;
			while (MAXULONG / Divisor >= Radix) {
				Power++;
				Divisor *= Radix;
			}

			do	{
				tmp.Div(Divisor, tmp, Remainder);
				for (int i = 0; i < Power; ++i) {
					// Mathemtical Note: Remainder will always be less than MAXULONG
					char rem = (char)(Remainder.m_value[0] % Radix);
					Remainder.m_value[0] /= Radix;
					if (!tmp && !rem && !Remainder)
						break;	// Finished - and we dont want leading zeros
					if (cbNeeded < cbBuf) {
						// Convert to a character
						if (rem <= 9)
							*(szBuf + cbNeeded) = (rem + '0');
						else 
							*(szBuf + cbNeeded) = (char)(rem - 10 + 'A');
					}
					cbNeeded++;
				}
			} while (tmp);
			if (neg) {
				if (cbNeeded < cbBuf)
					*(szBuf + cbNeeded) = '-';
				cbNeeded++;
			}
			if (cbNeeded < cbBuf) 
				*(szBuf + cbNeeded) = '\0';
			else if (cbBuf)
				*(szBuf + cbBuf - 1) = '\0';
			cbNeeded++;
			if (szBuf)
				strrev(szBuf);
		}
	}
	if (pNeeded)
		*pNeeded = cbNeeded;
	return szBuf;
}

char* CBigInt::Format(unsigned int Radix /* = 10 */ ) const {
	if (m_stringBuf) {
		delete [] m_stringBuf;
		m_stringBuf = NULL;
	}
	// Calculate how many bytes needed
	unsigned long cbNeeded = 1;	// At least 1 for the trailing null character
	if (IsNull())
		cbNeeded = 5;
	else if (operator !()) {
		cbNeeded = 2;
	}
	else {
		int Power = 1;
		unsigned long Divisor = (unsigned long)Radix;
		while (MAXULONG / Divisor > Radix) {
			Power++;
			Divisor *= Radix;
		}
		CBigInt tmp(*this);
		if (tmp.IsNegative()) {
			tmp.Expand(1);
			if (Radix == 10)
				cbNeeded++;
		}
		while (tmp > Divisor) {
			tmp /= Divisor;
			cbNeeded += Power;
		}
		while (tmp.m_value[0]) {
			tmp.m_value[0] /= Radix;
			cbNeeded++;
		}
	}
	m_stringBuf = new char [cbNeeded];
	return Format(m_stringBuf, cbNeeded, NULL, Radix);
}