#pragma once

#include <Windows.h>

class MODULE {
public:
	union {
		HMODULE module = nullptr;
		DWORD base;
	};

	MODULE() {}
	MODULE(HMODULE instance): module(instance){}
	MODULE(const MODULE& module): module(module.module) {}

	bool operator=(HMODULE module) {
		this->module = module;
	}

	bool operator=(DWORD base) {
		this->base = base;
	}

	DWORD operator+(DWORD offset) {
		return base ? base + offset : NULL;
	}

	bool operator==(MODULE module) {
		return this->module == module.module;
	}

	bool operator==(HMODULE module) {
		return this->module == module;
	}

	bool operator==(DWORD base) {
		return this->base == base;
	}

	bool operator!() {
		return !module;
	}

	explicit operator bool() {
		return module != nullptr;
	}
};

template <typename UnusedType>
class function;

template <typename ReturnType, typename ... ArgumentTypes>
class function<ReturnType(ArgumentTypes ...)> {
protected:
	void* m_address = nullptr;
public:
	function() {}

	template <typename FunctionType>
	function(FunctionType function): m_address((void*)function) {}

	ReturnType operator()(ArgumentTypes... args) {
		return m_address ? reinterpret_cast<ReturnType(__cdecl*)(ArgumentTypes...)>(m_address)(args...) : ReturnType();
	}

	template <typename FunctionType>
	function& operator=(FunctionType function) {
		m_address = (void*)function;

		return *this;
	}

	void*& address() {
		return m_address;
	}

	template <typename FunctionType>
	bool operator==(FunctionType function) {
		return m_address == (void*)function;
	}

	explicit operator bool() {
		return m_address != nullptr;
	}

	explicit operator void* () {
		return m_address;
	}
};

template <typename UnusedType>
class sfunction;

template <typename ReturnType, typename ... ArgumentTypes>
class sfunction<ReturnType(ArgumentTypes ...)> {
protected:
	void* m_address = nullptr;
public:
	sfunction() {}

	template <typename FunctionType>
	sfunction(FunctionType function) : m_address((void*)function) {}

	ReturnType operator()(ArgumentTypes... args) {
		return m_address ? reinterpret_cast<ReturnType(__stdcall*)(ArgumentTypes...)>(m_address)(args...) : ReturnType();
	}

	template <typename FunctionType>
	sfunction& operator=(FunctionType function) {
		m_address = (void*)function;

		return *this;
	}

	void*& address() {
		return m_address;
	}

	template <typename FunctionType>
	bool operator==(FunctionType function) {
		return m_address == (void*)function;
	}

	explicit operator bool() {
		return m_address != nullptr;
	}

	explicit operator void* () {
		return m_address;
	}
};

template <typename UnusedType>
class ffunction;

template <typename ReturnType, typename ... ArgumentTypes>
class ffunction<ReturnType(ArgumentTypes ...)> {
protected:
	void* m_address = nullptr;
public:
	ffunction() {}

	template <typename FunctionType>
	ffunction(FunctionType function) : m_address((void*)function) {}

	ReturnType operator()(ArgumentTypes... args) {
		return m_address ? reinterpret_cast<ReturnType(__fastcall*)(ArgumentTypes...)>(m_address)(args...) : ReturnType();
	}

	template <typename FunctionType>
	ffunction& operator=(FunctionType function) {
		m_address = (void*)function;

		return *this;
	}

	void*& address() {
		return m_address;
	}

	template <typename FunctionType>
	bool operator==(FunctionType function) {
		return m_address == (void*)function;
	}

	explicit operator bool() {
		return m_address != nullptr;
	}

	explicit operator void* () {
		return m_address;
	}
};

template <typename UnusedType>
class tfunction;

template <typename ReturnType, typename This, typename ... ArgumentTypes>
class tfunction<ReturnType(This, ArgumentTypes ...)> {
protected:
	void* m_address = nullptr;
public:
	tfunction() {}

	template <typename FunctionType>
	tfunction(FunctionType function) : m_address((void*)function) {}

	ReturnType operator()(This self, ArgumentTypes... args) {
		return m_address ? reinterpret_cast<ReturnType(__thiscall*)(This, ArgumentTypes...)>(m_address)(self, args...) : ReturnType();
	}

	template <typename FunctionType>
	tfunction& operator=(FunctionType function) {
		m_address = (void*)function;

		return *this;
	}

	void*& address() {
		return m_address;
	}

	template <typename FunctionType>
	bool operator==(FunctionType function) {
		return m_address == (void*)function;
	}

	explicit operator bool() {
		return m_address != nullptr;
	}

	explicit operator void* () {
		return m_address;
	}
};