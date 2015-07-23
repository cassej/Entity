#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "php_output.h"
#include <php/Zend/zend_exceptions.h>
#include <php/Zend/zend_hash.h>


#include "php_entity.h"

static zend_function_entry entity_class_functions[] = {
    PHP_ME(Entity, __construct, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Entity, getChanges, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Entity, getData, NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

zend_module_entry entity_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_ENTITY_EXTNAME,
    NULL,
    PHP_MINIT(entity),
    NULL,
    NULL,
    NULL,
    PHP_MINFO(entity),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_ENTITY_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};



#ifdef COMPILE_DL_ENTITY
        ZEND_GET_MODULE(entity)
#endif


zend_class_entry    * entity_class_entry;
zend_object_handlers  entity_object_handlers;

static zend_bool if_exists(HashTable *ht, void *pData, zend_hash_key *hash_key, void *pParam) {


    zval  **ppzval;
    if (zend_hash_find(ht, hash_key->arKey, hash_key->nKeyLength, (void **)&ppzval) == SUCCESS) {

        return 1;
    }

    return 0;
}

void init_entity(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "Entity", entity_class_functions);
    ce.create_object = entity_create_object;
    entity_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
}

zend_object_value entity_create_object(zend_class_entry *class_type TSRMLS_DC)
{
    zend_object_value retval;
    zend_entity_object *context;

    // Allocate the actual struct
    context = (zend_entity_object*)emalloc(sizeof(zend_entity_object));
    memset(context, 0, sizeof(zend_entity_object));

    zend_object_std_init(&context->std, class_type TSRMLS_CC);

#if PHP_VERSION_ID < 50399
    zval *tmp;

    zend_hash_copy(context->std.properties,
        &class_type->default_properties,
        (copy_ctor_func_t) zval_add_ref,
        (void*)&tmp,
        sizeof(zval *)
    );
#else
    object_properties_init(&(context->std), class_type);
#endif

    retval.handle = zend_objects_store_put(
        context,
        (zend_objects_store_dtor_t)zend_objects_destroy_object,
        entity_free_storage,
        NULL TSRMLS_CC
    );

    memcpy(&entity_object_handlers,
        zend_get_std_object_handlers(), sizeof(zend_object_handlers));

    retval.handlers = &entity_object_handlers;

    entity_object_handlers.clone_obj = NULL;

    entity_object_handlers.write_property = entity_property_write_property;

    entity_object_handlers.write_dimension = entity_property_write_dimension;
    entity_object_handlers.read_dimension = entity_property_read_dimension;
    entity_object_handlers.has_dimension = entity_property_has_dimension;

    entity_object_handlers.get = entity_get_property;
    entity_object_handlers.set = entity_set_property;
    entity_object_handlers.cast_object = entity_cast;

    context->data = NULL;
    MAKE_STD_ZVAL(context->data);
    array_init(context->data);
    convert_to_array(context->data);

    return retval;
}

void entity_free_storage(void *object TSRMLS_DC)
{
    zend_entity_object *context = (zend_entity_object*)object;

    if(context->data) {
        zval_dtor(context->data);
        efree(context->data);
    }

    efree(context);
}

void entity_property_write_property(zval *object, zval *member, zval *value, const struct _zend_literal *key TSRMLS_DC)
{
    if (zend_get_std_object_handlers()->has_property(object, member, 2, key)) {
        zend_property_info *property_info;
        property_info = zend_get_property_info(Z_OBJCE_P(object), member, 1 TSRMLS_CC);

        if (property_info->flags&ZEND_ACC_PUBLIC) {
            zend_entity_object *obj = (zend_entity_object*)zend_object_store_get_object(object TSRMLS_CC);


            Z_ADDREF_P(value);
            add_assoc_zval(obj->data, Z_STRVAL_P(member), value);
        }

        Z_ADDREF_P(value);
        zend_std_write_property(object, member, value, key TSRMLS_DC);
    }
}

void entity_property_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC)
{
    entity_property_write_property(object, offset, value, NULL);
}

zval *entity_property_read_dimension(zval *object, zval *offset, int type TSRMLS_DC)
{
    return zend_get_std_object_handlers()->read_property(object, offset, type, NULL TSRMLS_DC);
}

