#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include <Tlhelp32.h>
#include <atlconv.h>

uintptr_t client;

uintptr_t AutoOffsetSearch(std::string Signature, uintptr_t ModuleAddress);

uintptr_t s_dwEntityList_Offset;
uintptr_t localplayer_Offset;
uintptr_t viewmatrix_Offset;

uintptr_t s_dwPlayerPawn_Offset = 0x7EC;
uintptr_t s_teamnuOffset = 0x3BF;
uintptr_t s_IsAlive = 0x7f4;
uintptr_t s_bonearray = 0x1E0;
uintptr_t s_gamescene = 0x310;
uintptr_t s_bIsLocalPlayerController_Offset = 0x6d0;

class ShovenManager
{
private:
public:
	HANDLE hProcess;
	DWORD  ProcessID;
	bool initcheat;
	void InitCheat()
	{
		if (!initcheat)
		{
			ProcessID = GetCurrentProcessId();
			hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD, TRUE, ProcessID);
			client = (uintptr_t)GetModuleHandleA("client.dll");
			uintptr_t otoofset = 0;
			otoofset = AutoOffsetSearch("48 8B 0D ?? ?? ?? ?? 48 89 7C 24 ?? 8B FA C1", client);
			s_dwEntityList_Offset = otoofset - client;
			otoofset = AutoOffsetSearch("48 8D 05 ?? ?? ?? ?? C3 CC CC CC CC CC CC CC CC 48 83 EC ?? 8B 0D", client);
			localplayer_Offset = otoofset + 0x138 - client;
			otoofset = AutoOffsetSearch("48 8D 0D ?? ?? ?? ?? 48 C1 E0 06", client);
			viewmatrix_Offset = otoofset - client;
			otoofset = AutoOffsetSearch("48 8B 05 ?? ?? ?? ?? 48 8D 1D ?? ?? ?? ?? 48 89 45", client);
			initcheat = true;
		}
	}
	template <typename ReadType>
	void readsize(uintptr_t Address, ReadType& Value, int Size)
	{
		ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, Size, 0);
	}
	template <typename ReadType>
	ReadType read(uintptr_t Address)
	{
		ReadType Value = {};
		ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, sizeof(ReadType), 0);
		return Value;
	}
};

inline ShovenManager shovenmemory;

template <typename... Args>
inline std::string Format(const char* pFormat, Args...args)
{
	int Length = std::snprintf(nullptr, 0, pFormat, args...);
	if (Length <= 0)
		return "";
	char* Str = new char[Length + 1];
	std::string Result;
	std::snprintf(Str, Length + 1, pFormat, args...);
	Result = std::string(Str);
	delete[] Str;
	return Result;
}

inline const DWORD BLOCKMAXSIZE = 409600;
int GetSignatureArray(const std::string& Signature, std::vector<WORD>& SignatureArray)
{
	std::string Sig = Signature;
	Sig.erase(std::remove(Sig.begin(), Sig.end(), ' '), Sig.end());

	std::size_t Size = Sig.size();

	if (Size % 2 != 0)
		return 0;

	for (int i = 0; i < Size; i += 2)
	{
		std::string ByteString = Sig.substr(i, 2);
		WORD Byte = (ByteString == "??") ? 256 : std::stoi(ByteString, nullptr, 16);
		SignatureArray.push_back(Byte);
	}
	return SignatureArray.size();
}
void GetNextArray(std::vector<short>& NextArray, const std::vector<WORD>& SignatureArray)
{
	std::size_t Size = SignatureArray.size();
	for (int i = 0; i < Size; i++)
		NextArray[SignatureArray[i]] = i;
}
void SearchMemoryBlock(byte* MemoryBuffer, const std::vector<short>& NextArray, const std::vector<WORD>& SignatureArray, uintptr_t StartAddress, DWORD Size, std::vector<uintptr_t>& ResultArray)
{
	shovenmemory.readsize(StartAddress, *MemoryBuffer, Size);

	int SignatureLength = SignatureArray.size();

	for (int i = 0, j, k; i < Size;)
	{
		j = i; k = 0;

		for (; k < SignatureLength && j < Size && (SignatureArray[k] == MemoryBuffer[j] || SignatureArray[k] == 256); k++, j++);

		if (k == SignatureLength)
			ResultArray.push_back(StartAddress + i);

		if ((i + SignatureLength) >= Size)
			return;

		int Num = NextArray[MemoryBuffer[i + SignatureLength]];
		if (Num == -1)
			i += (SignatureLength - NextArray[256]);
		else
			i += (SignatureLength - Num);
	}
}
std::vector<uintptr_t> SearchMemory(const std::string& Signature, uintptr_t StartAddress, uintptr_t EndAddress, int SearchNum)
{
	std::vector<uintptr_t> ResultArray;
	std::vector<WORD> SignatureArray;
	std::vector<short> NextArray(260, -1);

	byte* MemoryBuffer = new byte[BLOCKMAXSIZE];

	if (GetSignatureArray(Signature, SignatureArray) <= 0)
		return ResultArray;

	GetNextArray(NextArray, SignatureArray);

	MEMORY_BASIC_INFORMATION mbi;
	int Count;
	while (VirtualQueryEx(shovenmemory.hProcess, reinterpret_cast<LPCVOID>(StartAddress), &mbi, sizeof(mbi)) != 0)
	{
		Count = 0;
		auto BlockSize = mbi.RegionSize;

		while (BlockSize >= BLOCKMAXSIZE)
		{
			if (ResultArray.size() >= SearchNum)
				goto END;

			SearchMemoryBlock(MemoryBuffer, NextArray, SignatureArray, StartAddress + (BLOCKMAXSIZE * Count), BLOCKMAXSIZE, ResultArray);

			BlockSize -= BLOCKMAXSIZE;
			Count++;
		}

		SearchMemoryBlock(MemoryBuffer, NextArray, SignatureArray, StartAddress + (BLOCKMAXSIZE * Count), BlockSize, ResultArray);

		StartAddress += mbi.RegionSize;

		if (ResultArray.size() >= SearchNum || EndAddress != 0 && StartAddress > EndAddress)
			break;
	}

END:

	delete[] MemoryBuffer;
	return ResultArray;
}
uintptr_t AutoOffsetSearch(std::string Signature, uintptr_t ModuleAddress)
{
	std::vector<uintptr_t> TempAddressList;
	uintptr_t Address = 0;
	TempAddressList = SearchMemory(Signature, ModuleAddress, ModuleAddress + 0x4000000, 1);
	if (TempAddressList.size() <= 0) return 0;
	DWORD Offsets = shovenmemory.read<DWORD>(TempAddressList.at(0) + 3);
	Address = TempAddressList.at(0) + Offsets + 7;
	return Address;
}