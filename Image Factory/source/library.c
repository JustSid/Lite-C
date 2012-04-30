#include "library.h"
#include "gui.h"
#include "persistence.h"


BMAP *bmap_copy(BMAP *copy, BMAP *source)
{
	if(!source)
		return NULL;
	
	if(copy)
	{
		if((bmap_width(copy) != bmap_width(source)) || (bmap_height(copy) != bmap_height(source)))
		{
			ptr_remove(copy);
			copy = NULL;
		}
	}
	
	if(!copy)
		copy = bmap_createblack(bmap_width(source), bmap_height(source), 32);
	
	bmap_blit(copy, source, NULL, NULL);
	return copy;
}

BOOL library_writeOutput(filter_t *filter, filter_data_t *data)
{
	BMAP *bmap = variable_getBMAP(filter_getInput(filter, 0));
	
	if(library_outputBMAP)
		ptr_remove(library_outputBMAP);
		
	library_outputBMAP = bmap_copy(NULL, bmap);
	
		
	float targetWidth, width  = bmap_width(bmap);
	float targetHeight, height = bmap_height(bmap);

	if(width > height)
	{
		targetWidth = 256.0;
		targetHeight = height / width * targetWidth;
	}
	else
	{
		targetHeight = 256.0;
		targetWidth = width / height * targetHeight;
	}
	
	_renderedImagePan->bmap = bmap_createblack(targetWidth, targetHeight, 32);
	_renderedImagePan->size_x = targetWidth;
	_renderedImagePan->size_y = targetHeight;
		
	bmap_blitpart(_renderedImagePan->bmap, bmap, NULL, vector(targetWidth, targetHeight, 0), NULL, vector(width, height, 0));
	
	return true;
}

BOOL library_runShader(filter_t *filter, filter_data_t *data)
{
	MATERIAL *mat = data->material;
	if(!mat)
	{
		mat = mtl_create();
		void *effect = effect_load(mat, data->file);
		
		if(mat == NULL || effect == NULL)
			return false;
	}
	
	int i;
	int in = filter->header->numIn;
	int out = filter->header->numOut;
	
	variable_t *output = NULL;
	for(i=0; i<out; i++)
	{
		variable_t *v = filter_getOutput(filter, i);
		
		if(v->type == kVariableTypeBMAP)
		{
			output = v;
			break;
		}
	}
	
	if(!output)
		return false;
	
	
	// Now that we found the output, gather the input
	int bmaps = 0;
	int floats = 0;
	
	BMAP **skins = &mat->skin1;
	
	for(i=0; i<in; i++)
	{
		variable_t *v = (filter->input)[i];
		switch(v->type)
		{
			case kVariableTypeBMAP:
			{
				BMAP *b = variable_getBMAP(v);
				skins[bmaps] = b;
				
				bmaps ++;
				break;
			}
				
			default:
			{
				float f = variable_getFloat(v);
				mat->skill[floats] = floatv(f);
				
				floats ++;
				break;
			}
		}
	}
	
	BMAP *t = bmap_copy(variable_getBMAP(output), skins[0]);
	bmap_process(t, skins[0], mat);

	output->buffer = (void *)t;
	return true;
}

BOOL library_bmapCreate(filter_t *filter, filter_data_t *data)
{
	int width  = (int)variable_getFloat(filter_getInput(filter, 0));
	int height = (int)variable_getFloat(filter_getInput(filter, 1));
	
	if(width <= 0 || height <= 0)
		return false;
	
	variable_t *output = variable_clean(filter_getOutput(filter, 0));
	variable_setType(output, BMAP *, bmap_createblack(width, height, 24));
	
	return true;
}

BOOL library_vectorCreate(filter_t *filter, filter_data_t *data)
{
	float value  = variable_getFloat(filter_getInput(filter, 0));
	
	variable_t *out1 = filter_getOutput(filter, 0);
	variable_t *out2 = filter_getOutput(filter, 1);
	variable_t *out3 = filter_getOutput(filter, 2);
	
	variable_setType(out1, float, value);
	variable_setType(out2, float, value);
	variable_setType(out3, float, value);
	
	return true;
}

BOOL library_bmapExplode(filter_t *filter, filter_data_t *data)
{
	BMAP *source = variable_getBMAP(filter_getInput(filter, 0));
	if(!source)
		return false;
	
	variable_t *out = filter_getOutput(filter, 0);
	
	BMAP *bmap = bmap_copy(variable_getBMAP(out), source);
	variable_setType(out, BMAP *, bmap);
	
	
	variable_t *width  = filter_getOutput(filter, 1);
	variable_t *height = filter_getOutput(filter, 2);
	
	variable_setType(width, float, (float)bmap_width(bmap));
	variable_setType(height, float, (float)bmap_height(bmap));
	
	return true;
}

BOOL library_bmapBlit(filter_t *filter, filter_data_t *data)
{
	BMAP *dest 		= variable_getBMAP(filter_getInput(filter, 0));
	BMAP *source 	= variable_getBMAP(filter_getInput(filter, 1));
	
	if(dest == NULL || source == NULL)
		return false;
	
	
	float startX = variable_getFloat(filter_getInput(filter, 2));
	float startY = variable_getFloat(filter_getInput(filter, 3));
	
	float sizeX = variable_getFloat(filter_getInput(filter, 4));
	float sizeY = variable_getFloat(filter_getInput(filter, 5));
	
	
	variable_t *out = filter_getOutput(filter, 0);
	
	BMAP *bmap = bmap_copy(variable_getBMAP(out), dest);
	bmap_blit(dest, source, vector(startX, startY, 0), vector(sizeX, sizeY, 0));
	variable_setType(out, BMAP *, bmap);

	return true;
}

