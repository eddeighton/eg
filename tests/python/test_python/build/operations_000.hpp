#ifndef EG_INTERFACE_GUARD_OPERATIONS
#define EG_INTERFACE_GUARD_OPERATIONS
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//root
template<>
void __eg_root< void >::operator()() const
{
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
  std::cout << "startup" << std::endl;

resource::set< eg::TextResourceHandler >( "test.py" );
  Python.Start();

while( !Done() )
    eg::sleep();
}

//root::Test
template<>
template<>
void __eg_root< void >::__eg_Test< void >::operator()() const
{
  std::cout << "Test called" << std::endl;
}

//root::Python
template<>
template<>
void __eg_root< void >::__eg_Python< void >::operator()() const
{
  bContinue( true );

    while( bContinue() )
    {
        eg::TextResource::Ptr pText = resource::get< eg::TextResource >( "test.py" );
        try
        {
            pybind11::exec( pText->get() );
        }
        catch( std::exception& e )
        {
            std::cout << e.what() << std::endl;
        }
        eg::sleep( 500ms );
        
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
