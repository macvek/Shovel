#pragma once

#define NAMEENUM_STR(S) #S
#define NAMEENUM(varToCheck, className, enumName) if ( varToCheck == className::enumName ) return NAMEENUM_STR(enumName)