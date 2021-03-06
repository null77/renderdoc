Drivers
--------------

Each RenderDoc back-end is called a 'driver' and is under the renderdoc/driver/ folder. If you don't have the SDK or don't care about a particular API you can remove a driver from building - in the Makefile by editing LIBS= in renderdoc/Makefile, in VS by removing the project and the reference on the renderdoc reference. Be aware of dependencies, such as D3D11 expecting DXGI to be loaded too.

Some shared code for handling shader formats used in multiple APIs such as DXBC and SPIR-V are in renderdoc/driver/shaders/ folder.


Windows
--------------

The main [renderdoc.sln](renderdoc.sln) is a VS2010 solution. To build on later VS versions, simply open & upgrade, I've tested building on VS2012, VS2013 and VS2015 without issues. It compiles fine in the [free VS2013 version](http://www.visualstudio.com/en-us/news/vs2013-community-vs.aspx), just select to update the compilers.

There should be no external dependencies, all libraries/headers needed to build are included in the git checkout.

Development configuration is recommended for day-to-day dev. It's debuggable but not too slow. Release configuration is then obviously what you should build for any builds you'll send out to people or if you want to evaluate performance.

### Visual Studio Visualisers ###

You might find these visualisers useful, going under your [Visualizer] section in autoexp.dat:

    rdctype::str { preview([$e.elems,s]) stringview([$e.elems,s]) }

    rdctype::array<*> {
        preview ( #( "[",$e.count,"] {", #array(expr: $e.elems[$i], size: $e.count), "}") )
        children ( #( #([size] : $e.count), #array(expr: $e.elems[$i], size: $e.count) ) )
    }

Linux
--------------

Just 'make' in the root should do the trick. This build system is work in progress as the linux port is very early, so it may change!

There's no configuration or cmake setup, it assumes gcc/g++ (this can be overwridden via variables CC and CPP, or just in the makefiles).

Requirements are linking against -lX11 and -lGL. For qrenderdoc you need qt5 along with the 'x11extras' package.

This is the apt-get line you'd need to install the requirements on Ubuntu 15.04:

```
sudo apt-get install libx11-dev mesa-common-dev libgl1-mesa-dev qt5-default libqt5x11extras5-dev
```

For Archlinux (as of 2016.02.01) you'll need:

```
sudo pacman -S libx11 xcb-util-keysyms mesa mesa-libgl qt5-base qt5-x11extras
```

If you know the required packages for another distribution, please share (or pull request this file!)

Builds
--------------

Official releases will get a github release made for them, nightly builds and beta are just marked with the hash of the commit they were built from.

Please don't distribute releases marked with a version number and commit hash as it will confuse me with auto-submitted crashes since I won't have the symbols for them. If you distribute releases leave the version information as is (master is always marked as an unofficial non-versioned build).

