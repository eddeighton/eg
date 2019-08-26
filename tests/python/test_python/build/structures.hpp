#ifndef EG_INTERFACE_GUARD_STRUCTURES
#define EG_INTERFACE_GUARD_STRUCTURES
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//data structures

//Buffer: b_root type: 16 size: 1
struct b_root
{
    eg::TimeStamp g_root_cycle;
    eg::ActionState g_root_state;
    boost::fibers::fiber g_root_fiber;
    __eg_root< void > g_root_reference;
    eg::Instance g_root_ring_index;
    int m_bContinue;
    std::atomic< std::uint64_t > g_rootTest_ring_iter;
};
extern b_root *g_root;

//Buffer: b_root_Test type: 23 size: 1
struct b_root_Test
{
    eg::TimeStamp g_root_Test_cycle;
    eg::ActionState g_root_Test_state;
    boost::fibers::fiber g_root_Test_fiber;
    __eg_root< void >::__eg_Test< void > g_root_Test_reference;
    eg::Instance g_root_Test_ring_index;
    eg::Instance g_root_Test_ring;
};
extern b_root_Test *g_root_Test;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
