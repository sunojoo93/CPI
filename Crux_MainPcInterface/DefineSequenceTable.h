#pragma once

// ´Ù¸¥ Task No
#define VS_UI_TASK									10
#define VS_ALGORITHM_TASK							50	+ (theApp.GetPCNum() * 100)
#define VS_MAIN_PC_TASK								60	+ (theApp.GetPCNum() * 100)
#define VS_MAIN_PC_SIMULATOR_TASK					61	+ (theApp.GetPCNum() * 100)
#define VS_LIGHT_TASK								80	+ (theApp.GetPCNum() * 100)
#define VS_CAMERA_TASK								90	+ (theApp.GetPCNum() * 100)
#define	VS_SEQUENCE_TASK							21	+ (theApp.GetPCNum() * 100)

// UI
#define SEND_CHANGE_MODEL							10, 1
#define SEND_THETA_ON_OFF							10, 5

// Algoithm
#define SEND_INSPECT_START							21, 1

// Light
#define SEND_LIGHT_ON								80, 13
#define SEND_LIGHT_OFF								80, 14

// Camera
#define SEND_CAMERA_EXPOSE							90, 11
#define SEND_WAIT_CAMERA_GRAB_END					90, 12
#define SEND_SET_CAMERA_COND						90, 30

// Sequence
#define START_GRAB_REQ								21, 20
#define START_AF_READY							21, 23
#define START_GRAB_END							21, 24
// Main PC Interface
#define START_MAIN_PC_IF_SOCKET						20, 1

#define START_GRAB_REQ_FROM_MAIN_PC					60, 1
#define RECIPE_CHANGE_REQ_FROM_MAIN_PC				60, 2
#define GRAB_END				60, 5
#define AF_READY				60, 23

#define WAIT_CLASSIFY_END							90, 1