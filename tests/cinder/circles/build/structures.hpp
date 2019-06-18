#ifndef EG_INTERFACE_GUARD_STRUCTURES
#define EG_INTERFACE_GUARD_STRUCTURES
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//data structures

//Buffer: b_root type: 90 size: 1
struct b_root
{
    eg::TimeStamp g_root_cycle;
    eg::ActionState g_root_state;
    boost::fibers::fiber g_root_fiber;
    __eg_root< void > g_root_reference;
    float m_morphSpeed;
    std::atomic< std::uint64_t > g_rootStrokedCircle_alloc_iter;
    std::atomic< std::uint64_t > g_rootCircle_alloc_iter;
    std::atomic< std::uint64_t > g_rootSpiral_alloc_iter;
    std::atomic< std::uint64_t > g_rootAttract_alloc_iter;
    std::atomic< std::uint64_t > g_rootRandomise_alloc_iter;
    std::atomic< std::uint64_t > g_rootimgui_alloc_iter;
};
extern b_root *g_root;

//Buffer: b_root_StrokedCircle type: 96 size: 256
struct b_root_StrokedCircle
{
    eg::TimeStamp g_root_StrokedCircle_cycle;
    eg::ActionState g_root_StrokedCircle_state;
    boost::fibers::fiber g_root_StrokedCircle_fiber;
    __eg_root< void >::__eg_StrokedCircle< void > g_root_StrokedCircle_reference;
    struct glm::tvec2<float, glm::highp> m_position;
    struct glm::tvec2<float, glm::highp> m_positionActual;
    class cinder::ColorT<float> m_color;
    float m_size;
    eg::Instance g_root_StrokedCircle_alloc_data;
    std::atomic< std::uint64_t > g_root_StrokedCircleDefault_alloc_iter;
    std::atomic< std::uint64_t > g_root_StrokedCircleColorChanger_alloc_iter;
    std::atomic< std::uint64_t > g_root_StrokedCircleMorph_alloc_iter;
};
extern b_root_StrokedCircle *g_root_StrokedCircle;

//Buffer: b_root_StrokedCircle_Default type: 130 size: 256
struct b_root_StrokedCircle_Default
{
    eg::TimeStamp g_root_StrokedCircle_Default_cycle;
    eg::ActionState g_root_StrokedCircle_Default_state;
    boost::fibers::fiber g_root_StrokedCircle_Default_fiber;
    __eg_root< void >::__eg_Shape< void >::__eg_Default< void > g_root_StrokedCircle_Default_reference;
    eg::Instance g_root_StrokedCircle_Default_alloc_data;
};
extern b_root_StrokedCircle_Default *g_root_StrokedCircle_Default;

//Buffer: b_root_StrokedCircle_ColorChanger type: 135 size: 256
struct b_root_StrokedCircle_ColorChanger
{
    eg::TimeStamp g_root_StrokedCircle_ColorChanger_cycle;
    eg::ActionState g_root_StrokedCircle_ColorChanger_state;
    boost::fibers::fiber g_root_StrokedCircle_ColorChanger_fiber;
    __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > g_root_StrokedCircle_ColorChanger_reference;
    float m_f;
    eg::Instance g_root_StrokedCircle_ColorChanger_alloc_data;
};
extern b_root_StrokedCircle_ColorChanger *g_root_StrokedCircle_ColorChanger;

//Buffer: b_root_StrokedCircle_Morph type: 140 size: 256
struct b_root_StrokedCircle_Morph
{
    eg::TimeStamp g_root_StrokedCircle_Morph_cycle;
    eg::ActionState g_root_StrokedCircle_Morph_state;
    boost::fibers::fiber g_root_StrokedCircle_Morph_fiber;
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > g_root_StrokedCircle_Morph_reference;
    eg::Instance g_root_StrokedCircle_Morph_alloc_data;
};
extern b_root_StrokedCircle_Morph *g_root_StrokedCircle_Morph;

