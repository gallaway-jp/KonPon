#include "MemoryMapLib.h"

#include <QFile>
MemoryMapLib::MemoryMapLib()
{
	Q_INIT_RESOURCE(DictRes);
}

// Opens a file with the Qt API
// Returns an int-based handle to the opened file on success
// Returns -1 on failure
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

// Returns size of opened file
size_t MemoryMapLib::size(int handle)
{
	if (handle < fileHandles.size() && handle >= 0
		&& fileHandles[handle].first != nullptr) {
		QFile* file = static_cast<QFile*>(fileHandles[handle].first);
		return static_cast<size_t>(file->size());
	}
	return 0;
}

// Closes opened file
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

// Maps opened file to memory
// Returns address to beginning of map on success
// Returns nullptr on failure
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

// Unmaps memory mapped file
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

MemoryMapLib::~MemoryMapLib()
{
	for (int i = 0; i < fileHandles.size(); i++) {
		close(i);
	}
}