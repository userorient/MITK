#include "mitkStringProperty.h"

//##ModelId=3E3FF04F005F
mitk::StringProperty::StringProperty( const char* string ) 
: m_String( string ) 
{

}

//##ModelId=3E3FF04F00E1
bool mitk::StringProperty::operator==(const BaseProperty& property ) const 
{	
    const Self *other = dynamic_cast<const Self*>(&property);

    if(other==NULL) return false;

    return other->m_String==m_String;
}