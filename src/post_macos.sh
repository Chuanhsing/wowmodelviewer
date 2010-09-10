#!/bin/bash

# copy need file for package
cp /sw/lib/libexpat.1.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libGLEW.1.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libiconv.2.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libjpeg.62.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libjpeg.8.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libpng12.0.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libpng.3.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libtiff.3.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libwx_base_carbon-2.8.0.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libwx_base_carbon_net-2.8.0.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libwx_base_carbon_xml-2.8.0.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libwx_mac_adv-2.8.0.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libwx_mac_aui-2.8.0.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libwx_mac_core-2.8.0.dylib wowmodelviewer.app/Contents/MacOS/
cp /sw/lib/libwx_mac_gl-2.8.0.dylib wowmodelviewer.app/Contents/MacOS/
mkdir -p wowmodelviewer.app/Contents/Resources/
cp ../bin_support/Icons/wmv.icns wowmodelviewer.app/Contents/Resources/
