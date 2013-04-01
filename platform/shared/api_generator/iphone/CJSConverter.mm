
#import "CJSConverter.h"
#import "api_generator/js_helpers.h"
#import "CMethodResult.h"

@implementation CJSConverter




+ (NSString*) convertToJS:(NSObject*)objectiveC_value level:(int)level {

    NSString* strRes = @"";
    //if (level == 0)
    //    strRes = [NSString stringWithUTF8String:"{\"jsonrpc\": \"2.0\""];
    
    if ([objectiveC_value isKindOfClass:[NSString class]]) {
        // string
        NSString* objString = (NSString*)objectiveC_value;
        
        if (level == 0)
            strRes = [strRes stringByAppendingString:@"\"result\":\""];
        strRes = [strRes stringByAppendingString:objString];
        strRes = [strRes stringByAppendingString:@"\""];
    }
    else if ([objectiveC_value isKindOfClass:[NSNumber class]]) {
        // int, bool or float
        NSNumber* objNumber = (NSNumber*)objectiveC_value;
        NSString* str = @"";
        if ([CMethodResult isBoolInsideNumber:objNumber]) {
            if([objNumber boolValue]) {
                str = @"true";
            }
            else {
                str = @"false";
            }
        }
        else if ([CMethodResult isFloatInsideNumber:objNumber]) {
            str = [NSString stringWithFormat:@"%@", objNumber];
        }
        else {
            str = [NSString stringWithFormat:@"%@", objNumber];
        }
        if (level == 0)
            strRes = [strRes stringByAppendingString:@"\"result\":"];
        strRes = [strRes stringByAppendingString:str];
    }
    else if ([objectiveC_value isKindOfClass:[NSArray class]]) {
        // array
        if (level == 0)
            strRes = [strRes stringByAppendingString:@"\"result\":"];
        strRes = [strRes stringByAppendingString:@"["];
        NSArray* objArray = (NSArray*)objectiveC_value;
        int count = [objArray count];
        int i;
        for (i = 0; i < count; i++) {
            if (i > 0) {
                strRes = [strRes stringByAppendingString:@","];
            }
            NSObject* obj = [objArray objectAtIndex:i];
            strRes = [strRes stringByAppendingString:[CJSConverter convertToJS:obj level:(level+1)]];
        }
        strRes = [strRes stringByAppendingString:@"]"];
    }
    else if ([objectiveC_value isKindOfClass:[CRhoAPIClassInstance class]]) {
        // rubyModule
        CRhoAPIClassInstance* rubyModule = (CRhoAPIClassInstance*)objectiveC_value;
        NSString* rubyID = [rubyModule getInstanceID];
        NSString* rubyClass = [rubyModule getClassName];
        if (level == 0)
            strRes = [strRes stringByAppendingString:@"\"result\":"];
        strRes = [strRes stringByAppendingString:[NSString stringWithFormat:@"{\“__rhoID\”: \“%@\”,\“__rhoClass\”:\“%@\”}", rubyID, rubyClass]];
    }
    else if ([objectiveC_value isKindOfClass:[NSDictionary class]]) {
        // dictionary
        if (level == 0)
            strRes = [strRes stringByAppendingString:@",\"result\":"];
        strRes = [strRes stringByAppendingString:@"{"];
        NSDictionary* objDictionary = (NSDictionary*)objectiveC_value;
        NSEnumerator* enumerator = [objDictionary keyEnumerator];
        NSObject* obj = nil;
        BOOL is_first = YES;
        while ((obj = [enumerator nextObject]) != nil) {
            NSString* objKey = (NSString*)obj;
            NSObject* objValue = [objDictionary objectForKey:objKey];
            if (!is_first) {
               strRes = [strRes stringByAppendingString:@","]; 
            }
            strRes = [strRes stringByAppendingString:@"\""];
            strRes = [strRes stringByAppendingString:objKey];
            strRes = [strRes stringByAppendingString:@"\":"];
            
            strRes = [strRes stringByAppendingString:[CJSConverter convertToJS:objValue level:(level+1)]];
            
            is_first = NO;
        }
        strRes = [strRes stringByAppendingString:@"}"];
    }
    else if ([objectiveC_value isKindOfClass:[NSNull class]]) {
        if (level == 0)
            strRes = [strRes stringByAppendingString:@"\"result\":"];
        strRes = [strRes stringByAppendingString:@"null"];
    }
    //if (level == 0)
    //    strRes = [strRes stringByAppendingString:@",\"id\": 1}"];
    
    if ([strRes length] <= 0) {
        return @"null";
    }
    return strRes;
    
    
 return nil;
}

+ (NSObject*) convertFromJSentry:(rho::json::CJSONEntry*)json_entry rho_api_param:(RHO_API_PARAM*)rho_api_param {
    rho::json::CJSONEntry* jsonEntry = (rho::json::CJSONEntry*)json_entry;
    

    if (jsonEntry->isString()) {
        return [NSString stringWithUTF8String:jsonEntry->getString()];
    }
    if (jsonEntry->isInteger()) {
        return [NSNumber numberWithInt:jsonEntry->getInt()];
    }
    if (jsonEntry->isFloat()) {
        return [NSNumber numberWithDouble:jsonEntry->getDouble()];
    }
    if (jsonEntry->isBoolean()) {
        return [NSNumber numberWithBool:(jsonEntry->getBoolean() == true)];
    }
    if (jsonEntry->isArray()) {
        rho::json::CJSONArray js_array(*jsonEntry);
        int i;
        int count = js_array.getSize();
        NSMutableArray* ns_array = [NSMutableArray arrayWithCapacity:count];
        for (i =0 ; i < count; i++) {
            rho::json::CJSONEntry js_entry = js_array[i];
            [ns_array addObject:[CJSConverter convertFromJSentry:&js_entry rho_api_param:NULL]];
        }
        return ns_array;
    }
    if (jsonEntry->isObject()) {
        rho::json::CJSONStructIterator js_iterator(*jsonEntry);
        
        NSMutableDictionary* ns_hash = [NSMutableDictionary dictionaryWithCapacity:10];
        
        while (!js_iterator.isEnd()) {
            rho::String key = js_iterator.getCurKey();
            rho::json::CJSONEntry js_entry = js_iterator.getCurValue();
            
            [ns_hash setObject:[CJSConverter convertFromJSentry:&js_entry rho_api_param:NULL] forKey:[NSString stringWithUTF8String:(key.c_str())]];
            
            js_iterator.next();
        }
        
        // check for object
        if (([ns_hash objectForKey:@"__rhoID"] != nil) && ([ns_hash objectForKey:@"__rhoClass"] != nil)) {
            NSString* obj_id = (NSString*)[ns_hash objectForKey:@"__rhoID"];
            return obj_id;
        }
        
        return ns_hash;
    }
    return nil;
}


+ (NSObject*) convertFromJS:(void*)json_entry rho_api_param:(RHO_API_PARAM*)rho_api_param {
    rho::json::CJSONEntry* jsonEntry = (rho::json::CJSONEntry*)json_entry;
    
    
    //TODO:
    @try {
        return [CJSConverter convertFromJSentry:jsonEntry rho_api_param:rho_api_param];
    }
    @catch (NSException *exception) {
        
    }
    @finally {
        
    }
    
    
    
    
    return nil;
}

+ (NSObject*) getObjectiveCNULL {
    return [CMethodResult getObjectiveCNULL];
}

@end
