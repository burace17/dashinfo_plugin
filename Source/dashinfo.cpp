//‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
//›                                                                         ﬁ
//› Module: Internals Example Source File                                   ﬁ
//›                                                                         ﬁ
//› Description: Declarations for the Internals Example Plugin              ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› This source code module, and all information, data, and algorithms      ﬁ
//› associated with it, are part of CUBE technology (tm).                   ﬁ
//›                 PROPRIETARY AND CONFIDENTIAL                            ﬁ
//› Copyright (c) 1996-2007 Image Space Incorporated.  All rights reserved. ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› Change history:                                                         ﬁ
//›   tag.2005.11.30: created                                               ﬁ
//›                                                                         ﬁ
//ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

#include "dashinfo.hpp"          // corresponding header file
#include <math.h>               // for atan2, sqrt
#include <stdio.h>              // for sample output


// plugin information
unsigned g_uPluginID          = 0;
char     g_szPluginName[]     = "DashInfo";
unsigned g_uPluginVersion     = 001;
unsigned g_uPluginObjectCount = 1;
DashInfo g_PluginInfo;

// interface to plugin information
extern "C" __declspec(dllexport)
const char* __cdecl GetPluginName() { return g_szPluginName; }

extern "C" __declspec(dllexport)
unsigned __cdecl GetPluginVersion() { return g_uPluginVersion; }

extern "C" __declspec(dllexport)
unsigned __cdecl GetPluginObjectCount() { return g_uPluginObjectCount; }

// get the plugin-info object used to create the plugin.
extern "C" __declspec(dllexport)
PluginObjectInfo* __cdecl GetPluginObjectInfo( const unsigned uIndex )
{
  switch(uIndex)
  {
    case 0:
      return  &g_PluginInfo;
    default:
      return 0;
  }
}


// DashInfo class

DashInfo::DashInfo()
{
  // put together a name for this plugin
  sprintf( m_szFullName, "%s - %s", g_szPluginName, DashInfo::GetName() );
}

const char*    DashInfo::GetName()     const { return DashInfoPlugin::GetName(); }
const char*    DashInfo::GetFullName() const { return m_szFullName; }
const char*    DashInfo::GetDesc()     const { return "Sends telemetry over UDP to a dashboard client"; }
const unsigned DashInfo::GetType()     const { return DashInfoPlugin::GetType(); }
const char*    DashInfo::GetSubType()  const { return DashInfoPlugin::GetSubType(); }
const unsigned DashInfo::GetVersion()  const { return DashInfoPlugin::GetVersion(); }
void*          DashInfo::Create()      const { return new DashInfoPlugin(); }


// InternalsPlugin class

const char DashInfoPlugin::m_szName[] = "DashInfo";
const char DashInfoPlugin::m_szSubType[] = "Internals";
const unsigned DashInfoPlugin::m_uID = 1;
const unsigned DashInfoPlugin::m_uVersion = 3;  // set to 3 for InternalsPluginV3 functionality and added graphical and vehicle info


// WinSock object
WSAData wsaData;

// Socket
SOCKET s;
sockaddr_in dest;

bool realTime = false;

PluginObjectInfo *DashInfoPlugin::GetInfo()
{
  return &g_PluginInfo;
}

void DashInfoPlugin::SendDashMessage(char msg, int data) 
{
	char message[50];
	memset(message, 0, sizeof(message));
	sprintf(message, "%c%d", msg, data);
	sendto(s, message, sizeof(message), 0, (const sockaddr*)&dest, sizeof(dest));
}

void DashInfoPlugin::SendDashMessage(char msg, float data)
{
	char message[50];
	memset(message, 0, sizeof(message));
	sprintf(message, "%c%.1f", msg, data);
	sendto(s, message, sizeof(message), 0, (const sockaddr*)&dest, sizeof(dest));
}

void DashInfoPlugin::Startup()
{
	// Startup winsock
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	dest.sin_family = AF_INET;
	dest.sin_port = htons(1337);
	InetPton(AF_INET, "192.168.1.114", &dest.sin_addr.S_un.S_addr);
}


void DashInfoPlugin::Shutdown()
{
	closesocket(s);
	WSACleanup();
}


void DashInfoPlugin::StartSession()
{
 
}


void DashInfoPlugin::EndSession()
{

}


void DashInfoPlugin::EnterRealtime()
{
  mET = 0.0f;
  realTime = true;
}


void DashInfoPlugin::ExitRealtime()
{
	realTime = false;
}


void DashInfoPlugin::UpdateTelemetry( const TelemInfoV2 &info )
{
	if (!realTime)
		return;

	SendDashMessage(MAXRPM, (int)info.mEngineMaxRPM);
	SendDashMessage(RPM, (int)info.mEngineRPM);
	SendDashMessage(OILTEMP, (int)info.mEngineOilTemp);
	SendDashMessage(WATERTEMP, (int)info.mEngineWaterTemp);
	SendDashMessage(FUEL, (int)info.mFuel);
	SendDashMessage(GEAR, info.mGear);

	SendDashMessage(LAPTIME, info.mLapStartET);
	SendDashMessage(LAPNUMBER, info.mLapNumber);
	
	SendDashMessage(LF_TIRE_TEMP, (int)info.mWheel[0].mTemperature[2]);
	SendDashMessage(RF_TIRE_TEMP, (int)info.mWheel[1].mTemperature[2]);
	SendDashMessage(LR_TIRE_TEMP, (int)info.mWheel[2].mTemperature[2]);
	SendDashMessage(RR_TIRE_TEMP, (int)info.mWheel[3].mTemperature[2]);

	SendDashMessage(LF_BRAKE_TEMP, (int)info.mWheel[0].mBrakeTemp);
	SendDashMessage(RF_BRAKE_TEMP, (int)info.mWheel[1].mBrakeTemp);
	SendDashMessage(LR_BRAKE_TEMP, (int)info.mWheel[2].mBrakeTemp);
	SendDashMessage(RR_BRAKE_TEMP, (int)info.mWheel[3].mBrakeTemp);
 
}


bool DashInfoPlugin::CheckHWControl( const char * const controlName, float &fRetVal )
{
  return( false );
}


bool DashInfoPlugin::ForceFeedback( float &forceValue )
{
  return( false );
}


void DashInfoPlugin::UpdateScoring( const ScoringInfoV2 &info )
{
	if (!realTime)
		return;
	SendDashMessage(MAXLAPS, info.mMaxLaps);
}


void DashInfoPlugin::UpdateGraphics(const GraphicsInfoV2 &info)
{

}

bool DashInfoPlugin::RequestCommentary( CommentaryRequestInfo &info )
{
  return( false );
}

