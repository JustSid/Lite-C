#include "helper.h"
#include "types.h"

// -----------------------
// Filter
// -----------------------

variable_t *variable_create(int type, void *buffer);

filter_header_t *header_create(const char *name, const char *description, void *callback, int input, int output)
{
	filter_header_t *header = malloc(sizeof(filter_header_t));
	header->name = str_create(name);
	header->description = str_create(description);
	
	header->callback = callback;
	header->flags = 0;
	
	header->numIn  = input;
	header->numOut = output;
	
	header->input = calloc(input, sizeof(int));
	header->output = calloc(output, sizeof(int));
	
	header->namesIn = calloc(input, sizeof(STRING *));
	header->namesOut = calloc(output, sizeof(STRING *));
	
	return header;
}

void header_setInput(filter_header_t *header, int number, const char *name, int type)
{
	(header->namesIn)[number] = str_create(name);
	(header->input)[number]	  = type;
}

void header_setOutput(filter_header_t *header, int number, const char *name, int type)
{
	(header->namesOut)[number]	= str_create(name);
	(header->output)[number] 	= type;
}

void header_setShader(filter_header_t *header, const char *file)
{
	header->data.file = str_create(file);
	header->data.material = NULL;
}


filter_t *filter_create(filter_header_t *header)
{
	filter_t *filter = calloc(1, sizeof(filter_t));
	filter->header = header;
	
	filter->input  = calloc(header->numIn, sizeof(variable_t *));
	filter->output = calloc(header->numOut, sizeof(variable_t *));
	
	int i;
	for(i=0; i<header->numOut; i++)
	{
		variable_t *v;		
		switch((header->output)[i])
		{
			case kVariableTypeBMAP:
				v = variable_create((header->output)[i], NULL);
				break;
			
			case kVariableTypeFloat:
				v = variable_createFloat(0.0);
				break;
		}
		
		v->parent = (void *)filter;
		(filter->output)[i] = v;
	}
	
	return filter;
}

void filter_unconnect2(filter_t *filter, variable_t *vx)
{
	int i, length = filter->header->numIn;
	for(i=0; i<length; i++)
	{
		variable_t *v = (filter->input)[i];
		if(v == vx)
		{
			(filter->input)[i] = NULL;
			break;
		}
	}
}

void filter_destroy(filter_t *filter)
{
	int i, j, length = filter->header->numIn;
	for(i=0; i<length; i++)
	{
		variable_t *v = (filter->input)[i];
		if(!v)
			continue;
		
		filter_unconnect(filter, i);
	}
	
	length = filter->header->numOut;
	for(i=0; i<length; i++)
	{
		variable_t *v = (filter->output)[i];
		if(!v)
			continue;
	
	
		for(j=0; j<kVariableMaxConnections; j++)
		{
			filter_t *tfilter = (v->connection)[j];
			
			if(tfilter)
				filter_unconnect2(tfilter, v);
		}
	}
	
	free(filter->input);
	free(filter->output);
	free(filter);
}


void filter_run(filter_t *filter, BOOL passThrough)
{
	BOOL _fcallback(filter_t *, filter_data_t *);
	_fcallback = filter->header->callback;
	
	// Check the requirements
	int i, j;
	for(i=0; i<filter->header->numIn; i++)
	{
		variable_t *v = (filter->input)[i];
		
		if(!v)
			return;
		
		if(v->type == v->type != kVariableTypeBMAPFastPath)
		{
			if(fv->ancestor)
				*((BMAP **)v->buffer) = *((BMAP **)fv-ancestor->buffer);
			
		}
		
		if(!v->buffer)
			return;
	}

	if(!_fcallback(filter, &filter->header->data))
		return;
	
	if(passThrough)
	{
		for(i=0; i<filter->header->numOut; i++)
		{
			variable_t *v = (filter->output)[i];

			for(j=0; j<kVariableMaxConnections; j++)
			{
				filter_t *filter = (v->connection)[j];
				
				if(filter)
					filter_run(filter, true);
			}
		}
	}
}

int variable_connectionEqualTo(variable_t *v, filter_t *filter)
{
	int i;
	for(i=0; i<kVariableMaxConnections; i++)
	{
		if((v->connection)[i] == filter)
			return i;
	}
	
	return -1;
}

BOOL filter_connect(filter_t *filter, variable_t *variable, int number)
{
	if(variable->type == (filter->header->input)[number])
	{
		int index = variable_connectionEqualTo(variable, NULL);
		if(index == -1)
		{
			printf("Max connections per var reached! You can connect a var max %i times to a filter!", kVariableMaxConnections);
			return false;
		}
		
		(filter->input)[number] = variable;
		(variable->connection)[index] = filter;
		
		if(variable_connectionCount(variable) > 1)
			variable_convertFromFastpath(variable);
		
		filter_run(filter, true);
	}
	else
		return false;
	
	return true;
}