// Math filter
BOOL library_mathAddFilter(filter_t *filter, filter_data_t *data)
{
	float a = variable_getFloat(filter_getInput(filter, 0));
	float b = variable_getFloat(filter_getInput(filter, 1));
	 
	variable_t *out = filter_getOutput(filter, 0);
	variable_setType(out, float, (a + b));
}

BOOL library_mathSubFilter(filter_t *filter, filter_data_t *data)
{
	float a = variable_getFloat(filter_getInput(filter, 0));
	float b = variable_getFloat(filter_getInput(filter, 1));
	 
	variable_t *out = filter_getOutput(filter, 0);
	variable_setType(out, float, (a - b));
}

BOOL library_mathMulFilter(filter_t *filter, filter_data_t *data)
{
	float a = variable_getFloat(filter_getInput(filter, 0));
	float b = variable_getFloat(filter_getInput(filter, 1));
	 
	variable_t *out = filter_getOutput(filter, 0);
	variable_setType(out, float, (a * b));
}

BOOL library_mathDivFilter(filter_t *filter, filter_data_t *data)
{
	float a = variable_getFloat(filter_getInput(filter, 0));
	float b = variable_getFloat(filter_getInput(filter, 1));
	 
	variable_t *out = filter_getOutput(filter, 0);
	variable_setType(out, float, (a / b));
}




void createOutputFilter()
{
	// Output
	filter_header_t *header = header_create("", "", library_writeOutput, 1, 0);
	header_setInput(header, 0, "", kVariableTypeBMAP);
	
	library_outputFilter = filter_create(header);
}

void createHardHeaders()
{
	filter_header_t *header;
	
	// Bitmap Create header
	header = header_create("Create bitmap", "Creates an empty bitmap!", library_bmapCreate, 2, 1);	
	header_setInput(header, 0, "Width", kVariableTypeFloat);
	header_setInput(header, 1, "Height", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Bitmap", kVariableTypeBMAP);
	generateSidebarEntry(header);	
	
	// Combine header
	header = header_create("Combine", "Takes a number and returns three\nvariables with the same value", library_vectorCreate, 1, 3);	
	header_setInput(header, 0, "Input", kVariableTypeFloat);
	
	header_setOutput(header, 0, "X", kVariableTypeFloat);
	header_setOutput(header, 1, "Y", kVariableTypeFloat);
	header_setOutput(header, 2, "Z", kVariableTypeFloat);
	
	generateSidebarEntry(header);	
	
	// Bitmap explode
	header = header_create("Bitmap explode", "Takes a bitmap and returns\nits components", library_bmapExplode, 1, 3);	
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	
	header_setOutput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setOutput(header, 1, "Width", kVariableTypeFloat);
	header_setOutput(header, 2, "Height", kVariableTypeFloat);
	
	generateSidebarEntry(header);	
	
	// BMAP blit
	header = header_create("Bitmap blit", "Blits one bitmap over another.", library_bmapBlit, 6, 1);	
	header_setInput(header, 0, "Bitmap 1", kVariableTypeBMAP);
	header_setInput(header, 1, "Bitmap 2", kVariableTypeBMAP);
	header_setInput(header, 2, "Dst X", kVariableTypeFloat);
	header_setInput(header, 3, "Dst Y", kVariableTypeFloat);
	header_setInput(header, 4, "Size X", kVariableTypeFloat);
	header_setInput(header, 5, "Size Y", kVariableTypeFloat);
	
	header_setOutput(header, 0, "BMAP", kVariableTypeBMAP);
	
	generateSidebarEntry(header);
	
	// Math
	{
		header = header_create("Math Add", "Adds two numbers.", library_mathAddFilter, 2, 1);	
		header_setInput(header, 0, "A", kVariableTypeFloat);
		header_setInput(header, 1, "B", kVariableTypeFloat);
	
		header_setOutput(header, 0, "Result", kVariableTypeFloat);
		generateSidebarEntry(header);
	
		//
		header = header_create("Math Sub", "Subtracts two numbers.", library_mathSubFilter, 2, 1);	
		header_setInput(header, 0, "A", kVariableTypeFloat);
		header_setInput(header, 1, "B", kVariableTypeFloat);
	
		header_setOutput(header, 0, "Result", kVariableTypeFloat);
		generateSidebarEntry(header);
	
		//
		header = header_create("Math Mul", "Multiplies two numbers.", library_mathMulFilter, 2, 1);	
		header_setInput(header, 0, "A", kVariableTypeFloat);
		header_setInput(header, 1, "B", kVariableTypeFloat);
	
		header_setOutput(header, 0, "Result", kVariableTypeFloat);
		generateSidebarEntry(header);
	
		//
		header = header_create("Math Div", "Divides two numbers.", library_mathAddFilter, 2, 1);	
		header_setInput(header, 0, "A", kVariableTypeFloat);
		header_setInput(header, 1, "B", kVariableTypeFloat);
	
		header_setOutput(header, 0, "Result", kVariableTypeFloat);
		generateSidebarEntry(header);
	}
}

void loadHeaders()
{
	STRING *path = str_create(work_dir);
	str_cat(path, "\\headers");
	
	FILE *file = fopen(_chr(path), "rb");
	
	filter_header_t *header;
	while((header = read_headerFromFile(file)))
	{
		generateSidebarEntry(header);	
	}
	
	fclose(file);
	ptr_remove(path);
}
