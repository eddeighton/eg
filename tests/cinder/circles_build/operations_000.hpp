
//root
template<>
EGCoroutine __interface_root< void >::operator()()
{
  int numCircles = 2048;
for( int index = 0; index < numCircles; ++index ) 
{
    Circle circle = Circle.Start();
    Circle::ColorChanger cc = circle.ColorChanger.Start();
}
Spiral.Start();

while( true )
{
    while( std::optional< cinder::app::KeyEvent > k = Input::getKeyEvent() )
    {
        if( k.value().getChar() == 's')
        {
            if( Spiral.Empty() )
                Spiral.Start();
        }
        else if( k.value().getChar() == 'd')
        {
            if( !Spiral.Empty() )
                Spiral.Stop();
        }
    }
	gl::clear();
    SLEEP;
}

  co_return;
}

//root::Circle
template<>
template<>
EGCoroutine __interface_root< void >::__interface_Circle< void >::operator()()
{
  while( true )
    {
        gl::color( color() ); // red
        gl::drawSolidCircle( position(), size() );
        SLEEP;
    }

  co_return;
}

//root::Circle::ColorChanger
template<>
template<>
template<>
EGCoroutine __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >::operator()()
{
  while( true )
        {
            f( fmodf( f() + clock::dt() * 0.3, 1.0f ) );
            color( Color( CM_HSV, f(), 1, 1 ) );
            SLEEP;
        }

  co_return;
}

//root::Spiral
template<>
template<>
EGCoroutine __interface_root< void >::__interface_Spiral< void >::operator()()
{
  while( true )
    {
        int numCircles = 0;
        for( Circle c : Circle.Range() )
        {
            ++numCircles;
        }
        
        int index = 0;
        const vec2 center = getWindowCenter();
        float radius = 1.0f;
        for( Circle circle : Circle.Range() )
        {
            float rel = ( clock::ct() / 10 ) + ( index / (float)numCircles );
            float angle = rel * M_PI * 10;
            vec2 offset( cos( angle ), sin( angle ) );
        
            circle.position( center + offset * radius * 4.0f );
            circle.size( 25.0f );
            
            radius = radius + 0.1f;
        
            ++index;
        }
        SLEEP;
    }

  co_return;
}
