#include <acknex.h>
#include <stdio.h>
#include <windows.h>  

#define PRAGMA_PATH "gfx";

#include "source\\types.h"
#include "source\\library.h"
#include "source\\persistence.h"

void write_headerToFile(filter_header_t *header, FILE *file)
{
	fwrite(_chr(header->name), 1, str_len(header->name) + 1, file);
	fwrite(_chr(header->description), 1, str_len(header->description) + 1, file);
	fwrite(_chr(header->data.file), 1, str_len(header->data.file) + 1, file);
	
	fwrite(&header->flags, sizeof(int), 1, file);
	
	int i;
	int in  = header->numIn;
	int out = header->numOut;
	
	fwrite(&in,  sizeof(int), 1, file);
	fwrite(&out, sizeof(int), 1, file);
	
	fwrite(header->input, sizeof(int), in, file);
	fwrite(header->output, sizeof(int), out, file);
	
	for(i=0; i<in; i++)
	{
		STRING *string = (header->namesIn)[i];
		if(string)
		{
			int len = str_len(string) + 1;
			fwrite(_chr(string), 1, len, file);
		}
		else
			fwrite("\0", 1, 1, file);
	}
	
	for(i=0; i<out; i++)
	{
		STRING *string = (header->namesOut)[i];
		if(string)
		{
			int len = str_len(string) + 1;
			fwrite(_chr(string), 1, len, file);
		}
		else
			fwrite("\0", 1, 1, file);
	}
	
	fflush(file);
}


void create_header(FILE *file)
{
	filter_header_t *header;
	
	// Fill BMAP
	header = header_create("Fill bitmap", "Fills a bitmap with the given color", NULL, 5, 1);	
	header_setShader(header, "fill.fx");
	header_setInput(header, 0, "Target", kVariableTypeBMAP);
	header_setInput(header, 1, "R", kVariableTypeFloat);
	header_setInput(header, 2, "G", kVariableTypeFloat);
	header_setInput(header, 3, "B", kVariableTypeFloat);
	header_setInput(header, 4, "Alpha", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Bitmap", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Blend additive
	header = header_create("Additive blend", "Blends bitmap 2 over bitmap 1\nby adding the pixels together.", NULL, 2, 1);	
	header_setShader(header, "addblend.fx");
	header_setInput(header, 0, "Bitmap 1", kVariableTypeBMAP);
	header_setInput(header, 1, "Bitmap 2", kVariableTypeBMAP);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Negative
	header = header_create("Negative", "Creates a negative version of the\ngiven bitmap.", NULL, 1, 1);	
	header_setShader(header, "negative.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Black & White
	header = header_create("Black & White", "Creates a black and white version\nof the given bitmap.", NULL, 2, 1);	
	header_setShader(header, "blackwhite.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "Treshold", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Monochrome
	header = header_create("Monochrome", "Creates a monochrome versionof the\n given bitmap.", NULL, 1, 1);	
	header_setShader(header, "monochrome.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Emboss
	header = header_create("Emboss", "Creates an emboss versionof the\n given bitmap.", NULL, 1, 1);	
	header_setShader(header, "emboss.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Sobel Edge
	header = header_create("Sobel Edge", "Processes the given image with\na sobel edge filter.", NULL, 3, 1);	
	header_setShader(header, "sobeledge.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "Brightness", kVariableTypeFloat);
	header_setInput(header, 2, "Strenght", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Color to Alpha
	header = header_create("Color to Alpha", "Converts the given color in the \ngivenimage to alpha.", NULL, 4, 1);	
	header_setShader(header, "colortoalpha.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "R", kVariableTypeFloat);
	header_setInput(header, 2, "G", kVariableTypeFloat);
	header_setInput(header, 3, "B", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Scale
	header = header_create("Scale", "Scales the given bitmap to the given\nsize.", NULL, 3, 1);	
	header_setShader(header, "scale.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "X", kVariableTypeFloat);
	header_setInput(header, 2, "Y", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Simple Blur
	header = header_create("Simple Blur", "Applies a simple blur to the\ngiven bitmap.", NULL, 2, 1);	
	header_setShader(header, "blursimple.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "Factor", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Complex Blur
	header = header_create("Complex Blur", "Applies a complex blur to the\ngiven bitmap.", NULL, 2, 1);	
	header_setShader(header, "blurcomplex.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "Factor", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Sepia
	header = header_create("Sepia", "Creates a sepia version of the\ngiven bitmap.", NULL, 3, 1);	
	header_setShader(header, "sepia.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "Desaturation", kVariableTypeFloat);
	header_setInput(header, 2, "Toning", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Frost
	header = header_create("Frost", "I have no idea what it does.", NULL, 5, 1);	
	header_setShader(header, "frost.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "Noise", kVariableTypeBMAP);
	header_setInput(header, 2, "Offset X", kVariableTypeFloat);
	header_setInput(header, 3, "Offset Y", kVariableTypeFloat);
	header_setInput(header, 4, "Frequency", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Sharpen
	header = header_create("Sharpen", "Sharpens the given bitmap.", NULL, 2, 1);	
	header_setShader(header, "sharpen.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "Strenght", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Noise
	header = header_create("Noise", "NoiseNoiseNoiseNoise", NULL, 2, 1);	
	header_setShader(header, "noise.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "Seed", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);

	// Perlin Noise
	header = header_create("Perlin Noise", "PerlinNoiseNoiseNoiseNoise", NULL, 2, 1);	
	header_setShader(header, "perlinnoise.fx");
	header_setInput(header, 0, "Bitmap", kVariableTypeBMAP);
	header_setInput(header, 1, "Seed", kVariableTypeFloat);
	
	header_setOutput(header, 0, "Result", kVariableTypeBMAP);
	write_headerToFile(header, file);
	
}


void main()
{
	STRING *path = str_create(work_dir);
	str_cat(path, "\\headers");
	
	FILE *file = fopen(_chr(path), "wb");
	create_header(file);
	fclose(file);
	
	ptr_remove(file);
	sys_exit(NULL);
}
