<% moduleNamespace = api_generator_cpp_MakeNamespace($cur_module.parents) %>

#include "<%= $cur_module.name %>Base.h"
#include "api_generator/js_helpers.h"

#include "logging/RhoLog.h"
#undef DEFAULT_LOGCATEGORY
#define DEFAULT_LOGCATEGORY "<%= $cur_module.name %>"

#include "common/StringConverter.h"

extern "C" void rho_wm_impl_performOnUiThread(rho::common::IRhoRunnable* pTask);

using namespace rho;
using namespace rho::json;
using namespace rho::common;
using namespace rho::apiGenerator;

<% $cur_module.methods.each do |module_method| 
   if module_method.generateNativeAPI %>
<%= api_generator_MakeJSMethodDecl($cur_module.name, module_method.native_name, module_method.access == ModuleMethod::ACCESS_STATIC)%>
{
    RAWTRACE3("<%=module_method.native_name%>(strObjID = %s, strCallbackID = %s, strJsVmID = %s)", strObjID.c_str(), strCallbackID.c_str(), strJsVmID.c_str());

    rho::apiGenerator::CMethodResult oRes;

<% if module_method.result != nil
    if module_method.result.sub_param && module_method.result.sub_param.name %>
    oRes.setParamName( "<%= module_method.result.sub_param.name %>" );<%
    end

    result_type = nil
    if MethodParam::BASE_TYPES.include?(module_method.result.type)
      result_type = module_method.result.item_type
    else
      result_type = module_method.result.type;
    end

if api_generator_isSelfModule( $cur_module, result_type) %>
    oRes.setJSObjectClassPath( "<%= api_generator_getJSModuleName(api_generator_getRubyModuleFullName($cur_module))%>" );<%
elsif result_type && result_type.length()>0 && !MethodParam::BASE_TYPES.include?(result_type) %>
    oRes.setJSObjectClassPath( "<%= api_generator_getJSModuleName(result_type) %>" );<%
end; end

if module_method.linked_property && module_method.special_behaviour == ModuleMethod::SPECIAL_BEHAVIOUR_GETTER %>
    oRes.setRequestedType(<%= api_generator_cpp_makeMethodResultType(module_method.linked_property.type) %>);<%
elsif module_method.result && module_method.result.type %>
    oRes.setRequestedType(<%= api_generator_cpp_makeMethodResultType(module_method.result.type) %>);<%
end %>

    rho::common::CInstanceClassFunctorBase<rho::apiGenerator::CMethodResult>* pFunctor = 0;
    int argc = argv.getSize();
<% if module_method.access != ModuleMethod::ACCESS_STATIC %>
    <%= moduleNamespace%>I<%= $cur_module.name %>* pObj = <%= moduleNamespace%>C<%= $cur_module.name %>FactoryBase::getInstance()->getModuleByID(strObjID);
<%end%>

<% functor_params = ""; first_arg = 0;
   module_method.params.each do |param| %>
    <% if !param.can_be_nil %>
    if ( argc == <%= first_arg %> )
    {
        oRes.setArgError( "Wrong number of arguments: " + convertToStringA(argc) + " instead of " + convertToStringA(<%= module_method.params.size() %>) );
        return oRes.toJSON();
    }
    <% end %>

<% if param.type == MethodParam::TYPE_STRING %>
    <%= api_generator_cpp_makeNativeType(param.type) %> arg<%= first_arg %> = "<%= param.default_value ? param.default_value : "" %>";
    if ( argc > <%= first_arg %> )
    {
        if ( argv[<%= first_arg %>].isString() )
        {
            arg<%= first_arg %> = argv[<%= first_arg %>].getString();
        }
        else if (!argv[<%= first_arg %>].isNull())
        {
            oRes.setArgError( "Type error: argument " <%= "\"#{first_arg}\"" %> " should be " <%= "\"#{param.type.downcase}\"" %> );
            return oRes.toJSON();
        }
    }
<% end %>

<% if param.type == MethodParam::TYPE_INT %>
    <%= api_generator_cpp_makeNativeType(param.type) %> arg<%= first_arg %> = <%= param.default_value ? param.default_value : 0 %>;
    if ( argc > <%= first_arg %> )
    {
        if ( argv[<%= first_arg %>].isInteger() )
            arg<%= first_arg %> = argv[<%= first_arg %>].getInt();
        else if (!argv[<%= first_arg %>].isNull())
        {
            oRes.setArgError("Type error: argument " <%= "\"#{first_arg}\"" %> " should be " <%= "\"#{param.type.downcase}\"" %> );
            return oRes.toJSON();
        }
    }
<% end %>

<% if param.type == MethodParam::TYPE_BOOL %>
    <%= api_generator_cpp_makeNativeType(param.type) %> arg<%= first_arg %> = <%= param.default_value ? param.default_value : false %>;
    if ( argc > <%= first_arg %> )
    {
        if ( argv[<%= first_arg %>].isBoolean() )
            arg<%= first_arg %> = argv[<%= first_arg %>].getBoolean();
        else if (!argv[<%= first_arg %>].isNull())
        {
            oRes.setArgError("Type error: argument " <%= "\"#{first_arg}\"" %> " should be " <%= "\"#{param.type.downcase}\"" %> );
            return oRes.toJSON();
        }
    }
<% end %>

<% if param.type == MethodParam::TYPE_DOUBLE %>
    <%= api_generator_cpp_makeNativeType(param.type) %> arg<%= first_arg %> = <%= param.default_value ? param.default_value : 0 %>;
    if ( argc > <%= first_arg %> )
    {
        if ( argv[<%= first_arg %>].isFloat() )
            arg<%= first_arg %> = argv[<%= first_arg %>].getDouble();
        else if ( argv[<%= first_arg %>].isInteger() )
            arg<%= first_arg %> = argv[<%= first_arg %>].getInt();
        else if (!argv[<%= first_arg %>].isNull())
        {
            oRes.setArgError("Type error: argument " <%= "\"#{first_arg}\"" %> " should be " <%= "\"#{param.type.downcase}\"" %> );
            return oRes.toJSON();
        }
    }
<% end %>

<% if param.type == MethodParam::TYPE_ARRAY %>
    <%= api_generator_cpp_makeNativeType(param.type) %> arg<%= first_arg %>;
    if ( argc > <%= first_arg %> )
    {
        if ( argv[<%= first_arg %>].isArray() )
        {
            CJSONArray arParam(argv[<%= first_arg %>]);
            for( int i = 0; i < arParam.getSize(); i++ )
            {
                arg<%= first_arg %>.addElement( arParam[i].getString() );
            }
        }
        else if (!argv[<%= first_arg %>].isNull())
        {
            oRes.setArgError("Type error: argument " <%= "\"#{first_arg}\"" %> " should be " <%= "\"#{param.type.downcase}\"" %> );
            return oRes.toJSON();
        }
    }
<% end %>

<% if param.type == MethodParam::TYPE_HASH %>
    <%= api_generator_cpp_makeNativeType(param.type) %> arg<%= first_arg %>;
    if ( argc > <%= first_arg %> )
    {
        if ( argv[<%= first_arg %>].isObject() )
        {
            CJSONStructIterator objIter(argv[<%= first_arg %>]);

            for( ; !objIter.isEnd(); objIter.next() )
            {
                arg<%= first_arg %>[objIter.getCurKey()] = objIter.getCurString();
            }
        }
        else if (!argv[<%= first_arg %>].isNull())
        {
            oRes.setArgError("Type error: argument " <%= "\"#{first_arg}\"" %> " should be " <%= "\"#{param.type.downcase}\"" %> );
            return oRes.toJSON();
        }
    }
<% end %>

<% if !MethodParam::BASE_TYPES.include?(param.type) %>
    <%= api_generator_cpp_makeNativeType(param.type) %> arg<%= first_arg %>;
<% end %>

<% functor_params += "arg#{first_arg}, " %>
<% first_arg = first_arg+1 %>
<% end %>

<% if module_method.has_callback != ModuleMethod::CALLBACK_NONE %>
    oRes.setCallInUIThread(<%= (module_method.run_in_thread == ModuleMethod::RUN_IN_THREAD_UI) ? "true" : "false" %>);
    oRes.setJSCallback( strCallbackID );
    oRes.setCallbackParam( strCallbackParam );
<% end %>

<% numParams = module_method.params.size()+1
if module_method.access != ModuleMethod::ACCESS_STATIC
    nativeInterfaceFunc = "#{moduleNamespace}I#{$cur_module.name}::#{module_method.native_name}"
    functorWar = "pFunctor = rho_makeInstanceClassFunctor#{numParams}( pObj, &#{nativeInterfaceFunc}, #{functor_params} oRes );"
else 
    nativeSingleton = "#{moduleNamespace}C#{$cur_module.name}FactoryBase::get#{$cur_module.name}SingletonS()"
    nativeSingletonMethod = "#{moduleNamespace}I#{$cur_module.name}Singleton::#{module_method.native_name}"
    functorWar = "pFunctor = rho_makeInstanceClassFunctor#{numParams}( #{nativeSingleton}, &#{nativeSingletonMethod}, #{functor_params} oRes );"
end

if module_method.run_in_thread == ModuleMethod::RUN_IN_THREAD_UI %>
    <%= functorWar %>
    rho_wm_impl_performOnUiThread( pFunctor );<%
elsif (module_method.run_in_thread == ModuleMethod::RUN_IN_THREAD_MODULE) || (module_method.run_in_thread == ModuleMethod::RUN_IN_THREAD_SEPARATED) %>
    <%= functorWar %>
    <%= moduleNamespace%>C<%= $cur_module.name %>FactoryBase::get<%= $cur_module.name %>SingletonS()->addCommandToQueue( pFunctor );<%
else if module_method.run_in_thread != ModuleMethod::RUN_IN_THREAD_NONE %>
    if ( oRes.hasCallback() )
    {
        <%= functorWar %>
        <%= moduleNamespace%>C<%= $cur_module.name %>FactoryBase::get<%= $cur_module.name %>SingletonS()->addCommandToQueue( pFunctor );
    }
    else <%
end %>
    {
<% if module_method.access != ModuleMethod::ACCESS_STATIC %>
        pObj-><%= module_method.native_name %>( <%= functor_params %> oRes );<%
else %>
        <%= moduleNamespace%>C<%= $cur_module.name %>FactoryBase::get<%= $cur_module.name %>SingletonS()-><%= module_method.native_name %>( <%= functor_params %> oRes );<%
end %>
    }<% end %>

    return oRes.toJSON();

}
<% end 
   end %>

