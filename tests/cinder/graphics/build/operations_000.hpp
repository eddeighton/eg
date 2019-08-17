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
eg::sleep();
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

//root::Line::Up
template<>
template<>
template<>
void __eg_root< void >::__eg_Line< void >::__eg_Up< void >::operator()() const
{
  float fTarget = start().y - Distance();
    while( start().y > fTarget )
    {
        float f = Speed() * clock::dt();
        start(  glm::vec2( start().x, start().y - f ) );
        end(    glm::vec2( end().x, end().y - f ) );
        eg::sleep();
    }
}

//root::Line::Down
template<>
template<>
template<>
void __eg_root< void >::__eg_Line< void >::__eg_Down< void >::operator()() const
{
  float fTarget = start().y + Distance();
    while( start().y < fTarget )
    {
        float f = Speed() * clock::dt();
        start(  glm::vec2( start().x, start().y + f ) );
        end(    glm::vec2( end().x, end().y + f ) );
        eg::sleep();
    }
}

//root::Line::Left
template<>
template<>
template<>
void __eg_root< void >::__eg_Line< void >::__eg_Left< void >::operator()() const
{
  float fTarget = start().x - Distance();
    while( start().x > fTarget )
    {
        float f = Speed() * clock::dt();
        start(  glm::vec2( start().x - f, start().y ) );
        end(    glm::vec2( end().x - f, end().y ) );
        eg::sleep();
    }
}

//root::Line::Right
template<>
template<>
template<>
void __eg_root< void >::__eg_Line< void >::__eg_Right< void >::operator()() const
{
  float fTarget = start().x + Distance();
    while( start().x < fTarget )
    {
        float f = Speed() * clock::dt();
        start(  glm::vec2( start().x + f, start().y ) );
        end(    glm::vec2( end().x + f, end().y ) );
        eg::sleep();
    }
}

//root::Line::Idle
template<>
template<>
template<>
void __eg_root< void >::__eg_Line< void >::__eg_Idle< void >::operator()() const
{
  unsigned int ms = 1000 * Distance() / Speed();
    eg::sleep( 1ms * ms );
}

//root::Line::Sequencer
template<>
template<>
template<>
void __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >::operator()() const
{
  while( true )
        {
            for( int i : Sequence() )
            {
                switch( i )
                {
                    case 0:     Up();       break;
                    case 1:     Right();    break;
                    case 2:     Down();     break;
                    case 3:     Left();     break;
                    default:    Idle();     break;
                }
            }
            eg::sleep();
        }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
