#ifndef _TYPES_H_
#define _TYPES_H_

// Constants
#define kVariableTypeBMAP	1
#define kVariableTypeFloat	2

#define kFilterFlagReRunOnTrigger (1 << 1)

#define kVariableMaxConnections 32

// Types
typedef struct
{
	int type;
	void *buffer;
	
	void *connection[kVariableMaxConnections], *parent;
} variable_t;

typedef struct
{
	variable_t v;
	variable_t *ancestor;
	variable_t *next;
	
	BMAP *source; // Source bitmap used to receover from
} variable_fastpath_t;

typedef struct
{
	STRING *file;
	MATERIAL *material;
} filter_data_t;

typedef struct
{
	STRING *name, *description;
	
	void *callback;
	filter_data_t data;
	
	int flags;
	
	STRING **namesIn, **namesOut;
	int *input, *output;
	int numIn, numOut;
} filter_header_t;

typedef struct filter_s
{
	filter_header_t *header;
	
	BOOL dirty;
	variable_t **input;
	variable_t **output;
	
	struct filter_s *next;
} filter_t;

typedef struct sidebar_list_s
{
	filter_header_t *header; 
	PANEL *panel;
	struct sidebar_list_s *next;
} sidebar_list_t;

typedef struct workspace_panel_s
{
	PANEL *panel;
	filter_t *filter;
	variable_t *variable;
	int *isinput;
	int posX;
	int posY;
	int layerordernum;
	struct workspace_panel_s* next;
} workspace_panel_t;


// Functions
filter_header_t *header_create(const char *name, const char *description, void *callback, int input, int output);
void header_setInput(filter_header_t *header, int number, const char *name, int type);
void header_setOutput(filter_header_t *header, int number, const char *name, int type);
void header_setShader(filter_header_t *header, const char *file);

filter_t *filter_create(filter_header_t *header);
void filter_destroy(filter_t *filter);

BOOL filter_connect(filter_t *filter, variable_t *variable, int number);
void filter_unconnect(filter_t *filter, int number);

variable_t *filter_getInput(filter_t *filter, int number);
variable_t *filter_getOutput(filter_t *filter, int number);

void filter_run(filter_t *filter, BOOL passThrough);

// Fastpath conversion
void variable_convertFromFastpath(variable_t *v);
void variable_convertToFastpath(variable_t *v, variable_t *ancestor);

variable_t *variable_createBitmap(BMAP *bmap);
variable_t *variable_createFloat(float f);
variable_t *variable_clean(variable_t *v);

int variable_connectionCount(variable_t *v);

BMAP *	variable_getBMAP(variable_t *v);
float 	variable_getFloat(variable_t *v);

void variable_invalidate(variable_t *v);
void variable_destroy(variable_t *v);

#define variable_setType(v, type, value) *((type *)v->buffer) = (value)

#include "types.c"
#endif
