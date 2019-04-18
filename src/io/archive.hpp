

#ifndef ARCHIVE_18_04_2019
#define ARCHIVE_18_04_2019


#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/filesystem/path.hpp>

namespace boost { namespace serialization {

template<class Archive>
inline void serialize( Archive& ar, boost::filesystem::path& p, const unsigned int version )
{
     std::string s;
     if( Archive::is_saving::value )
         s = p.string();
     ar & boost::serialization::make_nvp( "string", s );
     if(Archive::is_loading::value)
         p = s;
}

}} 




#endif //ARCHIVE_18_04_2019
