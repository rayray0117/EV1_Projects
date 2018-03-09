#pragma once
#include "Engine/Core/EngineBase.hpp"
#define MAX_FRAMES_PER_CALLSTACK (128)
//////////////////////////////////////////////////////////////////////////
struct callstack_line_t
{
	char filename[128];
	char function_name[128];
	uint line;
	uint offset;
};
//////////////////////////////////////////////////////////////////////////
class Callstack
{
public:
	Callstack();

	uint hash;
	uint frame_count;
	void* frames[MAX_FRAMES_PER_CALLSTACK];
};


bool CallstackSystemInit();
void CallstackSystemDeinit();

// Feel free to reorg this in a way you like - this is very C style.  
// Really, we just want to make sure these callstacks are not allocated on the heap.
// - You could creat them in-place in the meta-data if you prefer (provide memory to fill)
// - You could overload new on the Callstack class, cause "new Callstack(skip_frames)" to call that, a
//   and keeping this functionality.

// As this is - this will create a callstack using malloc (untracked allocation), skipping the first few frames.
Callstack* CreateCallstack(uint skip_frames);
void DestroyCallstack(Callstack *c);

uint CallstackGetLines(callstack_line_t *line_buffer, uint const max_lines, Callstack *cs);

void MakeCallstackReport(Callstack *cs);