#include "persistence.h"
#include "library.h"

STRING *fread_delimiter(FILE *file)
{
	const char buffer[1024];
	memset(buffer, 0, 1024 * sizeof(char));
	
	int i=0;	
	while(1)
	{
		fread(&buffer[i], 1, 1, file);
		
		if(buffer[i] == '\0')
			break;
		
		i ++;
	}
	
	return str_create(buffer);
}

filter_header_t *read_headerFromFile(FILE *file)
{
	if(feof(file))
		return NULL;
		
	filter_header_t *header = calloc(1, sizeof(filter_header_t));
	
	header->name = fread_delimiter(file);
	header->description = fread_delimiter(file);
	header->data.file = fread_delimiter(file);
	
	if(str_len(header->name) <= 0 && str_len(header->description) <= 0)
	{
		free(header);
		return NULL;
	}
	
	
	fread(&header->flags, sizeof(int), 1, file);
	
	fread(&header->numIn, sizeof(int), 1, file);
	fread(&header->numOut, sizeof(int), 1, file);
	
	header->input  = calloc(header->numIn, sizeof(int));
	header->output = calloc(header->numOut, sizeof(int));
	
	header->namesIn  = calloc(header->numIn, sizeof(STRING *));
	header->namesOut = calloc(header->numOut, sizeof(STRING *));
	
	header->callback = (void *)library_runShader;
	
	fread(header->input,  sizeof(int), header->numIn, file);
	fread(header->output, sizeof(int), header->numOut, file);
	
	
	int i;
	for(i=0; i<header->numIn; i++)
		(header->namesIn)[i] = fread_delimiter(file);
	
	
	for(i=0; i<header->numOut; i++)
		(header->namesOut)[i] = fread_delimiter(file);
	
	return header;
}
