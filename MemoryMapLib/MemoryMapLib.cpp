#include "MemoryMapLib.h"

#include <QFile>

/*!
	\fn MemoryMapLib::MemoryMapLib()

	Constructs an MemoryMapLib object and initializes the DictRes resource.
*/
MemoryMapLib::MemoryMapLib()
{
	Q_INIT_RESOURCE(DictRes);
}

/*!
	\fn int MemoryMapLib::open(const char* filePath)

	Opens a file from \a filePath with the Qt API.
	Returns an int-based handle to the opened file on success.
	Returns -1 on failure.
*/
int MemoryMapLib::open(const char* filePath)
{
	QFile* file = new QFile(filePath);
	if (!file->exists() || !file->open(QIODevice::ReadOnly)) {
		delete file;
		return -1;
	}

	for (int i = 0; i < fileHandles.size(); i++) {
		if (fileHandles[i].first == nullptr) {
			fileHandles[i] = { static_cast<void*>(file), nullptr };
			return i;
		}
	}

	fileHandles.push_back({ static_cast<void*>(file), nullptr });
	return fileHandles.size() - 1;
}

/*!
	\fn size_t MemoryMapLib::size(int handle)

	Returns size of opened file by passing its \a handle.
*/
size_t MemoryMapLib::size(int handle)
{
	if (handle < fileHandles.size() && handle >= 0
		&& fileHandles[handle].first != nullptr) {
		QFile* file = static_cast<QFile*>(fileHandles[handle].first);
		return static_cast<size_t>(file->size());
	}
	return 0;
}

/*!
	\fn void MemoryMapLib::size(int handle)

	Closes opened file by passing its \a handle.
*/
void MemoryMapLib::close(int handle)
{
	if (handle < fileHandles.size() && handle >= 0
		&& fileHandles[handle].first != nullptr) {
		QFile* file = static_cast<QFile*>(fileHandles[handle].first);
		file->close();
		delete file;
		fileHandles[handle].first = nullptr;
		fileHandles[handle].second = nullptr;
	}
}

/*!
	\fn char* MemoryMapLib::map(int handle)

	Maps opened file to memory by passing its \a handle.
	Returns address to beginning of map on success.
	Returns nullptr on failure.
*/
char* MemoryMapLib::map(int handle)
{
	if (handle < fileHandles.size() && handle >= 0
		&& fileHandles[handle].first != nullptr) {
		QFile* file = static_cast<QFile*>(fileHandles[handle].first);
		if (fileHandles[handle].second != nullptr) {
			return reinterpret_cast<char*>(fileHandles[handle].second);
		}

		uchar* umap = file->map(0, file->size());
		fileHandles[handle].second = umap;
		return reinterpret_cast<char*>(umap);
	}
	return nullptr;
}

/*!
	\fn void MemoryMapLib::unmap(int handle)

	Unmaps memory mapped file by passing its \a handle.
*/
void MemoryMapLib::unmap(int handle)
{
	if (handle < fileHandles.size() && handle >= 0
		&& fileHandles[handle].first != nullptr) {
		QFile* file = static_cast<QFile*>(fileHandles[handle].first);
		if (fileHandles[handle].second == nullptr) {
			return;
		}
		file->unmap(fileHandles[handle].second);
		fileHandles[handle].second = nullptr;
	}
}

/*!
	\fn MemoryMapLib::~MemoryMapLib()

	Closes any opened files and destroys MemoryMapLib object.
*/
MemoryMapLib::~MemoryMapLib()
{
	for (int i = 0; i < fileHandles.size(); i++) {
		close(i);
	}
}