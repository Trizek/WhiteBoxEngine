:: Clean
:: rmdir /s /q ..\..\Core\Libs\GearVR\VrSamples\Native\VrCubeWorld_SurfaceView
rmdir /s /q ..\..\Core\Libs\GearVR\WhiteBoxGearLib

:: Build WhitexBox gear library
::cmd /c ""%VS140COMNTOOLS%"\VsMsBuildCmd.bat"
::cmd /c MsBuild WhiteBoxGearLib\WhiteBoxGearLib.sln /property:Configuration=Release /property:Platform=ARM
 ::/t:WhiteBoxGearLib:Rebuild

:: Copy the library
:: xcopy WhiteBoxGearLib ..\..\Core\Libs\GearVR\WhiteBoxGearLib /s /i /h /e /y

:: Copy and build WhitexBox gear vr application
xcopy VrCubeWorld_SurfaceView ..\..\Core\Libs\GearVR\VrSamples\Native\VrCubeWorld_SurfaceView /s /i /h /e /y
xcopy OsigFiles\* ..\..\Core\Libs\GearVR\VrSamples\Native\VrCubeWorld_SurfaceView\assets\ /y

:: Export assets to Android
"..\Windows\Cook for Android\WhiteBoxEngine.exe" ../../Assets ../../Core/Libs/GearVR/VrSamples/Native/VrCubeWorld_SurfaceView/assets


cd ../../Core/Libs/GearVR/VrSamples/Native/VrCubeWorld_SurfaceView/Projects/Android



cmd /c build

cd ../../../../../../../../Solution/GearVR