int entity_property_has_dimension(zval *object, zval *member, int check_empty TSRMLS_DC)
{
    return zend_get_std_object_handlers()->has_property(object, member, check_empty, NULL);
}

void entity_set_property(zval **object, zval *value TSRMLS_DC)
{
    zend_get_std_object_handlers()->set(object, value TSRMLS_DC);
}

zval *entity_get_property(zval *object TSRMLS_DC)
{
    return zend_get_std_object_handlers()->get(object TSRMLS_DC);
}

int entity_cast(zval *readobj, zval *retval, int type TSRMLS_DC)
{
    return zend_get_std_object_handlers()->cast_object(readobj, retval, type TSRMLS_DC);
}

PHP_MINIT_FUNCTION(entity)
{
    init_entity(TSRMLS_C);
    return SUCCESS;
}

PHP_MINFO_FUNCTION(entity)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "Entity", "enabled");
    php_info_print_table_row(2, "Entity", PHP_ENTITY_VERSION);
    php_info_print_table_end();
}

PHP_METHOD(Entity, __construct)
{
    zval *zdata = NULL;
    MAKE_STD_ZVAL(zdata);
    HashTable *data;
    zend_bool new_entity = 0;

    zend_entity_object *obj = (zend_entity_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zb", &zdata, &new_entity) == FAILURE) {
        if(Z_TYPE_P(zdata) != IS_NULL || Z_TYPE_P(zdata) != IS_ARRAY) {
            return;
        }
    }

    if(ZEND_NUM_ARGS() > 0) {
        Z_ADDREF_P(zdata);
        convert_to_array(zdata);
        data = Z_ARRVAL_P(zdata);

        if (zend_hash_num_elements(data) > 0) {
            zend_hash_merge_ex(Z_OBJPROP_P(getThis()), data, (copy_ctor_func_t) zval_add_ref, sizeof(zval *), (merge_checker_func_t)if_exists, NULL);
        }

        if (new_entity) {
            zval **value;
            char *key;
            ulong key_index;

            zval *val = NULL;
            MAKE_STD_ZVAL(val);

            zend_hash_internal_pointer_reset(data);

            while(zend_hash_has_more_elements(data) == SUCCESS) {
                zend_hash_get_current_key(data, &key, &key_index, 0);
                zend_hash_get_current_data(data, (void**) &value );
                zend_hash_move_forward(data);

                if (zend_hash_exists(Z_OBJPROP_P(getThis()), key, strlen(key))) {
                    ZVAL_COPY_VALUE(val, *value);
                    add_assoc_zval(obj->data, key, *value);
                }
            }
        }
    }
}

PHP_METHOD(Entity, getChanges)
{
    zend_entity_object *obj = (zend_entity_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
    zval *val = NULL;
    MAKE_STD_ZVAL(val);
    ZVAL_COPY_VALUE(val, obj->data);


    RETURN_ZVAL(val, 1, 0);
}

PHP_METHOD(Entity, getData)
{
    array_init(return_value);

    zval **value, *member;
    char *key;
    ulong key_index;
    MAKE_STD_ZVAL(member);

    zend_property_info *property_info;
    zend_hash_internal_pointer_reset(Z_OBJPROP_P(getThis()));

    while(zend_hash_has_more_elements(Z_OBJPROP_P(getThis())) == SUCCESS) {
        zend_hash_get_current_key(Z_OBJPROP_P(getThis()), &key, &key_index, 0);
        zend_hash_get_current_data(Z_OBJPROP_P(getThis()), (void**) &value );
        zend_hash_move_forward(Z_OBJPROP_P(getThis()));

        ZVAL_STRING(member, estrdup(key), 1);

        property_info = zend_get_property_info(Z_OBJCE_P(getThis()), member, 1 TSRMLS_CC);
        if (zend_get_std_object_handlers()->has_property(getThis(), member, 2, NULL) && (property_info->flags & ZEND_ACC_PUBLIC) ) {
            Z_ADDREF_PP(value);
            add_assoc_zval(return_value, key, *value);
        }
    }

    convert_to_array(return_value);
}
