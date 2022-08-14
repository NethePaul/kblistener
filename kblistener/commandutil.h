#pragma once
#include"CircularBuffer.h"
#include<string>
constexpr auto CMDLEN = 10000;
constexpr auto VK_IGNORE = 0x88;
constexpr auto VK_SEND = 0x89;
constexpr auto VK_REPLAYING = 0x8A;
constexpr auto WM_SENDBACK = WM_USER + 1;

void repnext();
void cont();
void receivedanswer();
void send(const INPUT&in);
void del(int i);
int loadintparam(CircularBuffer<wchar_t, CMDLEN>&command);
bool singlewordfilter(wchar_t);//filters out only a singular word
bool textfilter(wchar_t);
std::string loadstrparam(CircularBuffer<wchar_t, CMDLEN>&command,bool(*filter)(wchar_t)=singlewordfilter);
void popparam(CircularBuffer<wchar_t, CMDLEN>&command);