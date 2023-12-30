#pragma once

#include <map>
#include <string>
#include <windef.h>
#include <detours.h>

class HookManager {
	inline static std::map<LPVOID, LPVOID> m_detours;
public:
	template <typename Function, typename Detour>
	static BOOL Attach(Function function, Detour detour) {
		m_detours[(LPVOID)detour] = (LPVOID)function;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		BOOL status = DetourAttach(&m_detours[(LPVOID)detour], (LPVOID)detour);
		DetourTransactionCommit();

		if (status) {
			DetourTransactionAbort();
		}

		return status;
	}

	template <typename Detour>
	static BOOL Detach(Detour detour) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		BOOL status = DetourDetach(&m_detours[(LPVOID)detour], (LPVOID)detour);
		DetourTransactionCommit();

		if (status) {
			DetourTransactionAbort();
		}
		else {
			m_detours.erase((LPVOID)detour);
		}

		return status;
	}

	template <typename Detour>
	static Detour GetOrigin(Detour detour) {
		return (Detour)m_detours[(LPVOID)detour];
	}

	template <typename Return, typename ...Args>
	static Return Invoke(Return(__cdecl* detour)(Args...), Args ... args) {
		auto origin = GetOrigin(detour);

		return origin ? origin(args...) : Return();
	}

	template <typename Return, typename ...Args>
	static Return Invoke(Return(__stdcall* detour)(Args...), Args ... args) {
		auto origin = GetOrigin(detour);

		return origin ? origin(args...) : Return();
	}

	template <typename Return, typename ...Args>
	static Return Invoke(Return(__fastcall* detour)(Args...), Args ... args) {
		auto origin = GetOrigin(detour);

		return origin ? origin(args...) : Return();
	}
};