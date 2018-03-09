#pragma once
#include "Engine/RHI/RHIInstance.hpp"
#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

#include "Engine/Core/Window.hpp"
//------------------------------------------------------------------------
// RHI.h
// This file should be the only one the game needs to 
// include - so should include a reference to each other file. 
//------------------------------------------------------------------------


// Creates the RHIInstance Singleton
void RHIStartup();

// Destroys the RHIInstance Singleton
void RHIShutdown();