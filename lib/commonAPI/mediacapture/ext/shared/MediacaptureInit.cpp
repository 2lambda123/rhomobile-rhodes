#include "common/RhodesApp.h"


extern "C" void Init_Camera_API();

extern "C" void Init_Mediacapture_extension()
{
    Init_Camera_API();
#ifndef RHO_NO_RUBY_API
    RHODESAPP().getExtManager().requireRubyFile("camera");
#endif
}
