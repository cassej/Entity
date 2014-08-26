#ifndef PHP_ENTITY_H
#define PHP_ENTITY_H
#ifdef ZTS
#include <TSRM.h>
#endif

#define PHP_ENTITY_VERSION         "1.0"
#define PHP_ENTITY_EXTNAME         "Entity"

typedef struct _zend_entity_object
{
    zend_object std;
    zval *data;
} zend_entity_object;

zend_object_value entity_create_object(zend_class_entry *class_type TSRMLS_DC);
void entity_free_storage(void *object TSRMLS_DC);

void entity_set_property(zval **object, zval *value TSRMLS_DC);
zval *entity_get_property(zval *object TSRMLS_DC);
int entity_cast(zval *readobj, zval *retval, int type TSRMLS_DC);

void entity_property_write_property(zval *object, zval *member, zval *value, const struct _zend_literal *key TSRMLS_DC);


int entity_property_has_dimension(zval *object, zval *member, int check_empty TSRMLS_DC);

void entity_property_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC);
zval *entity_property_read_dimension(zval *object, zval *offset, int type TSRMLS_DC);

PHP_MINIT_FUNCTION(entity);
PHP_MSHUTDOWN_FUNCTION(entity);
PHP_MINFO_FUNCTION(entity);
PHP_RINIT_FUNCTION(entity);
PHP_RSHUTDOWN_FUNCTION(entity);

PHP_METHOD(Entity, __construct);
PHP_METHOD(Entity, getChanges);
PHP_METHOD(Entity, getData);

extern zend_module_entry entity_module_entry;

#endif
