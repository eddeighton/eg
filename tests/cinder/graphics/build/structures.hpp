#ifndef EG_INTERFACE_GUARD_STRUCTURES
#define EG_INTERFACE_GUARD_STRUCTURES
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//data structures

//Buffer: b_root type: 67 size: 1
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

//Buffer: b_root_Line type: 74 size: 2048
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
    float m_Distance;
    float m_Speed;
    class std::vector<int, class std::allocator<int> > m_Sequence;
    eg::Instance g_root_Line_ring;
    std::atomic< std::uint64_t > g_root_LineMove_ring_iter;
    std::atomic< std::uint64_t > g_root_LineUp_ring_iter;
    std::atomic< std::uint64_t > g_root_LineDown_ring_iter;
    std::atomic< std::uint64_t > g_root_LineLeft_ring_iter;
    std::atomic< std::uint64_t > g_root_LineRight_ring_iter;
    std::atomic< std::uint64_t > g_root_LineSequencer_ring_iter;
};
extern b_root_Line *g_root_Line;

//Buffer: b_root_Line_Move type: 83 size: 2048
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

//Buffer: b_root_Line_Up type: 89 size: 2048
struct b_root_Line_Up
{
    eg::TimeStamp g_root_Line_Up_cycle;
    eg::ActionState g_root_Line_Up_state;
    boost::fibers::fiber g_root_Line_Up_fiber;
    __eg_root< void >::__eg_Line< void >::__eg_Up< void > g_root_Line_Up_reference;
    eg::Instance g_root_Line_Up_ring_index;
    eg::Instance g_root_Line_Up_ring;
};
extern b_root_Line_Up *g_root_Line_Up;

//Buffer: b_root_Line_Down type: 95 size: 2048
struct b_root_Line_Down
{
    eg::TimeStamp g_root_Line_Down_cycle;
    eg::ActionState g_root_Line_Down_state;
    boost::fibers::fiber g_root_Line_Down_fiber;
    __eg_root< void >::__eg_Line< void >::__eg_Down< void > g_root_Line_Down_reference;
    eg::Instance g_root_Line_Down_ring_index;
    eg::Instance g_root_Line_Down_ring;
};
extern b_root_Line_Down *g_root_Line_Down;

//Buffer: b_root_Line_Left type: 101 size: 2048
struct b_root_Line_Left
{
    eg::TimeStamp g_root_Line_Left_cycle;
    eg::ActionState g_root_Line_Left_state;
    boost::fibers::fiber g_root_Line_Left_fiber;
    __eg_root< void >::__eg_Line< void >::__eg_Left< void > g_root_Line_Left_reference;
    eg::Instance g_root_Line_Left_ring_index;
    eg::Instance g_root_Line_Left_ring;
};
extern b_root_Line_Left *g_root_Line_Left;

//Buffer: b_root_Line_Right type: 107 size: 2048
struct b_root_Line_Right
{
    eg::TimeStamp g_root_Line_Right_cycle;
    eg::ActionState g_root_Line_Right_state;
    boost::fibers::fiber g_root_Line_Right_fiber;
    __eg_root< void >::__eg_Line< void >::__eg_Right< void > g_root_Line_Right_reference;
    eg::Instance g_root_Line_Right_ring_index;
    eg::Instance g_root_Line_Right_ring;
};
extern b_root_Line_Right *g_root_Line_Right;

//Buffer: b_root_Line_Sequencer type: 113 size: 2048
struct b_root_Line_Sequencer
{
    eg::TimeStamp g_root_Line_Sequencer_cycle;
    eg::ActionState g_root_Line_Sequencer_state;
    boost::fibers::fiber g_root_Line_Sequencer_fiber;
    __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > g_root_Line_Sequencer_reference;
    eg::Instance g_root_Line_Sequencer_ring_index;
    eg::Instance g_root_Line_Sequencer_ring;
};
extern b_root_Line_Sequencer *g_root_Line_Sequencer;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
