//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.


//ignor clang warnings

#pragma warning( disable : 4146) // unary minus operator applied to unsigned type,
#pragma warning( disable : 4141) //'inline': used more than once
#pragma warning( disable : 4141) //'argument': conversion from 'uint64_t' to 'size_t'
#pragma warning( disable : 4244) //'argument': conversion from 'uint64_t' to 'uint32_t'
#pragma warning( disable : 4996) //
//warning C4996: 'std::iterator<IteratorCategoryT,T,DifferenceTypeT,PointerT,ReferenceT >::reference': 
//warning STL4015: The std::iterator class template (used as a base class to provide typedefs) is deprecated in C++17. 
//(The <iterator> header is NOT deprecated.) 
//The C++ Standard has never required user-defined iterators to derive from std::iterator. 
//To fix this warning, stop deriving from std::iterator and start providing publicly accessible typedefs named iterator_category, 
//value_type, difference_type, pointer, and reference. 
//Note that value_type  is required to be non-const, even for constant iterators. 
//You can define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING 
//or _SILENCE_ALL_CXX17_DEPR ECATION_WARNINGS to acknowledge that you have received this warning. 
#pragma warning( disable : 4291) //no matching operator delete found
