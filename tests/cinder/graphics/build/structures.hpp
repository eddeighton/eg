#ifndef EG_INTERFACE_GUARD_STRUCTURES
#define EG_INTERFACE_GUARD_STRUCTURES
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//data structures

//Buffer: b_root type: 38 size: 1
struct b_root
{
    eg::TimeStamp g_root_cycle;
    eg::ActionState g_root_state;
    boost::fibers::fiber g_root_fiber;
    __eg_root< void > g_root_reference;
    eg::Instance g_root_ring_index;
    struct glm::tvec2<float, glm::highp> m_screen;
    std::atomic< std::uint64_t > g_rootLine_ring_iter;
};
extern b_root *g_root;

//Buffer: b_root_Line type: 45 size: 2048
struct b_root_Line
{
    eg::TimeStamp g_root_Line_cycle;
    eg::ActionState g_root_Line_state;
    boost::fibers::fiber g_root_Line_fiber;
    __eg_root< void >::__eg_Line< void > g_root_Line_reference;
    eg::Instance g_root_Line_ring_index;
    struct glm::tvec2<float, glm::highp> m_start;
    struct glm::tvec2<float, glm::highp> m_end;
    struct glm::tvec3<float, glm::highp> m_colour;
    float m_width;
    eg::Instance g_root_Line_ring;
    std::atomic< std::uint64_t > g_root_LineMove_ring_iter;
};
extern b_root_Line *g_root_Line;

//Buffer: b_root_Line_Move type: 54 size: 2048
struct b_root_Line_Move
{
    eg::TimeStamp g_root_Line_Move_cycle;
    eg::ActionState g_root_Line_Move_state;
    boost::fibers::fiber g_root_Line_Move_fiber;
    __eg_root< void >::__eg_Line< void >::__eg_Move< void > g_root_Line_Move_reference;
    eg::Instance g_root_Line_Move_ring_index;
    struct glm::tvec2<float, glm::highp> m_dir;
    eg::Instance g_root_Line_Move_ring;
};
extern b_root_Line_Move *g_root_Line_Move;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