//Buffer: b_root_Circle type: 101 size: 256
struct b_root_Circle
{
    eg::TimeStamp g_root_Circle_cycle;
    eg::ActionState g_root_Circle_state;
    boost::fibers::fiber g_root_Circle_fiber;
    __eg_root< void >::__eg_Circle< void > g_root_Circle_reference;
    struct glm::tvec2<float, glm::highp> m_position;
    struct glm::tvec2<float, glm::highp> m_positionActual;
    class cinder::ColorT<float> m_color;
    float m_size;
    eg::Instance g_root_Circle_alloc_data;
    std::atomic< std::uint64_t > g_root_CircleDefault_alloc_iter;
    std::atomic< std::uint64_t > g_root_CircleColorChanger_alloc_iter;
    std::atomic< std::uint64_t > g_root_CircleMorph_alloc_iter;
};
extern b_root_Circle *g_root_Circle;

//Buffer: b_root_Circle_Default type: 160 size: 256
struct b_root_Circle_Default
{
    eg::TimeStamp g_root_Circle_Default_cycle;
    eg::ActionState g_root_Circle_Default_state;
    boost::fibers::fiber g_root_Circle_Default_fiber;
    __eg_root< void >::__eg_Shape< void >::__eg_Default< void > g_root_Circle_Default_reference;
    eg::Instance g_root_Circle_Default_alloc_data;
};
extern b_root_Circle_Default *g_root_Circle_Default;

//Buffer: b_root_Circle_ColorChanger type: 165 size: 256
struct b_root_Circle_ColorChanger
{
    eg::TimeStamp g_root_Circle_ColorChanger_cycle;
    eg::ActionState g_root_Circle_ColorChanger_state;
    boost::fibers::fiber g_root_Circle_ColorChanger_fiber;
    __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > g_root_Circle_ColorChanger_reference;
    float m_f;
    eg::Instance g_root_Circle_ColorChanger_alloc_data;
};
extern b_root_Circle_ColorChanger *g_root_Circle_ColorChanger;

//Buffer: b_root_Circle_Morph type: 170 size: 256
struct b_root_Circle_Morph
{
    eg::TimeStamp g_root_Circle_Morph_cycle;
    eg::ActionState g_root_Circle_Morph_state;
    boost::fibers::fiber g_root_Circle_Morph_fiber;
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > g_root_Circle_Morph_reference;
    eg::Instance g_root_Circle_Morph_alloc_data;
};
extern b_root_Circle_Morph *g_root_Circle_Morph;

//Buffer: b_root_Spiral type: 106 size: 1
struct b_root_Spiral
{
    eg::TimeStamp g_root_Spiral_cycle;
    eg::ActionState g_root_Spiral_state;
    boost::fibers::fiber g_root_Spiral_fiber;
    __eg_root< void >::__eg_Spiral< void > g_root_Spiral_reference;
    float m_curve;
    float m_radius;
    float m_relative;
    float m_speed;
    eg::Instance g_root_Spiral_alloc_data;
};
extern b_root_Spiral *g_root_Spiral;

//Buffer: b_root_Attract type: 111 size: 1
struct b_root_Attract
{
    eg::TimeStamp g_root_Attract_cycle;
    eg::ActionState g_root_Attract_state;
    boost::fibers::fiber g_root_Attract_fiber;
    __eg_root< void >::__eg_Attract< void > g_root_Attract_reference;
    eg::Instance g_root_Attract_alloc_data;
};
extern b_root_Attract *g_root_Attract;

//Buffer: b_root_Randomise type: 116 size: 1
struct b_root_Randomise
{
    eg::TimeStamp g_root_Randomise_cycle;
    eg::ActionState g_root_Randomise_state;
    boost::fibers::fiber g_root_Randomise_fiber;
    __eg_root< void >::__eg_Randomise< void > g_root_Randomise_reference;
    int m_range;
    eg::Instance g_root_Randomise_alloc_data;
};
extern b_root_Randomise *g_root_Randomise;

//Buffer: b_root_imgui type: 121 size: 1
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
