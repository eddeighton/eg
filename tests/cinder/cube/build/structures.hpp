#ifndef EG_INTERFACE_GUARD_F53C354F_0B3E_4B59_819E_D97383DBCD26__2019_Jun_13_02_20_13
#define EG_INTERFACE_GUARD_F53C354F_0B3E_4B59_819E_D97383DBCD26__2019_Jun_13_02_20_13
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//data structures

//Buffer: b_root type: 56 size: 1
struct b_root
{
    eg::TimeStamp g_root_cycle;
    eg::ActionState g_root_state;
    boost::fibers::fiber g_root_fiber;
    __eg_root< void > g_root_reference;
    int m_x;
    int m_y;
    std::atomic< std::uint64_t > g_rootCamera_alloc_iter;
    std::atomic< std::uint64_t > g_rootCube_alloc_iter;
    std::atomic< std::uint64_t > g_rootCreateCube_alloc_iter;
};
extern b_root *g_root;

//Buffer: b_root_Camera type: 62 size: 1
struct b_root_Camera
{
    eg::TimeStamp g_root_Camera_cycle;
    eg::ActionState g_root_Camera_state;
    boost::fibers::fiber g_root_Camera_fiber;
    __eg_root< void >::__eg_Camera< void > g_root_Camera_reference;
    class cinder::CameraPersp m_perspective;
    struct glm::tvec3<float, glm::highp> m_eye;
    eg::Instance g_root_Camera_alloc_data;
    std::atomic< std::uint64_t > g_root_CameraOrbit_alloc_iter;
};
extern b_root_Camera *g_root_Camera;

//Buffer: b_root_Camera_Orbit type: 82 size: 1
struct b_root_Camera_Orbit
{
    eg::TimeStamp g_root_Camera_Orbit_cycle;
    eg::ActionState g_root_Camera_Orbit_state;
    boost::fibers::fiber g_root_Camera_Orbit_fiber;
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > g_root_Camera_Orbit_reference;
    float m_rate;
    float m_radius;
    eg::Instance g_root_Camera_Orbit_alloc_data;
};
extern b_root_Camera_Orbit *g_root_Camera_Orbit;

//Buffer: b_root_Cube type: 67 size: 9
struct b_root_Cube
{
    eg::TimeStamp g_root_Cube_cycle;
    eg::ActionState g_root_Cube_state;
    boost::fibers::fiber g_root_Cube_fiber;
    __eg_root< void >::__eg_Cube< void > g_root_Cube_reference;
    struct glm::tvec3<float, glm::highp> m_position;
    struct glm::tvec3<float, glm::highp> m_size;
    eg::Instance g_root_Cube_alloc_data;
};
extern b_root_Cube *g_root_Cube;

//Buffer: b_root_CreateCube type: 72 size: 1
struct b_root_CreateCube
{
    eg::TimeStamp g_root_CreateCube_cycle;
    eg::ActionState g_root_CreateCube_state;
    boost::fibers::fiber g_root_CreateCube_fiber;
    __eg_root< void >::__eg_CreateCube< void > g_root_CreateCube_reference;
    eg::Instance g_root_CreateCube_alloc_data;
};
extern b_root_CreateCube *g_root_CreateCube;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
