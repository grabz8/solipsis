#ifndef _PUSH_BRANCH_MODULE_H_
#define _PUSH_BRANCH_MODULE_H_

#include <map>
#include <string>

#include "Module.h"

/**
 * Definition of a L-System Terminal Push Branch Module
 * @author		Vincent Huiban
 */

class PushBranchModule : public Module 
{
	
public:
		
	/** Constructor. */
	PushBranchModule( const Symbol s = Module::module2symbol( PUSH_BRANCH ) ) 
		: Module( s ) {};

	/** Destructor. */
	virtual ~PushBranchModule() {};
			
	virtual void produce( std::vector< Module* >& successors );
		
protected:
	
	arVec3d m_position;
	
	double m_length;

	double m_width;

};

#endif // _ROTATE_MODULE_H_
