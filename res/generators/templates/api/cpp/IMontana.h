#pragma once

#include "common/RhoStd.h"
#include "api_generator/MethodResult.h"
#include "api_generator/BaseClasses.h"

<% $cur_module.parents.each do |parent| %>
namespace <%= parent.downcase() %> {<%
end %>
///////////////////////////////////////////////////////////
struct I<%= $cur_module.name %>
{
//constants
<% if $cur_module.properties_access == ModuleMethod::ACCESS_INSTANCE
   $cur_module.constants.each do |module_constant|
    if module_constant.type == MethodParam::TYPE_STRING %>
    static const char <%= module_constant.name %>[];// "<%= module_constant.value %>" <%
else %>
    static const <%= api_generator_cpp_makeNativeType(module_constant.type) %> <%= module_constant.name %> = <%= module_constant.value %>; <%
end; end; end %>

//methods
    virtual ~I<%= $cur_module.name %>(){}

<% $cur_module.methods.each do |module_method|
    next if module_method.access == ModuleMethod::ACCESS_STATIC

    params = ''
    module_method.params.each do |param|
        params += " #{api_generator_cpp_makeNativeTypeArg(param.type)} #{param.name}, "
    end

    params += 'rho::apiGenerator::CMethodResult& oResult'
    module_method.cached_data["cpp_params"] = params
%>    virtual void <%= module_method.native_name%>(<%= params%>) = 0;
<% end %>
};

struct I<%= $cur_module.name %>Singleton
{
//constants
<% if $cur_module.properties_access == ModuleMethod::ACCESS_STATIC
   $cur_module.constants.each do |module_constant|
    if module_constant.type == MethodParam::TYPE_STRING %>
    static const char <%= module_constant.name %>[];// "<%= module_constant.value %>" <%
else %>
    static const <%= api_generator_cpp_makeNativeType(module_constant.type) %> <%= module_constant.name %> = <%= module_constant.value %>; <%
end; end; end %>

    virtual ~I<%= $cur_module.name %>Singleton(){}

//methods
<% $cur_module.methods.each do |module_method|
    next if module_method.access != ModuleMethod::ACCESS_STATIC

    params = ''
    module_method.params.each do |param|
        params += " #{api_generator_cpp_makeNativeTypeArg(param.type)} #{param.name}, "
    end

    params += 'rho::apiGenerator::CMethodResult& oResult'

%>    virtual void <%= module_method.native_name%>(<%= params%>) = 0;
<% end %>
<% if $cur_module.is_template_default_instance %>
    virtual rho::String getDefaultID() = 0;
    virtual rho::String getInitialDefaultID() = 0;
    virtual void setDefaultID(const rho::String& strID) = 0;
<% end %>
    virtual void addCommandToQueue(rho::common::IRhoRunnable* pFunctor) = 0;
    virtual void callCommandInThread(rho::common::IRhoRunnable* pFunctor) = 0;
};

struct I<%= $cur_module.name %>Factory
{
    virtual ~I<%= $cur_module.name %>Factory(){}

    virtual I<%= $cur_module.name %>Singleton* getModuleSingleton() = 0;

<% if $cur_module.is_template_default_instance %>
    virtual I<%= $cur_module.name %>* getModuleByID(const rho::String& strID) = 0;
<% end %>
};

<% $cur_module.parents.each do |parent| %>
}<%
end %>
