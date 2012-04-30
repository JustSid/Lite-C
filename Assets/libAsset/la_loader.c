#include <windows.h>
#include <stdio.h>
#include "la_loader.h"

HANDLE 	la_mutex, la_workerHandle;
BOOL    la_paused;
la_asset *la_firstAsset = NULL;
long 	  la_threadCount = 0;

DWORD la_worker(void *unused);

int la_assetInit()
{
	la_threadCount	= 0;
	la_firstAsset 	= NULL;
	la_mutex 		= CreateMutex(NULL, FALSE, NULL);
	
	la_paused = FALSE;
	
	if(!la_mutex)
	   return 0;
	

	la_workerHandle = CreateThread(NULL, 0, la_worker, NULL, 0, NULL);
	if(!la_workerHandle)
	{
		CloseHandle(la_mutex);
		return 0;
	}

	return 1;
}

int la_pause()
{
    WaitForSingleObject(la_mutex, INFINITE);
    la_paused = TRUE;
    ReleaseMutex(la_mutex);
    
    return 1;
}

int la_unpause()
{
    WaitForSingleObject(la_mutex, INFINITE);
    
    la_paused = FALSE;
    la_workerHandle = CreateThread(NULL, 0, la_worker, NULL, 0, NULL);
    
    if(!la_workerHandle)
    {
        la_paused = TRUE;
    
        ReleaseMutex(la_mutex);
        return 0;
    }
    
    ReleaseMutex(la_mutex);
    
    return 1;
}



DWORD la_loadAsync(la_asset *asset)
{
	FILE *file = fopen(asset->filepath, "rb");
	if(!file)
	{
		WaitForSingleObject(la_mutex, INFINITE);
		asset->state = LA_ASSET_STATE_NOSUCHFILE;
		ReleaseMutex(la_mutex);
		
		return;
	}
	
	// Get the size of the file
	fseek(file, 0, SEEK_END);
	asset->size = ftell(file);
	rewind(file);
	
	
	// Read the file
	asset->buffer = (void *)malloc(asset->size);
	if(!asset->buffer)
	{
		WaitForSingleObject(la_mutex, INFINITE);
		asset->state = LA_ASSET_STATE_READFAIL;
		ReleaseMutex(la_mutex);
		
		fclose(file);
		return;
	}
	
	fread(asset->buffer, 1, asset->size, file);
	fclose(file);


	// Mark everything as done
	WaitForSingleObject(la_mutex, INFINITE);
	
	if(asset->state == LA_ASSET_STATE_DELETE)
	{
		la_threadCount --;
		ReleaseMutex(la_mutex);
		
		la_assetRemove(asset->file);
		return;
	}
	
	add_buffer(asset->file, asset->buffer, asset->size);
	asset->state = LA_ASSET_STATE_LOADED; // We are done, hooray!
	la_threadCount --;
	
	ReleaseMutex(la_mutex);
}

DWORD la_worker(void *unused)
{
	while(1)
	{
		WaitForSingleObject(la_mutex, INFINITE);
        
        if(la_paused == TRUE)
        {
            ReleaseMutex(la_mutex);
            return 1;
        }
        
		la_asset *asset = la_firstAsset;
		while(asset)
		{
			if(la_threadCount >= LA_MAX_THREADS)
			break;
			
			if(asset->state == LA_ASSET_STATE_NOT_LOADED)
			{
				la_threadCount ++;
				asset->state = LA_ASSET_STATE_LOADING;
				
				CreateThread(NULL, 0, la_loadAsync, asset, 0, NULL);
			}
			
			asset = asset->next;
		}
		
		ReleaseMutex(la_mutex);
	}
}

int la_assetRemoveAll()
{
	WaitForSingleObject(la_mutex, INFINITE);
	
	la_asset *nextAsset = NULL;
	la_asset *asset = la_firstAsset;
	
	// Sanity check
	while(asset)
	{
		if(asset->state <= LA_ASSET_STATE_LOADING || asset->state == LA_ASSET_STATE_DELETE)
		{
			ReleaseMutex(la_mutex);
			return 0;
		}
	}
	
	// Remove everything
	asset = la_firstAsset;
	while(asset)
	{
		nextAsset = asset->next;
		
		if(asset->buffer)
		  free(asset->buffer);
		
		free(asset->file);
		free(asset->filepath);
		free(asset);
		
		asset = nextAsset;
	}
	
	la_firstAsset = NULL;
	ReleaseMutex(la_mutex);
	
	return 1;
}