<% if $cur_module.is_template_default_instance %>
<%= api_generator_MakeJSMethodDecl($cur_module.name, "getDefaultID", true)%>
{
    rho::apiGenerator::CMethodResult oRes;
    rho::String strDefaultID = <%= moduleNamespace%>C<%= $cur_module.name %>FactoryBase::get<%= $cur_module.name %>SingletonS()->getDefaultID();
    oRes.set(strDefaultID);

    return oRes.toJSON();
}

<%= api_generator_MakeJSMethodDecl($cur_module.name, "getDefault", true)%>
{
    rho::apiGenerator::CMethodResult oRes;
    rho::String strDefaultID = <%= moduleNamespace%>C<%= $cur_module.name %>FactoryBase::get<%= $cur_module.name %>SingletonS()->getDefaultID();
    oRes.set(strDefaultID);
    oRes.setJSObjectClassPath("<%= api_generator_getJSModuleName(api_generator_getRubyModuleFullName($cur_module))%>");

    return oRes.toJSON();
}

<%= api_generator_MakeJSMethodDecl($cur_module.name, "setDefaultID", true)%>
{
    rho::apiGenerator::CMethodResult oRes;
    <%= moduleNamespace%>C<%= $cur_module.name %>FactoryBase::get<%= $cur_module.name %>SingletonS()->setDefaultID(strObjID);

    return oRes.toJSON();
}

<% end %>