void filter_unconnect(filter_t *filter, int number)
{
	variable_t *v = (filter->input)[number];

	(filter->input)[number] = NULL;
	(v->connection)[variable_connectionEqualTo(v, filter)] = NULL;
	
	if(variable_connectionCount(variable) <= 1)
	{
		// TODO: Implement a way back!
	}
}


variable_t *filter_getInput(filter_t *filter, int number)
{
	variable_t *v = (filter->input)[number];
	return v;
}

variable_t *filter_getOutput(filter_t *filter, int number)
{
	variable_t *v = (filter->output)[number];
	return v;
}


// -----------------------
// Variables
// -----------------------

// Conversion
void variable_convertFromFastpath(variable_t *v)
{
	if(v->type == kVariableTypeBMAPFastPath)
	{
		variable_fastpath_t *fv = (variable_fastpath_t *)v;
		v->type = kVariableTypeBMAP;
	
	
		BMAP *bmap;
		if(fv->source)
		{
			bmap_copy(NULL, fv->source);
			ptr_remove(fv->source);
			fv->source = NULL;
		}
		else
		{
			bmap = bmap_copy(NULL, *((BMAP **)fv->ancestor->buffer));
		}
		
		*((BMAP **)v->buffer) = bmap;
		
		// Invalidate the chain
		int i;
		for(i=0; i<kVariableMaxConnections; i++)
		{
			filter_t *filter = (v->connection)[i];
			if(filter)
				run_filter(filter, true);
		}
			
		if(fv->next)
		{
			variable_convertFromFastpath(fv->next, bmap);
			fv->next = NULL;
		}
	}
}

void variable_convertToFastpath(variable_t *v, variable_t *ancestor)
{
	if(v->type == kVariableTypeBMAP)
	{
		variable_fastpath_t *fv = (variable_fastpath_t *)v;
		v->type = kVariableTypeBMAPFastPath;
		
		fv->ancestor = ancestor;
		fv->next = NULL;
		
		
		if(ancestor)
		{
			ptr_remove(*((BMAP **)v->buffer));
			*((BMAP **)v->buffer) = NULL;
		}
		else
		{
			fv->source = bmap_copy(NULL, *((BMAP **)v->buffer));
		}
		
		
		int i, j;
		for(i=0; i<kVariableMaxConnections; i++)
		{
			filter_t *filter = (v->connection)[i];
			if(filter)
			{
				for(j=0; j<filter->header->numOut; i++)
				{
					
				}
			}
		}
	}
}

// Creation
variable_t *variable_create(int type, void *buffer)
{
	variable_t *v = calloc(1, sizeof(variable_t));
	v->type = type;
	v->buffer = buffer;
	
	return v;
}

variable_fastpath_t *variable_fastpath_create(BMAP *bmap)
{
	variable_fastpath_t *v = calloc(1, sizeof(variable_fastpaht_t));
	v->v.type = kVariableTypeBMAPFastPath;
	v->v.buffer = falloc(&bmap, sizeof(BMAP *));
	v->source = bmap;
	
	return v;
}

variable_t *variable_createBitmap(BMAP *bmap)
{
	return variable_fastpath_create(bmap);
}

variable_t *variable_createFloat(float f)
{
	return variable_create(kVariableTypeFloat, falloc(&f, sizeof(float)));
}

variable_t *variable_clean(variable_t *v)
{
	if(v->type == kVariableTypeBMAP && v->buffer)
	{
		ptr_remove(*((BMAP **)v->buffer));
		*((BMAP **)v->buffer) = NULL;
	}
	else if(v->type == kVariableTypeBMAPFastPath)
	{
		variable_fastpath_t *fv = (variable_fastpath_t *)v;
		bmap_blit(*((BMAP **)v->buffer), fv->source, NULL, NULL);
	}
	
	return v;
}
	
int variable_connectionCount(variable_t *v)
{
	int i, count = 0;
	for(i=0; i<kVariableMaxConnections; i++)
	{
		if((v->connection)[i] != NULL)
			count ++;
	}
	
	return count;
}
	

// Triggering
void variable_update(variable_t *v)
{
	if(v->parent)
	{
		filter_t *filter = (filter_t *)v->parent;
		
		if(filter->header->flags & kFilterFlagReRunOnTrigger)
			filter_run(filter, false);
	}
}

void variable_invalidate(variable_t *v)
{
	int j;
	for(j=0; j<kVariableMaxConnections; j++)
	{
		filter_t *filter = (v->connection)[j];
				
		if(filter)
			filter_run(filter, true);
	}
}

void variable_destroy(variable_t *v)
{
	int j;
	for(j=0; j<kVariableMaxConnections; j++)
	{
		filter_t *filter = (v->connection)[j];
				
		if(filter)
			filter_unconnect2(filter, v);
	}
	
	if(v->type != kVariableTypeBMAP)
		free(v->buffer);
	
	free(v);
}

BMAP *variable_getBMAP(variable_t *v)
{
	variable_update(v);
	return *((BMAP **)v->buffer);
}

float variable_getFloat(variable_t *v)
{
	variable_update(v);
	return *((float *)v->buffer);
}
