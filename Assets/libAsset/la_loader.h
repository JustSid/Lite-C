#ifndef LA_LOADER_H_
#define LA_LOADER_H_

// The number of threads the loader should spawn max. More threads = Greater memory footprint and probably even performance decreases
// The loader will spawn one thread per asset, thus, if you load 100 assets, the loader would spawn 100 threads. Because this is nothing one usually want,
// the loader respects this constant. Your 100 assets would be loaded using only five threads at the same time.
#define LA_MAX_THREADS 5

// The asset is not yet loaded
#define LA_ASSET_STATE_NOT_LOADED 	0
// The asset is currently being loaded in the background
#define LA_ASSET_STATE_LOADING		1
// The asset is succesfully loaded and can be used
#define LA_ASSET_STATE_LOADED			2
// The asset couldn't be loaded because the file was not found
#define LA_ASSET_STATE_NOSUCHFILE	3
// The asset couldn't be loaded because reading the file failed
#define LA_ASSET_STATE_READFAIL		4
// The asset was deleted but is currently loaded in the background, the loader thread will delete the asset later on.
#define LA_ASSET_STATE_DELETE			5

typedef struct la_asset
{
	// The assets state
	int state;
	
	// The file contents or NULL if no loading has happend yet
	void *buffer;
	// The number of bytes allocated by buffer
	long size;
	
	// The file and filepath
	char *file, *filepath; 

	// The next element in the linked list
	struct la_asset *next;	
} la_asset;


// Initializes the asset loader
// Returns 1 on success, otherwise 0.
int la_assetInit();

// Pauses the loader. This is useful in cases were you don't load any assets and don't want an extra idle thread draining your performance.
// REMARK: Unpausing is a expensive operation, so don't turn the loader on and off just because you know that there are only a few frames without loading assets!
// REMARK: While the loader is paused, no assets will be loaded!
int la_pause();
// Unpauses the loader.
// Returns 1 on success, otherwise 0.
int la_unpause();

// Returns the asset associated with the given file, if no such asset exists yet, the asset will be created and scheduled for loading
// If the filepath is NULL, la_assetForFile() will return NULL if no asset with the given name was found.
// REMARK: NOT!!! Thread safe. Call la_lock() before calling this function!
la_asset *la_assetForFile(char *file, char *filepath);
// Checks if the asset is still valid. Must be called when the loader is locked to check if the asset wasn't purged from the list yet.
// The function returns the asset if its still valid, otherwise NULL.
// REMARK: If you retrieved the asset from la_assetForFile() and haven't unlocked and locked the loader yet, you don't need to call this function.
la_asset *la_checkAsset(la_asset *asset);
// Checks wether the given asset is already loaded, or not. Returns 0 when the asset is currently being loaded in the background, 1 if the asset is loaded.
// Error codes: -1 = File not found, -2 = Failure while reading the object
int la_checkFile(STRING *file);
// Removes the asset associated with the given file. The next time la_assetForFile() is called, the asset will be loaded from scratch again.
void la_assetRemove(char *file);
// Removes all assets from the internal list.
// REMARK: Make sure all assets are already loaded before calling this function!
// REMARK: This function IS thread safe. Calling la_lock() before calling this function will end in an deadlock
// Returned value: 1 if all assets has been removed, 0 if the function couldn't remove all assets because one or more assets weren't loaded yet
int la_assetRemoveAll();
// Removes all assets from the internal list. Unlike la_assetRemoveAll(), this function will wait until all assets are safe to delete.
void la_clear();

// Locks the loader
void la_lock();
// Unlocks the loader
void la_unlock();

#endif
