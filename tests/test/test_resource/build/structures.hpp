#ifndef EG_INTERFACE_GUARD_STRUCTURES
#define EG_INTERFACE_GUARD_STRUCTURES
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//data structures

//Buffer: b_root type: 19 size: 1
struct b_root
{
    eg::TimeStamp g_root_cycle;
    eg::ActionState g_root_state;
    boost::fibers::fiber g_root_fiber;
    __eg_root< void > g_root_reference;
    eg::Instance g_root_ring_index;
    std::atomic< std::uint64_t > g_rootPrintRes_ring_iter;
};
extern b_root *g_root;

//Buffer: b_root_PrintRes type: 26 size: 1
struct b_root_PrintRes
{
    eg::TimeStamp g_root_PrintRes_cycle;
    eg::ActionState g_root_PrintRes_state;
    boost::fibers::fiber g_root_PrintRes_fiber;
    __eg_root< void >::__eg_PrintRes< void > g_root_PrintRes_reference;
    eg::Instance g_root_PrintRes_ring_index;
    eg::Instance g_root_PrintRes_ring;
};
extern b_root_PrintRes *g_root_PrintRes;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
