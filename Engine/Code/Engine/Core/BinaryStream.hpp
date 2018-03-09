#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "ErrorWarningAssert.hpp"

#pragma warning(disable:4996)
static constexpr uint32 ENDIAN_CHECK = 0x01020304;
bool constexpr IsBigEndian()
{
	return ((byte_t*)(&ENDIAN_CHECK))[0] == 0x01;
}
enum EEndianness
{
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};

//
const EEndianness  GetHostOrder();
void FlipBytes(void *buffer, uint const size);

/*************************************************************************************
/* BINARY FORMAT ==
/*************************************************************************************
Motivation: So loading an FBXSDK file takes a long time, plus it usually requires some post process
cleanup to get game ready.  If each model and animation takes 1-10s to load, a game of even 10 of
these now has a minute load time, not even accounting for audio or images.

So, after all our importing, we want to save it out in a game ready format.  We could do this as
text, but text has some obvious drawbacks...

- Takes up a lot of room
- Slower to parse than binary
- Easier for people to mess with [sometimes desired]
- ... anthing else?

C++ has some iostream classes, but by default, it is meant for text writing (my unfamiliarity here
my be limiting.  I see iostreams have "write", which does what I want, but the default "<<" &&
">>" operators tend to always convert to text.
/**/

// Stream class - not meant for random access, but for writing & reading in order.  
// If you need random access, recommend using something more akin to a BLOB [Binary Large OBject]
// which is just a large chunk of binary data 
//*/
// in case anyone has it undfined;.


// My stream [can be broken into InputStream & OutputStream variants if you prefer]
// Anything can be a stream [as we'll see in networking [SD6], we'll create a 
// NetworkBinaryStream for TCP/IP connections/pulling resources offline/from other users]

// Our basic interface looks like this - note, given either a singular or multiple version
// of writing/reading - you can implement the other.  



class BinaryStream
{
public:
	// read/write a single byte.  Returns success.
	virtual bool read_byte(byte_t *out) = 0;      // = 0 -> Means this is a pure virtual
	virtual bool write_byte(byte_t const &value) = 0;

	// read/write 'count' bytes.  Returns number of bytes actually read. 
	// will return 0 on failure. 
	virtual uint read_bytes(void *out_buffer, uint const count) = 0;
	virtual uint write_bytes(const void* buffer, uint const size) = 0;

	template <typename T>
	bool write(T const &v)
	{
		if (should_flip()) 
		{
			T copy = v; // again, advanced types will cause issues - as
						// this calls the copy constructor.
			FlipBytes(&copy, sizeof(copy));
			return (write_bytes(&copy, sizeof(v)) == sizeof(v));
		}
			
		return (write_bytes(&v, sizeof(v)) == sizeof(v));
	}
		
	template <typename T>
	bool read(T& v)
	{
		if (should_flip())
		{
			bool success = (read_bytes(&v, sizeof(v)) == sizeof(v));
			FlipBytes(&v, sizeof(v));
			return success;
				
		}
		return (read_bytes(&v, sizeof(v)) == sizeof(v));
	}

	inline bool should_flip() const { return stream_order != GetHostOrder(); }


public:
	EEndianness stream_order = LITTLE_ENDIAN;
};

//////////////////////////////////////////////////////////////////////////
class Vector2;
class Vector3;
class Vector4;
class UIntVector4;
class Pose;
struct DrawInstruction;
struct SQT;
struct Joint;
class Quaternion;
class Matrix4;
//////////////////////////////////////////////////////////////////////////
template <>
bool BinaryStream::write(const std::string& str);
// template read left as exercise to the reader.
template <>
bool BinaryStream::write(const Vector2& v);
template <>
bool BinaryStream::write(const Vector3& v);
template <>
bool BinaryStream::write(const Vector4& v);
template <>
bool BinaryStream::write(const UIntVector4& v);
template <>
bool BinaryStream::write(const DrawInstruction& v);
template <>
bool BinaryStream::write(const Pose& v);
template <>
bool BinaryStream::write(const SQT& v);
template <>
bool BinaryStream::write(const Joint& v);
template <>
bool BinaryStream::write(const Quaternion& v);
template <>
bool BinaryStream::write(const Matrix4& v);


