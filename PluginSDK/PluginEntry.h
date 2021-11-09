#pragma once
#ifndef     PLUGIN_ENTRY_H_
#define     PLUGIN_ENTRY_H_

#if defined(_WINDOWS) && !defined(_DOT_NET_BUILD)
    #ifdef DEVICEPLUGIN_EXPORTS
    #define PLUGIN_API __declspec(dllexport)
    #else
    #define PLUGIN_API __declspec(dllimport)
    #endif
#else
    #define PLUGIN_API
#endif // _WINDOWS

#endif