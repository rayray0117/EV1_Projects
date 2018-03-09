#include "Engine/Core/BinaryStream.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Renderer/Pose.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Renderer/DrawInstruction.hpp"
#include "Engine/Math/Matrix4.hpp"
//////////////////////////////////////////////////////////////////////////
// So the string example had issues - but we can get around this;  We can do a 
// "template specialization", that is, explicitly define one of the methods templates, in this case,
// a string. 
template <>
bool BinaryStream::write(std::string const &str)
{
	// write the string data, and a terminating NULL to signify I'm done
	write_bytes(str.c_str(), str.size());
	return write_byte(NULL);
}

template <>
bool BinaryStream::write(const Vector2& v)
{
	bool worked = true;
	worked &= write(v.x);
	worked &= write(v.y);
	return worked;
}

template <>
bool BinaryStream::write(const Vector3& v)
{
	bool worked = true;
	worked &= write(v.x);
	worked &= write(v.y);
	worked &= write(v.z);
	return worked;
}

template <>
bool BinaryStream::write(const Vector4& v)
{
	bool worked = true;
	worked &= write(v.x);
	worked &= write(v.y);
	worked &= write(v.z);
	worked &= write(v.w);
	return worked;
}

template <>
bool BinaryStream::write(const UIntVector4& v)
{
	bool worked = true;
	worked &= write(v.x);
	worked &= write(v.y);
	worked &= write(v.z);
	worked &= write(v.w);
	return worked;
}

template <>
bool BinaryStream::write(const DrawInstruction& v)
{
	bool worked = true;
	worked &= write(v.start_index);
	worked &= write(v.start_vertex);
	worked &= write(v.vertex_count);
	worked &= write(v.primative);
	worked &= write(v.useIbo);
	return worked;
}

template <>
bool BinaryStream::write(const Pose& v)
{
	bool worked = true;
	write(v.m_transforms.size());
	for each (SQT var in v.m_transforms)
	{
		worked &= write(var);
	}
	return worked;
}

template <>
bool BinaryStream::write(const SQT& v)
{
	bool worked = true;
	worked &= write(v.scale);
	worked &= write(v.rotation);
	worked &= write(v.position);
	return worked;
}

template <>
bool BinaryStream::write(const Joint& v)
{
	bool worked = true;
	worked &= write(v.name);
	worked &= write(v.parentName);
	worked &= write(v.global_transform);
	return worked;
}

template <>
bool BinaryStream::write(const Quaternion& v)
{
	bool worked = true;
	worked &= write(v.x);
	worked &= write(v.y);
	worked &= write(v.z);
	worked &= write(v.w);
	return worked;
}

template <>
bool BinaryStream::write(const Matrix4& v)
{
	bool worked = true;
	for (int i = 0; i < 16; i++)
	{
		worked &= write(v.values[i]);
	}
	return worked;
}

template <>
bool BinaryStream::read(std::string& str)
{
	bool worked = true;
	byte_t b;
	worked &= read_byte(&b);
	while (b != NULL )
	{
		str += b;
		worked &= read_byte(&b);
	}
	return worked;
}

template <>
bool BinaryStream::read(Vector2& v)
{
	bool worked = true;
	worked &= read(v.x);
	worked &= read(v.y);
	return worked;
}

template <>
bool BinaryStream::read(Vector3& v)
{
	bool worked = true;
	worked &= read(v.x);
	worked &= read(v.y);
	worked &= read(v.z);
	return worked;
}

template <>
bool BinaryStream::read(Vector4& v)
{
	bool worked = true;
	worked &= read(v.x);
	worked &= read(v.y);
	worked &= read(v.z);
	worked &= read(v.w);
	return worked;
}

template <>
bool BinaryStream::read(UIntVector4& v)
{
	bool worked = true;
	worked &= read(v.x);
	worked &= read(v.y);
	worked &= read(v.z);
	worked &= read(v.w);
	return worked;
}

template <>
bool BinaryStream::read(DrawInstruction& v)
{
	bool worked = true;
	worked &= read(v.start_index);
	worked &= read(v.start_vertex);
	worked &= read(v.vertex_count);
	worked &= read(v.primative);
	worked &= read(v.useIbo);
	return worked;
}

template <>
bool BinaryStream::read(Pose& v)
{
	bool worked = true;
	uint size;
	read(size);
	for(uint i = 0; i < size; i++)
	{
		SQT var;
		worked &= read(var);
		v.m_transforms.push_back(var);
	}
	return worked;
}

template <>
bool BinaryStream::read(SQT& v)
{
	bool worked = true;
	worked &= read(v.scale);
	worked &= read(v.rotation);
	worked &= read(v.position);
	return worked;
}

template <>
bool BinaryStream::read(Joint& v)
{
	bool worked = true;
	worked &= read(v.name);
	worked &= read(v.parentName);
	worked &= read(v.global_transform);
	return worked;
}

template <>
bool BinaryStream::read(Quaternion& v)
{
	bool worked = true;
	worked &= read(v.x);
	worked &= read(v.y);
	worked &= read(v.z);
	worked &= read(v.w);
	return worked;
}

template <>
bool BinaryStream::read(Matrix4& v)
{
	bool worked = true;
	for (int i = 0; i < 16; i++)
	{
		worked &= read(v.values[i]);
	}
	return worked;
}

//////////////////////////////////////////////////////////////////////////
bool FileBinaryStream::open_for_read(char const *filename)
{
	ASSERT_OR_DIE(!is_open(), "Trying to open something that is already open");
	file_pointer = fopen(filename, "rb"); // open for read & binary
	return is_open();
}

bool FileBinaryStream::open_for_write(char const *filename)
{
	ASSERT_OR_DIE(!is_open(), "Trying to open something that is already open");
	file_pointer = fopen(filename, "wb"); // open for read & binary
	return is_open();
}

void FileBinaryStream::close()
{
	if (is_open()) {
		fclose(file_pointer);
		file_pointer = nullptr;
	}
}


bool FileBinaryStream::read_byte(byte_t *out)
{
	return (read_bytes(out, 1) == 1);
}



bool FileBinaryStream::write_byte(byte_t const &value)
{
	return (write_bytes(&value, 1) == 1);
}




uint FileBinaryStream::read_bytes(void *out_buffer, uint const count)
{
	uint bytes_read = 0;
	if (is_open()) {
		bytes_read = (uint)fread(out_buffer, 1, count, file_pointer);
	}

	return bytes_read;
}




uint FileBinaryStream::write_bytes(void const* buffer, uint const size)
{
	uint bytes_read = 0;
	if (is_open()) {
		bytes_read = (uint)fwrite(buffer, 1, size, file_pointer);
	}

	return bytes_read;
}

const EEndianness GetHostOrder()
{
	if (IsBigEndian())
	{
		return BIG_ENDIAN;
	}

	return LITTLE_ENDIAN;
}

inline void dtSwapByte(unsigned char* a, unsigned char* b)
{
	unsigned char tmp = *a;
	*a = *b;
	*b = tmp;
}

void FlipBytes(void *buffer, uint const size)
{
	if (size <= 1) return;

	unsigned char* x = (unsigned char*)buffer;
	for (uint i = 0; i < (size / 2); ++i)
	{
		dtSwapByte(x + i, x + (size - 1 - i));
	}
}
