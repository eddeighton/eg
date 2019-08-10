#ifndef EG_INTERFACE_GUARD_OPERATIONS
#define EG_INTERFACE_GUARD_OPERATIONS
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//root
template<>
void __eg_root< void >::operator()() const
{
  cinder::app::getWindow()->setTitle( "Awesome" );
cinder::app::getWindow()->setSize( 800, 800 );
cinder::app::setWindowPos( 100, 100 );
std::cout << "Hello World" << std::endl;
  //render loop
while( true )
{
    cinder::gl::clear();
    eg::sleep();
}
}

//root::Line
template<>
template<>
void __eg_root< void >::__eg_Line< void >::operator()() const
{
  while( true )
    {
        glLineWidth( width() );
        cinder::gl::color( colour().x, colour().y, colour().z );
        cinder::gl::drawLine( start(), end() );
        eg::sleep();
    }
}

//root::Line::Move
template<>
template<>
template<>
void __eg_root< void >::__eg_Line< void >::__eg_Move< void >::operator()() const
{
  while( true )
        {
            start( start() + dir() * clock::dt() );
            end( end() + dir() * clock::dt() );
            
            if( std::max( start().x, end().x ) > screen().x )
            {
                dir( glm::vec2( -dir().x, dir().y ) );
            }
            else if( std::min( start().x, end().x ) < 0 )
            {
                dir( glm::vec2( -dir().x, dir().y ) );
            }
            if( std::max( start().y, end().y ) > screen().y )
            {
                dir( glm::vec2( dir().x, -dir().y ) );
            }
            else if( std::min( start().y, end().y ) < 0 )
            {
                dir( glm::vec2( dir().x, -dir().y ) );
            }
            
            eg::sleep();
        }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
