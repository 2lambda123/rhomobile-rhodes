
#import <Foundation/Foundation.h>

#import "api_generator/iphone/IMethodResult.h"

<% $iphone_types = {}
$iphone_types["BOOLEAN"] = 'BOOL'
$iphone_types["INTEGER"] = 'int'
$iphone_types["FLOAT"] = 'float'
$iphone_types["STRING"] = 'NSString*'
$iphone_types["ARRAY"] = 'NSArray*'
$iphone_types["HASH"] = 'NSDictionary*'
$iphone_types["SELF_INSTANCE"] = 'id<I'+$cur_module.name+'>' %>


<% $cur_module.constants.each do |module_constant|
   cline = ''
   cline = cline + '#define '
   cline = cline + module_constant.name + ' '
   if module_constant.type == MethodParam::TYPE_STRING
        cline = cline + '@"'+module_constant.value + '"'
   else
        cline = cline + module_constant.value
   end
%><%= cline %>
<%
   end
 %>



@protocol I<%= $cur_module.name %> <NSObject>

<% $cur_module.methods.each do |module_method|
  if module_method.access != ModuleMethod::ACCESS_STATIC
     method_line = '-(void) '
     method_line = method_line + module_method.native_name

     if module_method.params.size == 0
        if (module_method.has_callback != ModuleMethod::CALLBACK_NONE) || (module_method.is_return_value || module_method.is_factory_method)
           method_line = method_line + ':(id<IMethodResult>)methodResult'
        end
     else
        param_index = 0
        module_method.params.each do |method_param|
           if param_index != 0
               method_line = method_line + ' ' + method_param.name
           end
           method_line = method_line + ':'
           method_line = method_line + '(' + $iphone_types[method_param.type] + ')'
           method_line = method_line + method_param.name
           param_index = param_index + 1
        end
         if (module_method.has_callback != ModuleMethod::CALLBACK_NONE) || (module_method.is_return_value  || module_method.is_factory_method)
            method_line = method_line + ' methodResult:(id<IMethodResult>)methodResult'
         end
     end
     module_method.cached_data["iphone_line"] = method_line
     method_line = method_line + ';';
%><%= method_line %>
<% end
end %>



// NOTE: if you want to hold methodResult(for periodically call callback for example) you should release it buself when you stop using it !

@end


@protocol I<%= $cur_module.name %>Singleton <NSObject>

<% if $cur_module.is_template_default_instance %>
-(NSString*) getDefaultID;
-(void) setDefaultID:(NSString*)defaultID;
-(NSString*)getInitialDefaultID;
    <% end %>

<% $cur_module.methods.each do |module_method|
  if module_method.access == ModuleMethod::ACCESS_STATIC
     method_line = '-(void) '
     method_line = method_line + module_method.native_name

     if module_method.params.size == 0
        if (module_method.has_callback != ModuleMethod::CALLBACK_NONE) || (module_method.is_return_value || module_method.is_factory_method)
           method_line = method_line + ':(id<IMethodResult>)methodResult'
        end
     else
        param_index = 0
        module_method.params.each do |method_param|
           if param_index != 0
               method_line = method_line + ' ' + method_param.name
           end
           method_line = method_line + ':'
           method_line = method_line + '(' + $iphone_types[method_param.type] + ')'
           method_line = method_line + method_param.name
           param_index = param_index + 1
        end
         if (module_method.has_callback != ModuleMethod::CALLBACK_NONE) || (module_method.is_return_value  || module_method.is_factory_method)
            method_line = method_line + ' methodResult:(id<IMethodResult>)methodResult'
         end
     end
     module_method.cached_data["iphone_line"] = method_line
     method_line = method_line + ';';
%><%= method_line %>
<% end
end %>

@end


@protocol I<%= $cur_module.name %>Factory <NSObject>
-(id<I<%= $cur_module.name %>Singleton>) get<%= $cur_module.name %>Singleton;
-(id<I<%= $cur_module.name %>>) get<%= $cur_module.name %>ByID:(NSString*)ID;
-(void) destroyObjectByID:(NSString*)ID;
@end


@interface <%= $cur_module.name %>FactorySingleton : NSObject {
}
+(id<I<%= $cur_module.name %>Factory>) get<%= $cur_module.name %>FactoryInstance;
@end