template <>
bool BinaryStream::read(std::string& str);
template <>		   
bool BinaryStream::read(Vector2& v);
template <>		   
bool BinaryStream::read(Vector3& v);
template <>		   
bool BinaryStream::read(Vector4& v);
template <>		   
bool BinaryStream::read(UIntVector4& v);
template <>		   
bool BinaryStream::read(DrawInstruction& v);
template <>		   
bool BinaryStream::read(Pose& v);
template <>		   
bool BinaryStream::read(SQT& v);
template <>		   
bool BinaryStream::read(Joint& v);
template <>		   
bool BinaryStream::read(Quaternion& v);
template <>		   
bool BinaryStream::read(Matrix4& v);

// So we're going to be dealing directly with files in this class, so we'll
// also implement a file version. 
// I am most familiar with C style file commands, so that is what
// I'll use in my demo, but any implementation is fine
// as long as the reuslt is binary! 
class FileBinaryStream : public BinaryStream
{
public:
	//----------------------------------------------------------------------------
	FileBinaryStream()
		: file_pointer(nullptr)
	{}

	//----------------------------------------------------------------------------
	~FileBinaryStream()
	{
		close();
	}

	//----------------------------------------------------------------------------
	bool open_for_read(char const *filename);

	//----------------------------------------------------------------------------
	bool open_for_write(char const *filename);

	//----------------------------------------------------------------------------
	void close();


	//----------------------------------------------------------------------------
	// BinaryStream Impl
	//----------------------------------------------------------------------------
	virtual bool read_byte(byte_t *out) override;

	//----------------------------------------------------------------------------
	virtual bool write_byte(byte_t const &value) override;

	//----------------------------------------------------------------------------
	// read/write 'count' bytes.  Returns number of bytes actually read. 
	// will return 0 on failure. 
	virtual uint read_bytes(void *out_buffer, uint const count);

	//----------------------------------------------------------------------------
	virtual uint write_bytes(void const* buffer, uint const size);

	//----------------------------------------------------------------------------
	inline bool is_open() const { return nullptr != file_pointer; }

public:
	FILE *file_pointer;   // file pointer;

};
//*/
/*************************************************************************************
/* == TEMPLATES ==
/*************************************************************************************
Motivation: Just add some convenience.  Always writing stream.write_bytes( &thing, sizeof(thing) );
is annoying.  So, this is a nice easy time to introduce a simple template;  Templates can be
thought of as type safe macros.  Or Meta-Programming.  Templates are resolved and written at
compile time, based on use.
/**/
/*/
// So, now when I do...
void Foo(BinaryStream &stream)
{
int a = 10;


// This causes the compiler to know it has need for a write<int> invocation.
stream.write(a); // effectively becomes: stream.write( &a, sizeof(a) );

// DANGER!
// The below will compile - but will not have the desired behaviour;
std::string str = "hello world";
stream.write(str);
// this becomes stream.write( &str, sizeof(str) );
// which will write the binary data representing the string object [a shallow copy]
// and not the contents of the string itself - will likely crash when reading!
}




/*************************************************************************************
/* USING THIS ==
/*************************************************************************************
So for this class - you must save out Meshes/Motions/Skeletons as your own binary format,
and be able to read from this format.

There is not much to say about this task - you have total freedom to do this as you want,
the only limit is there must be three files;

- *.mesh:   .mesh files should store your mesh and everything you need to draw it.  I suggest
adding a MeshBuilder::write( BinaryStream &stream ) and a
MeshBuilder::read( BinaryStream &stream ).
This should track;
* Vertices
* [Optional] Indices
* Draw Instructions
* [Optiona] Material IDs

- *.skel:  These files will store the bine pose skeleton.  Should just be an ID and an array
of global transforms.

- *.motion:  A single motion.  At minimum should store off framerate, duration, frame count, and
the array of poses.  Each pose is just an array of local transforms.

To test - import from FBX, and then create console commands for saving each piece to
it's own format.

Also create console commands to load each one seperately (load_mesh, load_skeleton, load_motion).
Loading from your own format should be considerably faster.

* NEXT STEPS *
Once you have all commands working seperately, I would suggest making a single function...
/**/
/*/
// Imports an FBX file - and saves the results to output files.
// return number of files saved.
// Suggest also printing to log/console what files were created...
uint ConvertFBXFileToEngineFiles(char const *fbx_filename, // fbx file to iport
char const *root_output_name,       // root name for possible outputted files
float import_scale = 1.0f,          // scale to import as.
uint import_flags = IMPORT_ALL);   // flags for what to import, SKELETON/MESH/MOTION/etc...

// An example would be like;
void Foo()
{
ConvertFBXFileToEngineFiles("model/drotek.fbx", "drotek", 0.01f);
// Output:
// skeleton saved as: drotek.skel
// mesh saved as: drotek.mesh
// motion saved as: drotek.motion
// Total: 3 File(s) Created.

ConvertFBXFileToEngineFiles("model/unitychan/animations/unitychan_JUMP00.fbx",
"uchan@jump",
0.01f,
IMPORT_MOTION);
// Output;
// motion saved as: uchan@jump.motion
// Total: 1 File(s) Created.
}
//*/
// potentially, all import options can be put into a 
// struct fbx_import_options_t;  // Would make adding future work easier - but nothing more
// will be required for this class;


