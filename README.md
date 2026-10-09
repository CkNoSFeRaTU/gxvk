# GXVK

A Vulkan-based translation layer for 3Dfx Glide which allows running 3D applications on Linux using Wine.
It is a completely new frontend for DXVK.

## FAQ

### What the status of the project?

Project is on very early prototype stage of development. So don't expect it to run any games flawlessly. Even if it will run something it will be very slow and all kind of bugs will lay in ambush at every corner.

### What Glide API variants will be supported?

Plan is to eventually support all three 32-bit variants of API released by 3Dfx for Windows. Namely ones utilizing glide.dll, glide2x.dll and glide3x.dll. Native Linux variants aren't planned as despite Glide had been made available in later 3Dfx days for Linux there haven't been any games released utilizing Glide there. Various DOS-compatibility OVL and 64-bit variants are also not planned.

### When it will be ready for prime time?

Currently it is just off time one person project. And my free time is pretty much occupied with other things. Also unlike [D7VK](https://github.com/WinterSnowfall/d7vk) it doesn't rely on already matured D3D9 frontend which shoulder most of the grizzly work. That is because API is sufficiently different from Direct3D to warrant it's own separate frontend to translate directly and not through some API intermediary. But at the same time due to new frontend many things need to be re-designed to better fit Glide or fully re-implemented despite Glide itself as an API is arguably less complex than legacy Direct3D and much more thoroughly documented.

TLDR. Don't expect a rapid progress like it was the case with [D7VK](https://github.com/WinterSnowfall/d7vk) in the past. Snail crawling pace is much more realistic.

### Will it work on Windows?

I'm not using Windows, so can't test it or develop it to be adapted to such situations. It's primarily intended use case is, and always will be, Wine/Linux. To that end, GXVK is pretty much aligned with upstream [DXVK](https://github.com/doitsujin/dxvk) and it's spin-off [D7VK](https://github.com/WinterSnowfall/d7vk). It could theoretically work but it is nor tested, nor supported.

### Will it be upstreamed to DXVK at some point?

No. DXVK's development team have made it clear they are not interested in merging and/or maintaining any kinds of new APIs which aren't already there.

### Will DXVK's D3D9-D3D11 config options, such as frame rate limits, work with GXVK?

No. As GXVK is completely new DXVK frontend and not relying on already existing one of the consequences of that is that very limited set of dxvk-prefixed options will work and none from Direct3D frontend.
GXVK will have it's own set of options some of which will be similar to options other upstream frontends provide, including frame rate limiter.

## How to use

Grab the latest release. Alternatively you can compile the project manually or grab latest build artifact from [Actions](https://github.com/CkNoSFeRaTU/gxvk/actions) if you want to be "on the bleeding edge".

> [!WARNING]
> Please keep in mind that ABSOLUTELY NO TESTING is done on Windows. GXVK is developed on and primarily aimed at use with Wine/Linux, so your mileage may vary in other situations.

Copy matching glide.dll, glide2x.dll or glide3x.dll which this particular game/application uses (or all of them) to it's directory next to the executable.

Alternatively you can place all DLLs to Wine's prefix system path directly once and they'll be used for all games/applications in that prefix automatically.
For this method for WOW64 Wine prefix (default in recent Wine) it should go to `syswow64` directory and for a pure 32-bit Wine prefix (non default) should instead go to the `system32` directory.

As Wine doesn't implement Glide at all itself you don't need any DLL overrides.

> [!TIP]
> If you have other wrappers installed you can verify that your application uses GXVK by enabling the HUD (see notes below).

In order to remove GXVK just remove previously copied DLLs.

#### DLL dependencies

Listed below are the DLL requirements for using GXVK with any single API.

- Glide v2.00 - v2.11: `glide.dll`
- Glide v2.20 - v2.60: `glide2x.dll`
- Glide v3.00 - v3.10: `glide3x.dll`

### HUD

The `DXVK_HUD` environment variable controls a HUD which can display the framerate and some stat counters. It accepts a comma-separated list of the following options:
- `devinfo`: Displays the name of the GPU and the driver version.
- `fps`: Shows the current frame rate.
- `frametimes`: Shows a frame time graph.
- `submissions`: Shows the number of command buffers submitted per frame.
- `drawcalls`: Shows the number of draw calls and render passes per frame.
- `pipelines`: Shows the total number of graphics and compute pipelines.
- `descriptors`: Shows the number of descriptor pools and descriptor sets.
- `memory`: Shows the amount of device memory allocated and used.
- `allocations`: Shows detailed memory chunk suballocation info.
- `gpuload`: Shows estimated GPU load. May be inaccurate.
- `version`: Shows DXVK version.
- `api`: Shows the D3D feature level used by the application.
- `cs`: Shows worker thread statistics.
- `compiler`: Shows shader compiler activity
- `scale=x`: Scales the HUD by a factor of `x` (e.g. `1.5`)
- `opacity=y`: Adjusts the HUD opacity by a factor of `y` (e.g. `0.5`, `1.0` being fully opaque).

Additionally, `DXVK_HUD=1` has the same effect as `DXVK_HUD=devinfo,fps`, and `DXVK_HUD=full` enables all available HUD elements.

By default GXVK scale HUD automatically and you can toggle it's visibility (if HUD was enabled) with SHIFT + F12 and change opacity with SHIFT + F10 / SHIFT + F11 in real time.

### Logs

When used with Wine, GXVK will print log messages to `stderr`. Additionally, standalone log files can optionally be generated by setting the `GXVK_LOG_PATH` variable, where log files in the given directory will be called `app_glide1x.log`, `app_glide2x.log` or `app_glide3x.log`, where `app` is the name of the game executable. The `GXVK_LOG_LEVEL` variable can be used to control logging verbosity.

The naming of the environment variables has been altered in order to allow for finer control of logging specifically for GXVK, independently of upstream DXVK.

On Windows, log files will be created in the game's working directory by default, which is usually next to the game executable.

## Build instructions

In order to pull in all submodules that are needed for building, clone the repository using the following command:
```
git clone --recursive https://github.com/CkNoSFeRaTU/gxvk.git
```

## Acknowledgments

None of this would have ever been possible without DXVK and Wine, so remember to show your love to the awesome people involved in those projects.
Special thanks to 3Dfx for pioneering hardware-accelerated 3D graphics to mainstream audience, eventually open-sourced Glide API and excellent accompanied documentation.
