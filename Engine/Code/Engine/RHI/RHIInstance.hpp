#pragma once
#include "Engine/RHI/RHITypes.hpp"
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
class RHIDeviceContext;
class RHIDevice;
class RHIOutput;
//////////////////////////////////////////////////////////////////////////
class RHIInstance
{
public:
	// Constructor is privete:  Singleton Class
	~RHIInstance();

	//bool create_output(RHIDeviceContext*& out_context, RHIOutput*& out_output, unsigned int const px_width, unsigned int const px_height, /*OPTIONAL*/ eRHIOutputMode const initial_mode = RENDEROUTPUT_WINDOWED);

	// HELPERS
	// RHIDeviceContext* create_display_context( render_context_options_t const &options ) { return nullptr; }
	bool create_output(RHIDevice*& out_device, RHIDeviceContext*& out_context, RHIOutput*& out_output, uint const px_width, uint const px_height);

private:
	RHIInstance();

	static RHIInstance* gInstance;

public:
	static RHIInstance* GetInstance();
};
