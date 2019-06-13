#ifndef EG_INTERFACE_GUARD_255CE9B6_F868_4F35_B6CB_027DF9E9D402__2019_Jun_13_02_27_24
#define EG_INTERFACE_GUARD_255CE9B6_F868_4F35_B6CB_027DF9E9D402__2019_Jun_13_02_27_24
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//data structures

//Buffer: b_root type: 28 size: 1
struct b_root
{
    eg::TimeStamp g_root_cycle;
    eg::ActionState g_root_state;
    boost::fibers::fiber g_root_fiber;
    __eg_root< void > g_root_reference;
    float m_size;
    std::atomic< std::uint64_t > g_rootSquare_alloc_iter;
    std::atomic< std::uint64_t > g_rootimgui_alloc_iter;
};
extern b_root *g_root;

//Buffer: b_root_Square type: 34 size: 1
struct b_root_Square
{
    eg::TimeStamp g_root_Square_cycle;
    eg::ActionState g_root_Square_state;
    boost::fibers::fiber g_root_Square_fiber;
    __eg_root< void >::__eg_Square< void > g_root_Square_reference;
    eg::Instance g_root_Square_alloc_data;
};
extern b_root_Square *g_root_Square;

//Buffer: b_root_imgui type: 39 size: 4
struct b_root_imgui
{
    eg::TimeStamp g_root_imgui_cycle;
    eg::ActionState g_root_imgui_state;
    boost::fibers::fiber g_root_imgui_fiber;
    __eg_root< void >::__eg_imgui< void > g_root_imgui_reference;
    eg::Instance g_root_imgui_alloc_data;
};
extern b_root_imgui *g_root_imgui;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
