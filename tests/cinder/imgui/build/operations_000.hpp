#ifndef EG_INTERFACE_GUARD_F38C3D2B_C4D4_4362_AFEF_1558D34195F1__2019_Jun_13_02_27_23
#define EG_INTERFACE_GUARD_F38C3D2B_C4D4_4362_AFEF_1558D34195F1__2019_Jun_13_02_27_23
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//root
template<>
void __eg_root< void >::operator()() const
{
  size( 200 );
  imgui.Start( "EG has IMGUI!" );

while( !Done() )
{
    cinder::gl::clear();
    eg::sleep();
}
}

//root::Square
template<>
template<>
void __eg_root< void >::__eg_Square< void >::operator()() const
{
  const float fStart = clock::ct();
    while( ( fStart + 2.0f ) > clock::ct() )
    {
        cinder::gl::color( cinder::Color( cinder::CM_HSV, 0.5f, 1, 1 ) ); 
        cinder::gl::drawSolidRect( cinder::Rectf{ 0, 0, size(), size() } );
        eg::sleep();
    }
}

//root::imgui
template<>
template<>
void __eg_root< void >::__eg_imgui< void >::operator()(std::string strName) const
{
  bool bContinue = true;
    while( bContinue )
    {
        {
            ImGui::Begin( strName.c_str() );
            ImGui::TextColored( ImVec4(1,1,1,1), "Hello World" );
            ImGui::SliderFloat( "size", &size.Get(), 0.0f, 400.0f );
        
            if( ImGui::Button( "Square" ) )
            {
                Square.Start();
            }
            else if( ImGui::Button( "Other" ) )
            {
  std::ostringstream os;
                os << strName << "_again";
                imgui.Start( os.str() );
            }
            else if( ImGui::Button( "Quit" ) )
            {
                bContinue = false;
            }
            ImGui::End();
        }
        eg::sleep();
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
