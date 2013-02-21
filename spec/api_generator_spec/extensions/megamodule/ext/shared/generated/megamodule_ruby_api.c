
#include "ruby.h"

static VALUE rb_mParent;
static VALUE rb_mMegamodule;

VALUE rb_Megamodule_getStringProperty(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_getStringProperty(int argc, VALUE *argv);
VALUE rb_Megamodule_getIntegerProperty(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_getIntegerProperty(int argc, VALUE *argv);
VALUE rb_Megamodule_setIntegerProperty(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_setIntegerProperty(int argc, VALUE *argv);
VALUE rb_Megamodule_typesTest(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_typesTest(int argc, VALUE *argv);
VALUE rb_Megamodule_produceArray(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_produceArray(int argc, VALUE *argv);
VALUE rb_s_Megamodule_getObjectsCount(int argc, VALUE *argv);
VALUE rb_s_Megamodule_getObjectByIndex(int argc, VALUE *argv);
VALUE rb_Megamodule_showAlertFromUIThread(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_showAlertFromUIThread(int argc, VALUE *argv);
VALUE rb_Megamodule_setPeriodicallyCallback(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_setPeriodicallyCallback(int argc, VALUE *argv);
VALUE rb_Megamodule_isPeriodicallyCallbackSetted(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_isPeriodicallyCallbackSetted(int argc, VALUE *argv);
VALUE rb_Megamodule_stopPeriodicallyCallback(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_stopPeriodicallyCallback(int argc, VALUE *argv);
VALUE rb_Megamodule_getProperty(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_getProperty(int argc, VALUE *argv);
VALUE rb_Megamodule_getProperties(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_getProperties(int argc, VALUE *argv);
VALUE rb_Megamodule_getAllProperties(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_getAllProperties(int argc, VALUE *argv);
VALUE rb_Megamodule_setProperty(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_setProperty(int argc, VALUE *argv);
VALUE rb_Megamodule_setProperties(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_setProperties(int argc, VALUE *argv);
VALUE rb_Megamodule_clearAllProperties(int argc, VALUE *argv, VALUE obj);
VALUE rb_s_Megamodule_def_clearAllProperties(int argc, VALUE *argv);
VALUE rb_s_Megamodule_enumerate(int argc, VALUE *argv);



VALUE rb_Megamodule_s_default(VALUE klass);
VALUE rb_Megamodule_s_setDefault(VALUE klass, VALUE obj);


VALUE getRuby_Megamodule_Module(){ return rb_mMegamodule; }



void Init_RubyAPI_Megamodule(void)
{

    VALUE tmpParent = Qnil;
    rb_mParent = rb_define_module("Rho");
    
    tmpParent = rb_mParent;
    rb_mParent = rb_define_module_under(tmpParent, "Examples");
    

	rb_mMegamodule = rb_define_class_under(rb_mParent, "Megamodule", rb_cObject);

    //Constructor should be not available
	//rb_define_alloc_func(rb_cBarcode1, rb_barcode1_allocate);
    //rb_undef_alloc_func(rb_mMegamodule);

    rb_define_method(rb_mMegamodule, "StringProperty", rb_Megamodule_getStringProperty, -1);
    rb_define_singleton_method(rb_mMegamodule, "StringProperty", rb_s_Megamodule_def_getStringProperty, -1);
    rb_define_method(rb_mMegamodule, "IntegerProperty", rb_Megamodule_getIntegerProperty, -1);
    rb_define_singleton_method(rb_mMegamodule, "IntegerProperty", rb_s_Megamodule_def_getIntegerProperty, -1);
    rb_define_method(rb_mMegamodule, "IntegerProperty=", rb_Megamodule_setIntegerProperty, -1);
    rb_define_singleton_method(rb_mMegamodule, "IntegerProperty=", rb_s_Megamodule_def_setIntegerProperty, -1);
    rb_define_method(rb_mMegamodule, "typesTest", rb_Megamodule_typesTest, -1);
    rb_define_singleton_method(rb_mMegamodule, "typesTest", rb_s_Megamodule_def_typesTest, -1);
    rb_define_method(rb_mMegamodule, "produceArray", rb_Megamodule_produceArray, -1);
    rb_define_singleton_method(rb_mMegamodule, "produceArray", rb_s_Megamodule_def_produceArray, -1);
    rb_define_singleton_method(rb_mMegamodule, "getObjectsCount", rb_s_Megamodule_getObjectsCount, -1);
    rb_define_singleton_method(rb_mMegamodule, "getObjectByIndex", rb_s_Megamodule_getObjectByIndex, -1);
    rb_define_method(rb_mMegamodule, "showAlertFromUIThread", rb_Megamodule_showAlertFromUIThread, -1);
    rb_define_singleton_method(rb_mMegamodule, "showAlertFromUIThread", rb_s_Megamodule_def_showAlertFromUIThread, -1);
    rb_define_method(rb_mMegamodule, "setPeriodicallyCallback", rb_Megamodule_setPeriodicallyCallback, -1);
    rb_define_singleton_method(rb_mMegamodule, "setPeriodicallyCallback", rb_s_Megamodule_def_setPeriodicallyCallback, -1);
    rb_define_method(rb_mMegamodule, "isPeriodicallyCallbackSetted", rb_Megamodule_isPeriodicallyCallbackSetted, -1);
    rb_define_singleton_method(rb_mMegamodule, "isPeriodicallyCallbackSetted", rb_s_Megamodule_def_isPeriodicallyCallbackSetted, -1);
    rb_define_method(rb_mMegamodule, "stopPeriodicallyCallback", rb_Megamodule_stopPeriodicallyCallback, -1);
    rb_define_singleton_method(rb_mMegamodule, "stopPeriodicallyCallback", rb_s_Megamodule_def_stopPeriodicallyCallback, -1);
    rb_define_method(rb_mMegamodule, "getProperty", rb_Megamodule_getProperty, -1);
    rb_define_singleton_method(rb_mMegamodule, "getProperty", rb_s_Megamodule_def_getProperty, -1);
    rb_define_method(rb_mMegamodule, "getProperties", rb_Megamodule_getProperties, -1);
    rb_define_singleton_method(rb_mMegamodule, "getProperties", rb_s_Megamodule_def_getProperties, -1);
    rb_define_method(rb_mMegamodule, "getAllProperties", rb_Megamodule_getAllProperties, -1);
    rb_define_singleton_method(rb_mMegamodule, "getAllProperties", rb_s_Megamodule_def_getAllProperties, -1);
    rb_define_method(rb_mMegamodule, "setProperty", rb_Megamodule_setProperty, -1);
    rb_define_singleton_method(rb_mMegamodule, "setProperty", rb_s_Megamodule_def_setProperty, -1);
    rb_define_method(rb_mMegamodule, "setProperties", rb_Megamodule_setProperties, -1);
    rb_define_singleton_method(rb_mMegamodule, "setProperties", rb_s_Megamodule_def_setProperties, -1);
    rb_define_method(rb_mMegamodule, "clearAllProperties", rb_Megamodule_clearAllProperties, -1);
    rb_define_singleton_method(rb_mMegamodule, "clearAllProperties", rb_s_Megamodule_def_clearAllProperties, -1);
    rb_define_singleton_method(rb_mMegamodule, "enumerate", rb_s_Megamodule_enumerate, -1);



    rb_define_singleton_method(rb_mMegamodule, "default", rb_Megamodule_s_default, 0);
    rb_define_singleton_method(rb_mMegamodule, "setDefault", rb_Megamodule_s_setDefault, 1);
    rb_define_singleton_method(rb_mMegamodule, "default=", rb_Megamodule_s_setDefault, 1);



    rb_const_set(rb_mMegamodule, rb_intern("CONST1_STRING"), rb_str_new2("value1") );
    rb_const_set(rb_mMegamodule, rb_intern("CONST2_INTEGER"), rb_ll2inum(123) );
    rb_const_set(rb_mMegamodule, rb_intern("CONST_STRING_1"), rb_str_new2("const1") );
    rb_const_set(rb_mMegamodule, rb_intern("CONST_STRING_2"), rb_str_new2("const2") );

    rb_define_alias(rb_mMegamodule, "set_period", "setPeriodicallyCallback");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "set_period", "setPeriodicallyCallback");
    rb_define_alias(rb_mMegamodule, "string_property", "StringProperty");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "string_property", "StringProperty");
    rb_define_alias(rb_mMegamodule, "integer_property", "IntegerProperty");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "integer_property", "IntegerProperty");
    rb_define_alias(rb_mMegamodule, "integer_property=", "IntegerProperty=");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "integer_property=", "IntegerProperty=");
    rb_define_alias(rb_mMegamodule, "types_test", "typesTest");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "types_test", "typesTest");
    rb_define_alias(rb_mMegamodule, "produce_array", "produceArray");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "produce_array", "produceArray");
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "get_objects_count", "getObjectsCount");
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "get_object_by_index", "getObjectByIndex");
    rb_define_alias(rb_mMegamodule, "show_alert_from_uithread", "showAlertFromUIThread");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "show_alert_from_uithread", "showAlertFromUIThread");
    rb_define_alias(rb_mMegamodule, "set_periodically_callback", "setPeriodicallyCallback");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "set_periodically_callback", "setPeriodicallyCallback");
    rb_define_alias(rb_mMegamodule, "is_periodically_callback_setted", "isPeriodicallyCallbackSetted");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "is_periodically_callback_setted", "isPeriodicallyCallbackSetted");
    rb_define_alias(rb_mMegamodule, "stop_periodically_callback", "stopPeriodicallyCallback");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "stop_periodically_callback", "stopPeriodicallyCallback");
    rb_define_alias(rb_mMegamodule, "get_property", "getProperty");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "get_property", "getProperty");
    rb_define_alias(rb_mMegamodule, "get_properties", "getProperties");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "get_properties", "getProperties");
    rb_define_alias(rb_mMegamodule, "get_all_properties", "getAllProperties");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "get_all_properties", "getAllProperties");
    rb_define_alias(rb_mMegamodule, "set_property", "setProperty");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "set_property", "setProperty");
    rb_define_alias(rb_mMegamodule, "set_properties", "setProperties");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "set_properties", "setProperties");
    rb_define_alias(rb_mMegamodule, "clear_all_properties", "clearAllProperties");
    
    rb_define_alias(rb_singleton_class(rb_mMegamodule), "clear_all_properties", "clearAllProperties");

//TODO: support module aliases
    rb_const_set(rb_mKernel, rb_intern("Megamodule"), rb_mMegamodule );
}

