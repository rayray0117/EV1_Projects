#pragma once
#include "Engine/RHI/RHITypes.hpp"
//////////////////////////////////////////////////////////////////////////
class Window;
class RHIDevice;
class Texture2D;
struct IDXGISwapChain;
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------
// RHIOutput.*
// File creates a renderable surface for a Window.  It is what
// links out GPU to something the user can see.
//------------------------------------------------------------------------
class RHIOutput
{
public:
	RHIOutput(RHIDevice* owner, IDXGISwapChain *sc, Window* wnd, /*[OPTIONAL]*/ RHIEnum::OutputMode mode = RHIEnum::RENDEROUTPUT_WINDOWED);
	RHIOutput(RHIDevice* owner, Window* wnd, /*[OPTIONAL]*/ RHIEnum::OutputMode mode = RHIEnum::RENDEROUTPUT_WINDOWED);
	~RHIOutput();

	// Whatever is currently being renderered
	// that is what we're working with.
	void present();
	void close();

	Texture2D* getRenderTarget();
	float get_width() const;
	float get_height() const;
	float get_aspect_ratio() const;

	// [OPTIONAL]
	bool set_display_mode(RHIEnum::OutputMode mode,
		float const width = 0.0f,
		float const height = 0.0f);

public:
	RHIDevice* mo_device;
	Window* m_window;

	IDXGISwapChain* swap_chain;

	Texture2D* render_target;

private:
	void createSwapChain();
	bool createRenderTarget();

};