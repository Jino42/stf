//
// Created by ntoniolo on 19/06/2019.
//

#ifndef NTL_PS_NTL_HPP
#define NTL_PS_NTL_HPP

#ifndef __FUNCTION_NAME__
	#ifdef WIN32   //WINDOWS
		#define __FUNCTION_NAME__   __FUNCTION__
	#else          //*NIX
		#define __FUNCTION_NAME__   __PRETTY_FUNCTION__
	#endif
#endif

#define __CL_ENABLE_EXCEPTIONS
#include "Cl/cl.hpp"
#include <glm/glm.hpp>

#endif //NTL_PS_NTL_HPP

