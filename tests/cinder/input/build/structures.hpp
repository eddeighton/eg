#ifndef EG_INTERFACE_GUARD_C745C93B_43FC_4C79_B698_03EC71B4CB97__2019_Jun_13_02_09_23
#define EG_INTERFACE_GUARD_C745C93B_43FC_4C79_B698_03EC71B4CB97__2019_Jun_13_02_09_23
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//data structures

//Buffer: b_root type: 75 size: 1
struct b_root
{
    eg::TimeStamp g_root_cycle;
    eg::ActionState g_root_state;
    boost::fibers::fiber g_root_fiber;
    __eg_root< void > g_root_reference;
    class std::optional<struct glm::tvec2<float, glm::highp> > m_lastPos;
    std::atomic< std::uint64_t > g_rootCircle_alloc_iter;
};
extern b_root *g_root;

//Buffer: b_root_Circle type: 81 size: 4096
struct b_root_Circle
{
    eg::TimeStamp g_root_Circle_cycle;
    eg::ActionState g_root_Circle_state;
    boost::fibers::fiber g_root_Circle_fiber;
    __eg_root< void >::__eg_Circle< void > g_root_Circle_reference;
    struct glm::tvec2<float, glm::highp> m_position;
    class cinder::ColorT<float> m_color;
    float m_size;
    float m_starttime;
    float m_lifetime;
    float m_max_size;
    float m_max_lifetime;
    eg::Instance g_root_Circle_alloc_data;
    std::atomic< std::uint64_t > g_root_CircleShrink_alloc_iter;
    std::atomic< std::uint64_t > g_root_CircleGrow_alloc_iter;
    std::atomic< std::uint64_t > g_root_CircleMove_alloc_iter;
    std::atomic< std::uint64_t > g_root_CircleColourChanger_alloc_iter;
};
extern b_root_Circle *g_root_Circle;

//Buffer: b_root_Circle_Shrink type: 89 size: 4096
struct b_root_Circle_Shrink
{
    eg::TimeStamp g_root_Circle_Shrink_cycle;
    eg::ActionState g_root_Circle_Shrink_state;
    boost::fibers::fiber g_root_Circle_Shrink_fiber;
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > g_root_Circle_Shrink_reference;
    float m_start;
    eg::Instance g_root_Circle_Shrink_alloc_data;
};
extern b_root_Circle_Shrink *g_root_Circle_Shrink;

//Buffer: b_root_Circle_Grow type: 94 size: 4096
struct b_root_Circle_Grow
{
    eg::TimeStamp g_root_Circle_Grow_cycle;
    eg::ActionState g_root_Circle_Grow_state;
    boost::fibers::fiber g_root_Circle_Grow_fiber;
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > g_root_Circle_Grow_reference;
    float m_start;
    eg::Instance g_root_Circle_Grow_alloc_data;
};
extern b_root_Circle_Grow *g_root_Circle_Grow;

//Buffer: b_root_Circle_Move type: 99 size: 4096
struct b_root_Circle_Move
{
    eg::TimeStamp g_root_Circle_Move_cycle;
    eg::ActionState g_root_Circle_Move_state;
    boost::fibers::fiber g_root_Circle_Move_fiber;
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void > g_root_Circle_Move_reference;
    struct glm::tvec2<float, glm::highp> m_direction;
    float m_speed;
    eg::Instance g_root_Circle_Move_alloc_data;
};
extern b_root_Circle_Move *g_root_Circle_Move;

//Buffer: b_root_Circle_ColourChanger type: 104 size: 4096
struct b_root_Circle_ColourChanger
{
    eg::TimeStamp g_root_Circle_ColourChanger_cycle;
    eg::ActionState g_root_Circle_ColourChanger_state;
    boost::fibers::fiber g_root_Circle_ColourChanger_fiber;
    __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > g_root_Circle_ColourChanger_reference;
    float m_offset;
    eg::Instance g_root_Circle_ColourChanger_alloc_data;
};
extern b_root_Circle_ColourChanger *g_root_Circle_ColourChanger;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
