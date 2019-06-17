#ifndef EG_INTERFACE_GUARD_OPERATIONS
#define EG_INTERFACE_GUARD_OPERATIONS
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//root
template<>
void __eg_root< void >::operator()() const
{
  std::vector< Project > projects;
projects.push_back( Project{ "test" } );

bool bContinue = true;
bool bShowBrowser = false;
bool bShowCreate = false;

while( bContinue )
{
    //need to sleep atleast once before we start using imgui
    eg::sleep();
    
    if( ImGui::BeginMainMenuBar() )
    {
        if( ImGui::BeginMenu( "Wizard" ) )
        {
            if( ImGui::MenuItem( "New" ) )
            {
                bShowCreate = true;
            }
            if( ImGui::MenuItem( "Open" ) )
            {
                bShowBrowser = true;
            }
            if( ImGui::MenuItem( "Save" ) )
            {
            }
            if( ImGui::MenuItem( "Quit" ) )
            {
                bContinue = false;
            }
            
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if( bShowBrowser )
    {
        ImGui::Begin( "Project Browser", &bShowBrowser );
        
        if( ImGui::TreeNode( "Projects" ) )
        {
            for( int i = 0; i < 100; ++i )
            {
                std::ostringstream os;
                os << "Project " << i ;
                
                if( ImGui::TreeNode( "Project" ) )
                {
                    ImGui::Text( "Name" );
                    if( ImGui::Button( "Run" ) )
                    {
                        
                    }
                    
                    ImGui::TreePop();
                }
                
            }
            
            ImGui::TreePop();
        }
        
        ImGui::End();
    }

    if( bShowCreate )
    {
        ImGui::Begin( "Project Wizard", &bShowCreate );
        
        ImGui::End();
    }


    
    
    cinder::gl::clear();
}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
