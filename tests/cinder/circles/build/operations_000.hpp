#ifndef EG_INTERFACE_GUARD_OPERATIONS
#define EG_INTERFACE_GUARD_OPERATIONS
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//root
template<>
void __eg_root< void >::operator()() const
{
  cinder::app::getWindow()->setSize( 1024, 1024 );
cinder::app::setWindowPos( 100, 100 );
  morphSpeed( 0.1 );
  imgui.Start();

while( true )
{
	cinder::gl::clear();
    eg::sleep();
}
}

//root::Shape
template<>
template<>
void __eg_root< void >::__eg_Shape< void >::operator()() const
{
}

//root::Shape::Default
template<>
template<>
template<>
void __eg_root< void >::__eg_Shape< void >::__eg_Default< void >::operator()() const
{
  color( cinder::Color( cinder::CM_HSV, 0.5f, 1, 1 ) );
        position( cinder::app::getWindowCenter() );
        size( 25.0f );
}

//root::Shape::ColorChanger
template<>
template<>
template<>
void __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >::operator()() const
{
  while( true )
        {
            f( fmodf( f() + clock::dt() * 0.3, 1.0f ) );
            color( cinder::Color( cinder::CM_HSV, f(), 1, 1 ) );
            eg::sleep();
        }
}

//root::Shape::Morph
template<>
template<>
template<>
void __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >::operator()() const
{
  while( true )
        {
            positionActual( positionActual() + 
                ( position() - positionActual() ) * clock::dt() * morphSpeed() );
            eg::sleep();        
        }
}

//root::StrokedCircle
template<>
template<>
void __eg_root< void >::__eg_StrokedCircle< void >::operator()() const
{
  while( true )
    {
        cinder::gl::color( color() );
        cinder::gl::drawStrokedCircle( positionActual(), size() );
        eg::sleep();
    }
}

//root::Circle
template<>
template<>
void __eg_root< void >::__eg_Circle< void >::operator()() const
{
  while( true )
    {
        cinder::gl::color( color() ); 
        cinder::gl::drawSolidCircle( positionActual(), size() );
        eg::sleep();
    }
}

//root::Spiral
template<>
template<>
void __eg_root< void >::__eg_Spiral< void >::operator()() const
{
  while( true )
    {
        const int numCircles = eg::count( root.Get().Shape.Range() );
        
        int index = 0;
        const glm::vec2 center = cinder::app::getWindowCenter();
        for( Shape circle : root.Get().Shape.Range() )
        {
            float rel = ( clock::ct() * speed() / 10 ) + ( index * relative() / (float)numCircles );
            float angle = rel * M_PI * 10;
            glm::vec2 offset( cos( angle ), sin( angle ) );
        
            circle.position( center + ( offset * radius() ) + ( offset * ( curve() * index ) ) );
            circle.f( ( index / (float)numCircles ) );
        
            ++index;
        }
        eg::sleep();
    }
}

//root::Attract
template<>
template<>
void __eg_root< void >::__eg_Attract< void >::operator()() const
{
  const int numCircles = eg::count( root.Get().Shape.Range() );
    if( numCircles > 0 )
    {
        bool bFirst = true;
        glm::vec2 center;
        for( Shape circle : root.Get().Shape.Range() )
        {
            if( bFirst )
            {
                center = circle.positionActual();
                bFirst = false;
            }
            else
                circle.positionActual( center );
        }
    }
}

//root::Randomise
template<>
template<>
void __eg_root< void >::__eg_Randomise< void >::operator()() const
{
  const glm::vec2 center = cinder::app::getWindowCenter();
  if( range() > 0 )
    {
        for( Shape s : root.Get().Shape.Range() )
        {
            glm::vec2 vRand( center.x + center.x * float( ( rand() % range() ) - ( range() / 2 ) ) / float( range() ),
                        center.y + center.y * float( ( rand() % range() ) - ( range() / 2 ) ) / float( range() ) );
            s.positionActual( vRand );
        }
    }
}

//root::imgui
template<>
template<>
void __eg_root< void >::__eg_imgui< void >::operator()() const
{
  while( true )
    {
        ImGui::Begin( "Settings" );
        ImGui::TextColored( ImVec4(1,1,1,1), "Tweakatron 9000" );
        ImGui::SliderInt( "range", &Randomise.range.Get(), 1, 2000 );
        ImGui::SliderFloat( "speed", &Spiral.speed.Get(), 0.0f, 2.0f );
        ImGui::SliderFloat( "curve", &Spiral.curve.Get(), 0.0f, 10.0f );
        ImGui::SliderFloat( "radius", &Spiral.radius.Get(), -100.0f, 100.0f );
        ImGui::SliderFloat( "relative", &Spiral.relative.Get(), -10.0f, 10.0f );
        ImGui::SliderFloat( "morph speed", &morphSpeed.Get(), 0.0f, 1.0f );
    
        if( ImGui::Button( "Spiral" ) )
        {
            if( eg::count( root.Get().Spiral.Range() ) )
                Spiral.Stop();
            else
                Spiral.Start();
        }
        else if( ImGui::Button( "Rand" ) )
        {
            Randomise();
        }
        else if( ImGui::Button( "Attract" ) )
        {
            Attract();
        }
        else if( ImGui::Button( "Quit" ) )
        {
            root.Stop();
        }
        ImGui::End();
        eg::sleep();
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
