#ifndef EG_INTERFACE_GUARD_49E048BB_074A_4B1D_AC11_867DAA82CB8F__2019_Jun_13_02_20_12
#define EG_INTERFACE_GUARD_49E048BB_074A_4B1D_AC11_867DAA82CB8F__2019_Jun_13_02_20_12
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//root
template<>
void __eg_root< void >::operator()() const
{
  Camera cam = Camera.Start();
Camera::Orbit orbit = cam.Orbit.Start();
orbit.rate( 0.1f );
orbit.radius( 8.0f );
  x( -1 );
y( -1 );
  while( !Done() )
{
    cinder::gl::clear();
    cinder::gl::enableDepthRead();
    cinder::gl::enableDepthWrite();
    cinder::gl::setMatrices( perspective() );

    eg::sleep();
}
}

//root::Camera
template<>
template<>
void __eg_root< void >::__eg_Camera< void >::operator()() const
{
  eye( cinder::vec3( 8, 8, 2 ) );
  while( true )
    {
        perspective.Get().lookAt( eye(), cinder::vec3( 0 ) );
        eg::sleep();
    }
}

//root::Camera::Orbit
template<>
template<>
template<>
void __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::operator()() const
{
  while( true )
        {
            float fAngle = clock::ct() * rate() * M_PI * 2.0f;
            cinder::vec2 v( sin( fAngle ), cos( fAngle ) );
            eye( cinder::vec3( v.x, v.y, 2 ) * radius() );
            eg::sleep();
        }
}

//root::Cube
template<>
template<>
void __eg_root< void >::__eg_Cube< void >::operator()() const
{
  while( true )
    {
        cinder::gl::drawColorCube( position(), size() );
        eg::sleep();
    }
}

//root::CreateCube
template<>
template<>
void __eg_root< void >::__eg_CreateCube< void >::operator()() const
{
  Cube c = Cube.Start();
    c.size( cinder::vec3( 3, 3, 3 ) );
    c.position( cinder::vec3( x() * 4, y() * 4, 0 ) );
    
    x( x() + 1 );
    if( x() == 2 )
    {
        x( -1 );
        y( y() + 1 );
        if( y() == 2 )
            y( -1 );
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