/**/


/***
== ENDIANESS ==
*EXTRA - Up to you if you want to implement yet.  Not actually needed till SD6*
Motivation: So when storing binary, it is more compact, but it is also making an assumption
that the data is already in a format our machine can read.  This is called "host order",
or the endian order of our machine - which for our machines is Little Endian.

Consoles though are Big Endian [as our most phones], so if we tried to read our
data on them, it would break.  So, we want our stream to be endian aware.  That is,
we're going to define an endianness we expect the data to be in, and potentially flip
endianess when our host endian order doesn't match the file format.
/**/
/*/
// I usually prefer little endian for my file format, as the compression I more intuitively
// think in little endian.
// So first, how do I tell the endianness of my machine?
static constexpr uint32_t ENDIAN_CHECK = 0x01020304;
bool constexpr IsBigEndian()
{
return ((byte_t*)(&ENDIAN_CHECK))[0] == 0x01;
}
// how would you check for LITLE_ENDIAN?

enum eEndianness
{
LITTLE_ENDIAN,
BIG_ENDIAN,
};

//
eEndianness constexpr GetHostOrder();

// Now, we can update our BinaryStream class and define and implement some additional methods;
class BinaryStream
{
public:
template <typename T>
bool write(T const &v)
{
if (should_flip()) {
T copy = v; // again, advanced types will cause issues - as
// this calls the copy constructor.
FlipBytes(&copy, sizeof(copy));
write_bytes(&copy, sizeof(T));
}
else {
write_bytes(&v, sizeof(T));
}
}

// read update left as an exercize.

inline bool should_flip() const { return stream_order() != GetHostOrder(); }


public:
eEndianness stream_order;
};

// So this needs on last piece to work - FlipBytes
// This flips the order of the bytes in buffer,
// so if buffer had [0x01, 0x02, 0x03, 0x04] in it,
// after calling it would have [0x04, 0x03, 0x02, 0x01]
// DO NOT USE STL - this is a common interview question!
void FlipBytes(void *buffer, uint const size)
{
// excercise to reader
}
//*/
/**/

/***
== MAKING THINGS SMALLER ==

So, if you have everything working, the next step is trying to compact the save size.
Some tips;  We'll be playing more with this in SD5 once we have a means to measure
performance gains.

- Quaternions only need to have 3 channels saved, as the last can be deduced from those three.
- You can potentially compress the floats in a quaternion farther (8/16 bits per channel).
This may result in some loss of fidelity, at a savings of 75/50% space.
- If you compute the bounds of your mesh before saving, you can potentially save space on vertices
by saving floats as an offset from the bottom left corner (if you max range is 20 meters, you can
store each x,y,z using a 16bit float. )
- Most motions if stored using local poses do not actually change much frame to frame - could run
length encode each transform in a pose across the entire motion.
- Full file compression using gzip after the stream is written.  Require making a MemoryStream.

/**/