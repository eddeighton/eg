#ifndef EG_INTERFACE_GUARD_0E316C54_49AF_4B0F_973F_00A1470EB3A9__2019_Jun_13_02_09_22
#define EG_INTERFACE_GUARD_0E316C54_49AF_4B0F_973F_00A1470EB3A9__2019_Jun_13_02_09_22
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//root
template<>
void __eg_root< void >::operator()() const
{
  bool bContinue = true;
while( bContinue )
{
	cinder::gl::clear();
  while( std::optional< cinder::app::InputEvent > eventOpt = Input::getEvent() )
    {
        const cinder::app::InputEvent& e = eventOpt.value();
        switch( e.type )
        {
            case cinder::app::InputEvent::eMouseDown:
                {
                    lastPos( std::optional< cinder::vec2 >( e.mouseEvent.getPos() ) );
                    if( Circle c = Circle.Start() )
                    {
                        c.position( lastPos().value() );
                        c.color( cinder::Color( cinder::CM_HSV, fmod( clock::ct(), 1.0f ), 1, 1 ) );
                    }
                }
                break;
            case cinder::app::InputEvent::eMouseUp:
                {
                    lastPos( std::optional< cinder::vec2 >() );
                }
                break;
            case cinder::app::InputEvent::eMouseMove:
                break;
            case cinder::app::InputEvent::eMouseDrag:
                if( lastPos() )
                {
                    const cinder::vec2 mousePos( e.mouseEvent.getPos() );
                    
                    const cinder::vec2 dir = mousePos - lastPos().value();
                    const float fSteps = glm::length( dir ) * 0.02f;
                    for( float i = 0.0f; i < fSteps; ++i )
                    {
                        if( Circle c = Circle.Start() )
                        {
                            c.position( lastPos().value() + ( dir * ( i / fSteps ) ) );
                            c.color( cinder::Color( cinder::CM_HSV, fmod( clock::ct(), 1.0f ), 1, 1 ) );
                        }
                    }
                    lastPos( std::optional< cinder::vec2 >( mousePos ) );
                    
                    //try to start a circle
                    if( Circle c = Circle.Start() )
                    {
                        c.position( lastPos().value() );
                        c.color( cinder::Color( cinder::CM_HSV, fmod( clock::ct(), 1.0f ), 1, 1 ) );
                    }
                }
                else
                {
                    LOG( "Mouse Drag: no lastpos"  );
                }
                break;
            case cinder::app::InputEvent::eMouseWheel:
                break;
            case cinder::app::InputEvent::eKeyDown:
                if( e.keyEvent.getChar() == ' ' )
                {
                    Stop();
                    bContinue = false;
                }
                else if( e.keyEvent.getChar() == 'c' )
                {
                    float fCounter = 0.0f;
                    for( Circle c : Circle.Range() )
                    {
                        c.ColourChanger.Start();
                        c.ColourChanger.offset( fCounter );
                        fCounter += 0.01;
                    }
                }
                else if( e.keyEvent.getChar() == 'g' )
                {
                    for( Circle c : Circle.Range() )
                    {
                        c.Shrink.Stop();
                        c.Grow.Start();
                    }
                }
                else if( e.keyEvent.getChar() == 's' )
                {
                    for( Circle c : Circle.Range() )
                    {
                        c.Grow.Stop();
                        c.Shrink.Start();
                    }
                }
                else if( e.keyEvent.getChar() == 'm' )
                {
                    const float numCircles = eg::count( Circle.Range() );
                    float index = 0;
                    for( Circle c : Circle.Range() )
                    {
                        const float angle = ( ++index / numCircles ) * M_PI * 2.0f;
                        c.Move.Start().direction( cinder::vec2( cos( angle ), sin( angle ) ) );
                        c.Move.speed( 100.0f );
                    }
                }
                break;
            case cinder::app::InputEvent::eKeyUp:
                break;
        }
    }
    
    eg::sleep();
}
}

//root::Circle
template<>
template<>
void __eg_root< void >::__eg_Circle< void >::operator()() const
{
  max_size( 80.0f );
    max_lifetime( 8.0f );
    
    size( max_size() / 2.0f );
    starttime( clock::ct() );
    lifetime( clock::ct() + max_lifetime() );
    
    //draw the circle
    while( clock::ct() < lifetime() )
    {
        cinder::gl::color( color() ); // red
        cinder::gl::drawSolidCircle( position(), size() );
        eg::sleep();
    }
}

//root::Circle::Shrink
template<>
template<>
template<>
void __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >::operator()() const
{
  start( clock::ct() );
        while( true )
        {
            float fRatio = ( clock::ct() - start() ) / ( max_lifetime() / 16.0f );
            if( fRatio > 1.0f ) break;
            size( max_size() * ( 1.0f - fRatio ) );
            eg::sleep();
        }
}

//root::Circle::Grow
template<>
template<>
template<>
void __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >::operator()() const
{
  start( clock::ct() );
        while( true )
        {
            float fRatio = ( clock::ct() - start() ) / ( max_lifetime() / 16.0f );
            if( fRatio > 1.0f ) break;
            size( max_size() * fRatio );
            eg::sleep();
        }
}

//root::Circle::Move
template<>
template<>
template<>
void __eg_root< void >::__eg_Circle< void >::__eg_Move< void >::operator()() const
{
  while( true )
        {
            position( position() + ( direction() * clock::dt() ) * speed() );
            eg::sleep();
        }
}

//root::Circle::ColourChanger
template<>
template<>
template<>
void __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >::operator()() const
{
  while( true )
        {
            color( cinder::Color( cinder::CM_HSV, fmod( clock::ct() + offset(), 1.0f ), 1, 1 ) );
            eg::sleep();
        }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