void la_clear()
{
	while(la_assetRemoveAll() == 0)
	{
		wait(1);
	}
}

void la_assetRemove(char *file)
{
    WaitForSingleObject(la_mutex, INFINITE);

	la_asset *prevAsset = NULL;
	la_asset *asset = la_firstAsset;
	while(asset)
	{
		if(strcmp(file, asset->file) == 0)
		{
			if(asset->state <= LA_ASSET_STATE_LOADING)
			{
				asset->state = LA_ASSET_STATE_DELETE; // The asset is in use by the loader, so it has to remove the asset later!
				ReleaseMutex(la_mutex);
				return;
			}
			
			if(prevAsset)
			{
				prevAsset->next = asset->next;
			}
			else
                la_firstAsset = asset->next;
			
			if(asset->buffer)
                free(asset->buffer);
			
			free(asset->file);
			free(asset->filepath);
			free(asset);
		}
		
		prevAsset = asset;
		asset = asset->next;
	}
	
	ReleaseMutex(la_mutex);
}

// _NOT_ Threadsafe!
la_asset *la_assetForFile(char *file, char *filepath)
{
    if(!file)
        return NULL;

	la_asset *asset = la_firstAsset;
	while(asset)
	{
		if(strcmp(file, asset->file) == 0)
		{
			return asset;
		}
		
		asset = asset->next;
	}
	
	if(!filepath)
        return NULL;
	
	asset = (la_asset *)malloc(sizeof(la_asset));
	if(asset)
	{
		asset->file = (char *)malloc((strlen(file) + 1) * sizeof(char));
		asset->filepath = (char *)malloc((strlen(filepath) + 1) * sizeof(char));
		
		if(asset->file == NULL || asset->filepath == NULL)
		{
			if(asset->file)
			 free(asset->file);
			
			if(asset->filepath)
			 free(asset->filepath);
			
			free(asset);
			return NULL;
		}
		
		strcpy(asset->file, file);
		strcpy(asset->filepath, filepath);
		
		asset->state 	= 0;
		asset->buffer 	= NULL;
		asset->next 	= NULL;
		
		asset->next   = la_firstAsset;
		la_firstAsset = asset;
	}
	
	return asset;
}

la_asset *la_checkAsset(la_asset *asset)
{
	la_asset *_asset = la_firstAsset;
	while(_asset)
	{
		if(_asset == asset)
		{
			return _asset;
		}
		
		_asset = _asset->next;
	}
	
	
	return NULL;
}

int la_checkFile(STRING *file)
{
	STRING	*filepath;
	la_asset *asset;

	filepath = str_create(work_dir);
	str_cat(filepath, "\\");
	str_cat(filepath, file);
	
	la_lock();
	
	asset = la_assetForFile(_chr(file), _chr(filepath));
	switch(asset->state)
    {
        case LA_ASSET_STATE_LOADED:
        {
            la_unlock();
            ptr_remove(filepath);
            
            return 1;
        }
            break;
        
        case LA_ASSET_STATE_NOSUCHFILE:
        {
            printf("No such file or directory \"%s\"", asset->file);
            
            la_unlock();
            ptr_remove(filepath);
            
            return -1;
        }
            break;
        
        case LA_ASSET_STATE_READFAIL:
        {
            printf("Failed to load \"%s\"", asset->file);
            
            la_unlock();
            ptr_remove(filepath);
            
            return -2;
        }
            break;
        
        default:
            break;
    }
	
	la_unlock();
	ptr_remove(filepath);
	
	return 0;
}

void la_lock()
{
	WaitForSingleObject(la_mutex, INFINITE);
}

void la_unlock()
{
	ReleaseMutex(la_mutex);
}
