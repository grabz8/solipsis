#ifndef __RINGING_TIMER___HEADER_H__
#define __RINGING_TIMER___HEADER_H__


#include <vector>
#include "P2PTools.h"
#include "limits.h"

namespace Tools
{
	/*
	Class: Timer

	Purpose: Signals events in terms of time

	Limits: Identifiers are sequential ints.. {0,1,2 ... SIZE} 
	-simplifies design 
	- faster than a map (constant access.. less page faults)
	*/

	DEFINE_EXCEPTION(NoEntryInTimerException)

		template <int SIZE>
	class Timer
	{

		//-----------------------
		// Purpose: set the time_t value to max..
		// Problem: Time_t may be 32 or 64 bits.. (the swine!)
		// Solution: let templates solve that one.. very quick..faster than a memset		
		template <int CharsToSet> void MaxValueImpl( void*);
		template <> void MaxValueImpl<4>(void* pSetMe) 
		{     
			unsigned char* pSetChar = (unsigned char*)pSetMe;
			pSetChar[0]=0xFF;
			pSetChar[1]=0xFF;
			pSetChar[2]=0xFF;
			pSetChar[3]=0xFF;
		}
		template <> void MaxValueImpl<8>(void* pSetMe) 
		{     
			unsigned char* pSetChar = (unsigned char*)pSetMe;
			pSetChar[0]=0xFF;
			pSetChar[1]=0xFF;
			pSetChar[2]=0xFF;
			pSetChar[3]=0xFF;
			pSetChar[4]=0xFF;
			pSetChar[5]=0xFF;
			pSetChar[6]=0xFF;
			pSetChar[7]=0xFF;
		}
		template < typename T> 
		T MaxValue()
		{ 
			T t;
			MaxValueImpl< sizeof(T) >( &t);
			return t;
		}
		//-----------------------


	public:
		typedef int Identifier;
		typedef int TimeOut;

	private:
		
		struct AlarmEvent
		{
			TimeOut m_i_period;
			time_t m_t_scheduled;
			bool m_b_activated;

			AlarmEvent( ):
			m_i_period(0),
			m_t_scheduled( 0),
			m_b_activated( false)
			{			
			}

			AlarmEvent( TimeOut _Period):
			m_i_period(_Period),
			m_t_scheduled( ::time(NULL) + _Period ),
			m_b_activated( true)
			{			
			}

		};
		AlarmEvent m_arr_timeouts[	SIZE];
		


	public:

		Timer(){}

		//-----------------------
		void  add( Identifier _id, TimeOut _t)
		{		
			m_arr_timeouts[_id].m_i_period = _t;
			m_arr_timeouts[_id].m_t_scheduled = ::time(NULL) + _id ;
			m_arr_timeouts[_id].m_b_activated = true;
		}		

		//-----------------------
		unsigned int timeLeft() 
		{
			time_t soonest = MaxValue<time_t >();
			bool bSomeActivated = false;

			for ( Identifier id = 0; id < SIZE; id++)
			{
				if( m_arr_timeouts[id].m_b_activated )
				{
					bSomeActivated = true;
					soonest = std::min<time_t>( m_arr_timeouts[id].m_t_scheduled , soonest );
				}			
			}
			if ( !bSomeActivated )
				THROW(NoEntryInTimerException," Timer needs some entries");

			return (unsigned int)( soonest - ::time(NULL));
		}


		//-----------------------
		bool isRinging(Identifier _id)
		{
			return m_arr_timeouts[_id].m_t_scheduled < ::time(NULL);	
		}
	};




}

#endif