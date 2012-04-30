void *falloc(void *buffer, size_t size)
{
	void *retVal = malloc(size);
	memcpy(retVal, buffer, size);
	return retVal;
}

void *calloc(int count, size_t size)
{
	void *buffer = malloc(count * size);
	memset(buffer, 0, count * size);
	return buffer;
}

char *strmcpy(const char *source)
{
	char *memory = malloc((strlen(source) + 1) * sizeof(char));
	strcpy(memory, source);
	return memory;
}

#define isspace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r')
#define isdigit(c) (c >= '0' && c <= '9')
#define isdelimiter(c) (isspace(c) || c == ';' || c == '+' || c == '/' || c == '-' || c == '*')

BOOL isNumber(const char *string)
{
	while(*string != '\0')
	{
		if(isdigit(*string) || *string == '.')
		{
			string ++;
			continue;
		}
		
		if(isdelimiter(*string))
			return true;
		
		return false;
	}
	
	return true;
}

double atof(const char *string)
{
	while(*string == ' ')
		string ++; // Skip any leading whitespace

	double res = 0.0;
	BOOL negative = (*string == '-');
	BOOL positive = (*string == '+');

	int  exponent = 0;
	char character;


	if(negative || positive)
		string ++; // Skip the '-' or '+' sign

	while((character = *string) != '\0' && isdigit(character)) 
	{
		double value = (double)(character - '0');
		res = (res * 10.0) + value;
		
		string ++;
	}

	if(character == '.')
	{
		while((character = *string) != '\0' && isdigit(character)) 
		{
			double value = (double)(character - '0');
			res = (res * 10.0) + value;

			exponent --;
			string ++;
		}
	}
	
   	// Process the exponent
	while(exponent > 0)
	{
		res *= 10.0;
		exponent --;
	}
	while(exponent < 0)
	{
		res *= 0.1;
		exponent ++;
	}

	if(negative)
		return -res;
	
	return res;
}

char *strstr(char *str1, const char *str2)
{
	long index = 0;
	long length = strlen(str2);
	char *start = NULL;
	
	while(*str1 != '\0')
	{
		if(*str1 == str2[index])
		{
			if(index == 0)
				start = str1;
			
			index ++;
			if(index == length)
				return start;
		}
		else
			index = 0;
		
		str1 ++;
	}
	
	return NULL;
}

char *strpbrk(char *str1, const char *str2)
{
	long length = strlen(str2);
	
	while(*str1 != '\0')
	{
		long i;
		for(i=0; i<length; i++)
		{
			if(*str1 == str2[i])
				return str1;
		}
		
		str1 ++;
	}
	
	return NULL;
}

char *strcnt(char *str1, const char *str2)
{
	int count = 0;
	while(str1)
	{
		str1 = strstr(str1, str2) + strlen(str2);
		count ++;
	}
	
	return count;
}

char *buffer_fromFile(const char *path)
{
	FILE *file = fopen(path, "rb");
	long size;
	
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);
	
	char *buffer = malloc(size);
	fread(buffer, 1, size, file);
	fclose(file);
	
	return buffer;
}
