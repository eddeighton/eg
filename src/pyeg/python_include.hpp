
#ifndef PYTHON_INCLUDE_1_12_2016
#define PYTHON_INCLUDE_1_12_2016

//#include <patchlevel.h>

//#define Py_LIMITED_API PY_VERSION_HEX

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include "Python.h"
//#include "boost/python/detail/wrap_python.hpp"
#include "object.h"

#include "common/require_semicolon.hpp"

/*
  #include "python.h" will cause linking python27_d.lib due to #pragma
  the boost wrap utility works around this
*/


//#define PYTHON_ERROR( msg ) \
//    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
//        std::ostringstream _os; \
//        _os << "FILE " << __FILE__ << " LINE:" << __LINE__ /*<< " FUNCTION:" << BOOST_CURRENT_FUNCTION*/ << "\nMSG:" << msg; \
//        PyErr_SetString( ::Editor::Editor::m_pError, _os.str().c_str() );)
        

#endif //PYTHON_INCLUDE_1_12_2016



/*
#include <numpy/arrayobject.h>
#include "c:/Anaconda3/Lib/site-packages/numpy/core/include/numpy/numpyconfig.h"
#include "c:/Anaconda3/Lib/site-packages/numpy/core/include/numpy/arrayobject.h"

#pragma comment( lib, "c:/Anaconda3/Lib/site-packages/numpy/core/lib/npymath.lib" )
*/