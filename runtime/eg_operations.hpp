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


#ifndef EG_OPERATION_TYPES
#define EG_OPERATION_TYPES

struct [[clang::eg_type( eg::id_Imp_NoParams    )]] __eg_ImpNoParams;   //id_Imp_NoParams   
struct [[clang::eg_type( eg::id_Imp_Params      )]] __eg_ImpParams;     //id_Imp_Params   
struct [[clang::eg_type( eg::id_Get             )]] Get;                //id_Get           
struct [[clang::eg_type( eg::id_Stop            )]] Stop;               //id_Stop         
struct [[clang::eg_type( eg::id_Pause           )]] Pause;              //id_Pause        
struct [[clang::eg_type( eg::id_Resume          )]] Resume;             //id_Resume        
struct [[clang::eg_type( eg::id_Range           )]] Range;              //id_Range        

#endif //EG_OPERATION_TYPES