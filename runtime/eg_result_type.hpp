
#ifndef EG_RESULT_TYPE
#define EG_RESULT_TYPE

template< typename Context, typename TypePath, typename Operation >
struct __eg_invocation{};

namespace eg
{
    template< typename Context, typename TypePath, typename Operation >
    struct result_type
    {
        using Type = __eg_result_type( __eg_invocation< Context, TypePath, Operation > );
    };
}

#endif //EG_RESULT_TYPE