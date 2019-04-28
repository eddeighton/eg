
#ifndef EG_OPERATION_TYPES
#define EG_OPERATION_TYPES

struct [[clang::eg_type( eg::id_Imp_NoParams    )]] __eg_ImpNoParams;   //id_Imp_NoParams   
struct [[clang::eg_type( eg::id_Imp_Params      )]] __eg_ImpParams;     //id_Imp_Params   
struct [[clang::eg_type( eg::id_Get             )]] Get;                //id_Get          
struct [[clang::eg_type( eg::id_Update          )]] Update;             //id_Update       
struct [[clang::eg_type( eg::id_Old             )]] Old;                //id_Old          
struct [[clang::eg_type( eg::id_Stop            )]] Stop;               //id_Stop         
struct [[clang::eg_type( eg::id_Pause           )]] Pause;              //id_Pause        
struct [[clang::eg_type( eg::id_Resume          )]] Resume;             //id_Resume       
struct [[clang::eg_type( eg::id_Defer           )]] Defer;              //id_Defer        
struct [[clang::eg_type( eg::id_Size            )]] Size;               //id_Size        
struct [[clang::eg_type( eg::id_Range           )]] Range;              //id_Range        

#endif //EG_OPERATION_TYPES