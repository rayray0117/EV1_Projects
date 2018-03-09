#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/DX11.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/RHI/Texture2D.hpp"

RHIOutput::RHIOutput(RHIDevice* owner, Window* wnd, /*[OPTIONAL]*/ RHIEnum::OutputMode mode /*= RENDEROUTPUT_WINDOWED*/)
	: mo_device(owner)
	, m_window(wnd)
	, swap_chain(nullptr)
{
	createSwapChain();
	createRenderTarget();

	UNUSED(mode);
}

RHIOutput::RHIOutput(RHIDevice* owner, IDXGISwapChain *sc, Window* wnd, /*[OPTIONAL]*/ RHIEnum::OutputMode mode /*= RENDEROUTPUT_WINDOWED*/)
	:m_window(wnd)
	, swap_chain(sc)
{
	mo_device = owner;
	createRenderTarget();

	UNUSED(mode);
}

RHIOutput::~RHIOutput()
{
	close();
}

void RHIOutput::present()
{
	// We're done rendering, so tell the swap chain they can copy the back buffer to the front (desktop/window) buffer #output
	swap_chain->Present(1, // Sync Interval, set to 1 for VSync
		0);                    // Present flags, see;
							   // https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx
}

void RHIOutput::close()
{
	
	SAFE_DELETE(render_target);
	DX_SAFE_RELEASE(swap_chain);

	if (nullptr != m_window) {
		m_window->close();
		SAFE_DELETE(m_window);
	}
}

Texture2D* RHIOutput::getRenderTarget()
{
	return render_target;
}

void RHIOutput::createSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swap_desc;
	memset(&swap_desc, 0, sizeof(swap_desc));

	// fill the swap chain description struct
	swap_desc.BufferCount = 2;                                    // two buffers (one front, one back?)

	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;
	swap_desc.OutputWindow = m_window->hwnd;                                // the window to be copied to on present
	swap_desc.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)

																  // Default options.
	swap_desc.Windowed = TRUE;                                    // windowed/full-screen mode
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	swap_desc.BufferDesc.Width = m_window->windowWidth;
	swap_desc.BufferDesc.Height = m_window->windowHeight;

	IDXGIDevice1* pDXGIDevice;
	mo_device->dx_device->QueryInterface(__uuidof(IDXGIDevice1), (void **)&pDXGIDevice);

	IDXGIAdapter * adapter;
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&adapter);

	IDXGIFactory1* factory;
	adapter->GetParent(__uuidof(IDXGIFactory1), (void **)&factory);

	// Actually Create
	HRESULT hr = factory->CreateSwapChain( //#ToDo: Look into if CreateSwapChainForHwnd is beneficial to me
		mo_device->dx_device, 
		&swap_desc,     // Description of our swap chain
		&swap_chain); // Swap Chain we're creating


	UNUSED(hr); //#ToDo: safety check?
}

bool RHIOutput::createRenderTarget()
{
	ID3D11Texture2D *back_buffer = nullptr;
	swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
	render_target = new Texture2D(mo_device, back_buffer);
	DX_SAFE_RELEASE(back_buffer);

	return true;
}
