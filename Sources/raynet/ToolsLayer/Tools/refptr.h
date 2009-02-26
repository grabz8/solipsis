#ifndef __REFPTR_AND_LINKED_LIST__
#define __REFPTR_AND_LINKED_LIST__






namespace Tools
{



//-----------------------LINKED LIST--------------------------//			
			//- new node attaches before the attachable node	
	class LList{

		/**
			Design Note : Mutable list pointers are const valid
			- A const LList has no meaning.. by definition ... LList objects are rerouted
			.. LList is 'observably' const - meaning that mutations are encapsulated
		*/
		mutable const LList* m_p_prev,*m_p_next;

		void attach(const LList& _attachToMe){
			this->m_p_next = &_attachToMe;
			this->m_p_prev = _attachToMe.m_p_prev;
			_attachToMe.m_p_prev = this;
						
			if ( m_p_prev){
				m_p_prev->m_p_next = this;
			}	
		}

		void release(){
			if( m_p_prev != NULL && m_p_next != NULL){
				m_p_prev->m_p_next = m_p_next;
				m_p_next->m_p_prev = m_p_prev;
			}
			else if (  m_p_prev != NULL){
				m_p_prev->m_p_next = m_p_next;
			}
			else if( m_p_next != NULL){
				m_p_next->m_p_prev = m_p_prev;
			}
			else{ // No other pointers on object..so..
				//nothing to say
			}
			
			m_p_prev = NULL;
			m_p_next = NULL;
		}
	public:
		
		LList() : m_p_prev(0), m_p_next(0){}
		LList( const LList& _attachToMe) {
			attach( _attachToMe);
		}
		LList& operator =( const LList& _attachToMe) {
			if ( &_attachToMe != this){
				release();
				attach( _attachToMe);
			}
			return *this;
		}
		~LList(){
			release();
		}
		bool bAlone(){
			return !( m_p_prev || m_p_next);
		}	
	};
	//-----------------------LINKED LIST--------------------------//
	









	//-----------------------REF PTR--------------------------//
	/*
	*  RefPtr
	*
	*  This is a smart Ptr that performs reference counting
	*  to ensure that as long as there is a Ptr object, the pointee remains
	*  ..much like Java.
	*
	*  DESIGN LIMITATION: This class DOES NOT SUPPORT multithreading!!
	*                     That is.. two threads referencing the same pointed object using
	*                     two different refPtr objects.
	*
	* CONSTNESS: .. const RefPtr<T> returns a T
	*               RefPtr<const T> returns a const T
	*/

	
	template <class T> class RefPtr;
	
	template < class T >
		class RefPtr {
		
	private:
		
		LList m_linkedPtrs;

		//pointee
		T* m_ptr;
		
		
		//------helpers
		void _leaveList(){
			
			if ( m_linkedPtrs.bAlone() )
			{
				delete m_ptr;			
				m_ptr = NULL;			
			}	
			
		}
		
		//------		
		
	public:
		
		RefPtr(T* _p = 0):  m_ptr(_p){}
		//------
		template< class T2>
			RefPtr(const RefPtr<T2>& _p)		  : m_ptr(NULL){

			m_linkedPtrs = _p.m_linkedPtrs;
			
			//Design Note: const_cast is used to strip the constness of the
			//             pointer ONLY. The pointee remains constful.
			//             This is to satify the const_iterator::operator-> method
			//             so that a const RefPtr may still update a non-const RefPtr
			//             ..that is we want assignments to work!!
			m_ptr = ( const_cast< RefPtr<T2>& >(_p) ).m_ptr;
			
		}


		//------ destructor
		~RefPtr()
		{
			_leaveList();
		}

		//------ assignment 		
		template <class T2>
			RefPtr& operator =( const RefPtr<T2>& _p)
		{
			if ( &_p != this){
				_leaveList();
				m_linkedPtrs =_p.m_linkedPtrs;
				//point to same pointee
				m_ptr = _p.m_ptr;
				
			}
			return *this;
		}
		//------
		RefPtr& operator =( T* _p){
			if ( _p != m_ptr){
				_leaveList();
				m_ptr = _p;
			}
			return *this;
		}
		//------  Comparison
		template < class T1>
			bool operator==( const RefPtr<T1>& _p1) const{
			return (void*)m_ptr == (void*)_p1.m_ptr;
		}
		//------
		template < class T1>
			bool operator==( T1* _p)const {
			return (void*)m_ptr == (void*)_p;
		}
		//------ Operators
		/*  inline T* operator->() {   //const means I can always dereference a RefPtr object
		return  m_ptr;
}*/
		//---
		inline  T* operator->() const{   //const means I can always dereference a RefPtr object
			return  m_ptr;
		}
		//---
		inline operator bool() const {
			return  m_ptr != NULL;
		}
		//---
		/* inline T& operator*() {
		return  *m_ptr;
};*/
		//---
		inline  T& operator*() const{
			return  *m_ptr;
		};
		//---
		template < class T1>
			inline bool operator < ( const RefPtr<T1>& _ptr) const{
			return    (void *)m_ptr < (void *)_ptr.m_ptr ;
		}
		
	
};


}

#endif