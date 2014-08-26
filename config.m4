PHP_ARG_ENABLE(entity, whether to enable Entity support,
[ --enable-entity   Enable Entity support])
if test "$PHP_ENTITY" = "yes"; then
  PHP_SUBST(ENTITY_SHARED_LIBADD)
  AC_DEFINE(ENTITY, 1, [Whether you have Entity])
  PHP_NEW_EXTENSION(entity, entity.c, $ext_shared)
fi
