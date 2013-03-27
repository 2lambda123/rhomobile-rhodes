#pragma once

#include "json/JSONIterator.h"

namespace rho
{
namespace apiGenerator
{

typedef rho::String (*Func_JS)(const String& strObjID, rho::json::CJSONArray& argv, const String& strCallbackID, const String& strJsVmID);

void js_define_method(const char* szMethodPath, Func_JS pFunc );
rho::String js_entry_point(const char* szJSON);
void rho_http_js_entry_point(void *arg, rho::String const &query );

}
}
