#ifndef EG_INTERFACE_GUARD_OPERATIONS
#define EG_INTERFACE_GUARD_OPERATIONS
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//root
template<>
void __eg_root< void >::operator()() const
{
  std::cout << "startup" << std::endl;

resource::set< eg::TextResourceHandler >( "test.py" );

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
    eg::sleep( 1s );
    
}
}

//root::Test
template<>
template<>
void __eg_root< void >::__eg_Test< void >::operator()() const
{
  std::cout << "Test called" << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
