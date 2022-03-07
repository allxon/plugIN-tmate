export ENV=x86 
export APP_GUID=${APP_GUID}  
cd linux-plugin-sdk  
make  
cp x86/release_static/libadmplugin.a ../x86/lib/  
cd ../  
make