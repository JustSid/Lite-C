#ifndef LA_ENTITY_H_
#define LA_ENTITY_H_

ENTITY *la_ent_create(STRING *file, VECTOR *pos, EVENT entAction);
ENTITY *la_ent_createlocal(STRING *file, VECTOR *pos, EVENT entAction);
ENTITY *la_ent_createlayer(STRING *file, var flags2, var layer);

#endif
