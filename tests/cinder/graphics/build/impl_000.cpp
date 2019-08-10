#include "structures.hpp"


//input::Action function forward declarations

__eg_root< void > root_starter( eg::Instance _gid );
void root_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Line< void > root_Line_starter( eg::Instance _gid );
void root_Line_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Line< void >::__eg_Move< void > root_Line_Move_starter( eg::Instance _gid );
void root_Line_Move_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Line< void >::__eg_Up< void > root_Line_Up_starter( eg::Instance _gid );
void root_Line_Up_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Line< void >::__eg_Down< void > root_Line_Down_starter( eg::Instance _gid );
void root_Line_Down_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Line< void >::__eg_Left< void > root_Line_Left_starter( eg::Instance _gid );
void root_Line_Left_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Line< void >::__eg_Right< void > root_Line_Right_starter( eg::Instance _gid );
void root_Line_Right_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > root_Line_Sequencer_starter( eg::Instance _gid );
void root_Line_Sequencer_stopper( eg::Instance _gid );



//invocation implementations
template< typename ResultType, typename ContextType, typename TypePathType, typename OperationType >
struct __invoke_impl{
    template< typename... Args >
    ResultType operator()( ContextType, Args... )
    {
        static_assert( 0 && typeid( ResultType ).name() && typeid( ContextType ).name() && typeid( TypePathType ).name(), "Critical error: Invocation system failed to match implementation" );
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_width< void >::Read,
    __eg_root< void >::__eg_Line< void >,
    __eg_type_path< width >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_width< void >::Read operator()( __eg_root< void >::__eg_Line< void > context )
    {
        return g_root_Line[ context.data.instance ].m_width;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_colour< void >::Read,
    __eg_root< void >::__eg_Line< void >,
    __eg_type_path< colour >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_colour< void >::Read operator()( __eg_root< void >::__eg_Line< void > context )
    {
        return g_root_Line[ context.data.instance ].m_colour;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read,
    __eg_root< void >::__eg_Line< void >,
    __eg_type_path< start >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read operator()( __eg_root< void >::__eg_Line< void > context )
    {
        return g_root_Line[ context.data.instance ].m_start;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read,
    __eg_root< void >::__eg_Line< void >,
    __eg_type_path< end >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read operator()( __eg_root< void >::__eg_Line< void > context )
    {
        return g_root_Line[ context.data.instance ].m_end;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Move< void >,
    __eg_type_path< start >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Move< void > context )
    {
        return g_root_Line[ context.data.instance ].m_start;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Move< void >::__eg_dir< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Move< void >,
    __eg_type_path< dir >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Move< void >::__eg_dir< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Move< void > context )
    {
        return g_root_Line_Move[ context.data.instance ].m_dir;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Move< void >,
    __eg_type_path< start >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Move< void > context, __eg_root< void >::__eg_Line< void >::__eg_start< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_start = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Move< void >,
    __eg_type_path< end >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Move< void > context )
    {
        return g_root_Line[ context.data.instance ].m_end;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Move< void >,
    __eg_type_path< end >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Move< void > context, __eg_root< void >::__eg_Line< void >::__eg_end< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_end = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_screen< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Move< void >,
    __eg_type_path< screen >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_screen< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Move< void > context )
    {
        return g_root[ ( context.data.instance ) / 2048 ].m_screen;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Move< void >,
    __eg_type_path< dir >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Move< void > context, __eg_root< void >::__eg_Line< void >::__eg_Move< void >::__eg_dir< void >::Write value )
    {
        g_root_Line_Move[ context.data.instance ].m_dir = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Up< void >,
    __eg_type_path< start >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Up< void > context )
    {
        return g_root_Line[ context.data.instance ].m_start;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Distance< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Up< void >,
    __eg_type_path< Distance >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Distance< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Up< void > context )
    {
        return g_root_Line[ context.data.instance ].m_Distance;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Speed< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Up< void >,
    __eg_type_path< Speed >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Speed< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Up< void > context )
    {
        return g_root_Line[ context.data.instance ].m_Speed;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Up< void >,
    __eg_type_path< start >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Up< void > context, __eg_root< void >::__eg_Line< void >::__eg_start< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_start = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Up< void >,
    __eg_type_path< end >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Up< void > context )
    {
        return g_root_Line[ context.data.instance ].m_end;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Up< void >,
    __eg_type_path< end >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Up< void > context, __eg_root< void >::__eg_Line< void >::__eg_end< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_end = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Down< void >,
    __eg_type_path< start >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Down< void > context )
    {
        return g_root_Line[ context.data.instance ].m_start;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Distance< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Down< void >,
    __eg_type_path< Distance >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Distance< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Down< void > context )
    {
        return g_root_Line[ context.data.instance ].m_Distance;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Speed< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Down< void >,
    __eg_type_path< Speed >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Speed< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Down< void > context )
    {
        return g_root_Line[ context.data.instance ].m_Speed;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Down< void >,
    __eg_type_path< start >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Down< void > context, __eg_root< void >::__eg_Line< void >::__eg_start< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_start = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Down< void >,
    __eg_type_path< end >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Down< void > context )
    {
        return g_root_Line[ context.data.instance ].m_end;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Down< void >,
    __eg_type_path< end >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Down< void > context, __eg_root< void >::__eg_Line< void >::__eg_end< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_end = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Left< void >,
    __eg_type_path< start >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Left< void > context )
    {
        return g_root_Line[ context.data.instance ].m_start;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Distance< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Left< void >,
    __eg_type_path< Distance >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Distance< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Left< void > context )
    {
        return g_root_Line[ context.data.instance ].m_Distance;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Speed< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Left< void >,
    __eg_type_path< Speed >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Speed< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Left< void > context )
    {
        return g_root_Line[ context.data.instance ].m_Speed;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Left< void >,
    __eg_type_path< start >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Left< void > context, __eg_root< void >::__eg_Line< void >::__eg_start< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_start = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Left< void >,
    __eg_type_path< end >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Left< void > context )
    {
        return g_root_Line[ context.data.instance ].m_end;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Left< void >,
    __eg_type_path< end >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Left< void > context, __eg_root< void >::__eg_Line< void >::__eg_end< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_end = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Right< void >,
    __eg_type_path< start >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_start< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Right< void > context )
    {
        return g_root_Line[ context.data.instance ].m_start;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Distance< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Right< void >,
    __eg_type_path< Distance >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Distance< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Right< void > context )
    {
        return g_root_Line[ context.data.instance ].m_Distance;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Speed< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Right< void >,
    __eg_type_path< Speed >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Speed< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Right< void > context )
    {
        return g_root_Line[ context.data.instance ].m_Speed;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Right< void >,
    __eg_type_path< start >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Right< void > context, __eg_root< void >::__eg_Line< void >::__eg_start< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_start = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Right< void >,
    __eg_type_path< end >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_end< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Right< void > context )
    {
        return g_root_Line[ context.data.instance ].m_end;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Line< void >::__eg_Right< void >,
    __eg_type_path< end >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Line< void >::__eg_Right< void > context, __eg_root< void >::__eg_Line< void >::__eg_end< void >::Write value )
    {
        g_root_Line[ context.data.instance ].m_end = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Sequence< void >::Read,
    __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >,
    __eg_type_path< Sequence >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Line< void >::__eg_Sequence< void >::Read operator()( __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > context )
    {
        return g_root_Line[ context.data.instance ].m_Sequence;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Up< void >,
    __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >,
    __eg_type_path< __eg_root< void >::__eg_Line< void >::__eg_Up< void > >,
    __eg_ImpNoParams
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Line< void >::__eg_Up< void > operator()( __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > context, Args... args )
    {
        __eg_root< void >::__eg_Line< void >::__eg_Up< void > ref = root_Line_Up_starter( context.data.instance );
        if( ref )
        {
            ref( args... );
            root_Line_Up_stopper( ref.data.instance );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Right< void >,
    __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >,
    __eg_type_path< __eg_root< void >::__eg_Line< void >::__eg_Right< void > >,
    __eg_ImpNoParams
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Line< void >::__eg_Right< void > operator()( __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > context, Args... args )
    {
        __eg_root< void >::__eg_Line< void >::__eg_Right< void > ref = root_Line_Right_starter( context.data.instance );
        if( ref )
        {
            ref( args... );
            root_Line_Right_stopper( ref.data.instance );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Down< void >,
    __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >,
    __eg_type_path< __eg_root< void >::__eg_Line< void >::__eg_Down< void > >,
    __eg_ImpNoParams
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Line< void >::__eg_Down< void > operator()( __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > context, Args... args )
    {
        __eg_root< void >::__eg_Line< void >::__eg_Down< void > ref = root_Line_Down_starter( context.data.instance );
        if( ref )
        {
            ref( args... );
            root_Line_Down_stopper( ref.data.instance );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Line< void >::__eg_Left< void >,
    __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >,
    __eg_type_path< __eg_root< void >::__eg_Line< void >::__eg_Left< void > >,
    __eg_ImpNoParams
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Line< void >::__eg_Left< void > operator()( __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > context, Args... args )
    {
        __eg_root< void >::__eg_Line< void >::__eg_Left< void > ref = root_Line_Left_starter( context.data.instance );
        if( ref )
        {
            ref( args... );
            root_Line_Left_stopper( ref.data.instance );
        }
        return ref;
    }
};

//generic variant invocation adaptor
template< typename... Ts >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< __eg_variant< Ts... >, TypePath, Operation >::Type
__eg_variant< Ts... >::invoke( Args... args )
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_variant< Ts... >, TypePath, Operation >::Type, __eg_variant< Ts... >, CanonicalTypePathType, Operation >()( *this, args... );
}


//root
template<>
struct eg::is_convertible< __eg_root< void >, __eg_root< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_root( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 67: //root
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename TFrom >
__eg_root< __eg1 >& __eg_root< __eg1 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 67: //root
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root[ instance ].g_root_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root[ instance ].g_root_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root[ instance ].g_root_state;
}


//root::Line
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Line< void >, __eg_root< void >::__eg_Line< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Line( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 74: //root::Line
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
typename __eg_root< __eg1 >::template __eg_Line< __eg2 >& __eg_root< __eg1 >::__eg_Line< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 74: //root::Line
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Line< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line[ instance ].g_root_Line_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Line< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line[ instance ].g_root_Line_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Line< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line[ instance ].g_root_Line_state;
}


//root::Line::Move
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Line< void >::__eg_Move< void >, __eg_root< void >::__eg_Line< void >::__eg_Move< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Move< __eg3 >::__eg_Move( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Move< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 83: //root::Line::Move
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Move< __eg3 >& __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Move< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Move< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 83: //root::Line::Move
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Line< void >::__eg_Move< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Move[ instance ].g_root_Line_Move_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Line< void >::__eg_Move< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Move[ instance ].g_root_Line_Move_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Line< void >::__eg_Move< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Move[ instance ].g_root_Line_Move_state;
}


//root::Line::Up
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Line< void >::__eg_Up< void >, __eg_root< void >::__eg_Line< void >::__eg_Up< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Up< __eg3 >::__eg_Up( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Up< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 89: //root::Line::Up
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Up< __eg3 >& __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Up< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Up< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 89: //root::Line::Up
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Line< void >::__eg_Up< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Up[ instance ].g_root_Line_Up_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Line< void >::__eg_Up< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Up[ instance ].g_root_Line_Up_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Line< void >::__eg_Up< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Up[ instance ].g_root_Line_Up_state;
}


//root::Line::Down
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Line< void >::__eg_Down< void >, __eg_root< void >::__eg_Line< void >::__eg_Down< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Down< __eg3 >::__eg_Down( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Down< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 95: //root::Line::Down
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Down< __eg3 >& __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Down< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Down< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 95: //root::Line::Down
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Line< void >::__eg_Down< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Down[ instance ].g_root_Line_Down_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Line< void >::__eg_Down< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Down[ instance ].g_root_Line_Down_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Line< void >::__eg_Down< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Down[ instance ].g_root_Line_Down_state;
}


//root::Line::Left
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Line< void >::__eg_Left< void >, __eg_root< void >::__eg_Line< void >::__eg_Left< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Left< __eg3 >::__eg_Left( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Left< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 101: //root::Line::Left
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Left< __eg3 >& __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Left< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Left< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 101: //root::Line::Left
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Line< void >::__eg_Left< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Left[ instance ].g_root_Line_Left_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Line< void >::__eg_Left< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Left[ instance ].g_root_Line_Left_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Line< void >::__eg_Left< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Left[ instance ].g_root_Line_Left_state;
}


//root::Line::Right
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Line< void >::__eg_Right< void >, __eg_root< void >::__eg_Line< void >::__eg_Right< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Right< __eg3 >::__eg_Right( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Right< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 107: //root::Line::Right
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Right< __eg3 >& __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Right< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Right< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 107: //root::Line::Right
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Line< void >::__eg_Right< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Right[ instance ].g_root_Line_Right_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Line< void >::__eg_Right< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Right[ instance ].g_root_Line_Right_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Line< void >::__eg_Right< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Right[ instance ].g_root_Line_Right_state;
}


//root::Line::Sequencer
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >, __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Sequencer< __eg3 >::__eg_Sequencer( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 113: //root::Line::Sequencer
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Sequencer< __eg3 >& __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Sequencer< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 113: //root::Line::Sequencer
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_state;
}


//root
template< typename __eg1 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< __eg_root< __eg1 >, TypePath, Operation >::Type
__eg_root< __eg1 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >, TypePath, Operation >::Type, __eg_root< __eg1 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Line
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Line< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Line< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Line< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Line< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Line::Move
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Move< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Move< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Move< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Move< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Line::Up
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Up< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Up< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Up< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Up< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Line::Down
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Down< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Down< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Down< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Down< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Line::Left
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Left< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Left< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Left< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Left< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Line::Right
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Right< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Right< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Right< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Right< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Line::Sequencer
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Line< __eg2 >::template __eg_Sequencer< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Sequencer< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Sequencer< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Line< __eg2 >::__eg_Sequencer< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}


