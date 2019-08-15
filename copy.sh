#!/bin/sh
cp -rf ./release/include ./weldroidProject/include
cd weldroidProject/include
rm -rf walgo
mv include walgo
cd ../../
cp -rf ./release/lib/* ./weldroidProject/lib
cd weldroidProject/lib
chmod +x *

